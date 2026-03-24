#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <cmath>

// =========================================================
// WGS84 & UTM Parameters for Coordinate Conversion
// =========================================================
const double RADIANS_PER_DEGREE = M_PI / 180.0;
const double WGS84_A = 6378137.0;
const double UTM_K0 = 0.9996;
const double WGS84_E = 0.0818191908;
const double UTM_E2 = (WGS84_E * WGS84_E);

inline void LLtoUTM(const double Lat, const double Long, double &UTMNorthing, double &UTMEasting) {
    double a = WGS84_A;
    double eccSquared = UTM_E2;
    double k0 = UTM_K0;

    double LongOrigin;
    double eccPrimeSquared;
    double N, T, C, A, M;

    double LongTemp = (Long + 180) - int((Long + 180) / 360) * 360 - 180;
    double LatRad = Lat * RADIANS_PER_DEGREE;
    double LongRad = LongTemp * RADIANS_PER_DEGREE;
    double LongOriginRad;
    int ZoneNumber = int((LongTemp + 180) / 6) + 1;

    if(Lat >= 56.0 && Lat < 64.0 && LongTemp >= 3.0 && LongTemp < 12.0) ZoneNumber = 32;
    if(Lat >= 72.0 && Lat < 84.0) {
      if(LongTemp >= 0.0  && LongTemp <  9.0) ZoneNumber = 31;
      else if(LongTemp >= 9.0  && LongTemp < 21.0) ZoneNumber = 33;
      else if(LongTemp >= 21.0 && LongTemp < 33.0) ZoneNumber = 35;
      else if(LongTemp >= 33.0 && LongTemp < 42.0) ZoneNumber = 37;
    }

    LongOrigin = (ZoneNumber - 1) * 6 - 180 + 3;
    LongOriginRad = LongOrigin * RADIANS_PER_DEGREE;

    eccPrimeSquared = (eccSquared) / (1 - eccSquared);
    N = a / sqrt(1 - eccSquared * sin(LatRad) * sin(LatRad));
    T = tan(LatRad) * tan(LatRad);
    C = eccPrimeSquared * cos(LatRad) * cos(LatRad);
    A = cos(LatRad) * (LongRad - LongOriginRad);

    M = a * ((1 - eccSquared/4 - 3*eccSquared*eccSquared/64 - 5*eccSquared*eccSquared*eccSquared/256)*LatRad
           - (3*eccSquared/8 + 3*eccSquared*eccSquared/32 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatRad)
           + (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatRad)
           - (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatRad));

    UTMEasting = (double)(k0 * N * (A + (1-T+C)*A*A*A/6
                 + (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120)
                 + 500000.0);

    UTMNorthing = (double)(k0 * (M + N * tan(LatRad) * (A*A/2 + (5-T+9*C+4*C*C)*A*A*A*A/24
                  + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));
    if(Lat < 0) UTMNorthing += 10000000.0;
}

class UTMLocalOdomNode : public rclcpp::Node {
    // =========================================================
    //  USER CONFIGURATION
    // =========================================================
    const std::string MY_FRAME_ID = "gps_link"; // TF Frame ID ของ GPS
    const double YAW_OFFSET = 0.0;              // ชดเชยองศาของทิศเหนือ (rad) 
    const double MAGNETIC_DECLINATION = 0.0;    // ชดเชยทิศเหนือแม่เหล็ก (rad)
    const bool ZERO_ALTITUDE = true;            // บังคับแกน Z เป็น 0 เสมอ
    // =========================================================

public:
    UTMLocalOdomNode() : Node("utm_local_odom_node") {
        sub_fix_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(
            "/fix", 10, std::bind(&UTMLocalOdomNode::fix_callback, this, std::placeholders::_1));
        
        pub_odom_ = this->create_publisher<nav_msgs::msg::Odometry>("/peplink/odom", 10);
        
        RCLCPP_INFO(this->get_logger(), "🗺️ UTM Local Odom Node Started!");
        RCLCPP_INFO(this->get_logger(), "Subscribed to: /fix | Publishing to: /peplink/odom");
    }

private:
    void fix_callback(const sensor_msgs::msg::NavSatFix::SharedPtr msg) {
        if (msg->status.status == sensor_msgs::msg::NavSatStatus::STATUS_NO_FIX) {
            return;
        }

        double lat = msg->latitude;
        double lon = msg->longitude;
        double alt = msg->altitude;

        double utm_northing, utm_easting;
        LLtoUTM(lat, lon, utm_northing, utm_easting);

        // Calculate error from covariance to check stability
        double var_e = msg->position_covariance[0];
        double var_n = msg->position_covariance[4];
        double var_u = msg->position_covariance[8];
        
        double err_e = std::sqrt(var_e);
        double err_n = std::sqrt(var_n);

        // Stable check (Error < 4.0 meters)
        bool is_stable = (err_e < 4.0 && err_n < 4.0);

        if (!has_datum_) {
            if (!is_stable) {
                RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 2000,
                    "⏳ Waiting for STABLE GPS to set Datum... (Current Err: %.2f m)", 
                    std::max(err_e, err_n));
                return;
            }

            datum_easting_ = utm_easting;
            datum_northing_ = utm_northing;
            datum_yaw_ = YAW_OFFSET + MAGNETIC_DECLINATION;
            has_datum_ = true;
            RCLCPP_INFO(this->get_logger(), "📍 INITIAL DATUM SET! UTM East: %.2f, North: %.2f", utm_easting, utm_northing);
        }

        // Subtract Origin
        double local_utm_x = utm_easting - datum_easting_;
        double local_utm_y = utm_northing - datum_northing_;

        // Rotation (Heading)
        double theta = datum_yaw_;
        double cos_th = std::cos(theta);
        double sin_th = std::sin(theta);

        // Apply 2D Rotation matrix
        double robot_x = local_utm_x * cos_th + local_utm_y * sin_th;
        double robot_y = -local_utm_x * sin_th + local_utm_y * cos_th;
        double robot_z = ZERO_ALTITUDE ? 0.0 : alt;

        auto msg_odom = nav_msgs::msg::Odometry();
        msg_odom.header.stamp = msg->header.stamp;
        msg_odom.header.frame_id = "map";
        msg_odom.child_frame_id = MY_FRAME_ID;

        msg_odom.pose.pose.position.x = robot_x;
        msg_odom.pose.pose.position.y = robot_y;
        msg_odom.pose.pose.position.z = robot_z;

        // Pass covariance through
        msg_odom.pose.covariance[0] = var_e; 
        msg_odom.pose.covariance[7] = var_n; 
        msg_odom.pose.covariance[14] = var_u; 

        pub_odom_->publish(msg_odom);
    }

    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr sub_fix_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_odom_;

    bool has_datum_ = false;
    double datum_easting_ = 0.0;
    double datum_northing_ = 0.0;
    double datum_yaw_ = 0.0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UTMLocalOdomNode>());
    rclcpp::shutdown();
    return 0;
}

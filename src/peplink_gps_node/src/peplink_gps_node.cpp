#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

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


class PeplinkGPSDriver : public rclcpp::Node {
    // =========================================================
    //  USER CONFIGURATION
    // =========================================================
    const int MY_UDP_PORT = 8384;               // Port ที่ตั้งใน Peplink
    const std::string MY_FRAME_ID = "gps_link"; // TF Frame ID ของ GPS
    const double GPS_UERE = 3.0;                // ความคลาดเคลื่อนมาตรฐาน

    // Datum Configuration สำหรับแปลง XY
    const double YAW_OFFSET = 0.0;              // ชดเชยองศาของทิศเหนือ (rad) 
    const double MAGNETIC_DECLINATION = 0.0;    // ชดเชยทิศเหนือแม่เหล็ก (rad)
    const bool ZERO_ALTITUDE = true;            // บังคับแกน Z เป็น 0 เสมอ
    // =========================================================

public:
    PeplinkGPSDriver() : Node("peplink_gps_driver") {
        // Publishers
        pub_fix_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/fix", 10);
        pub_odom_ = this->create_publisher<nav_msgs::msg::Odometry>("/peplink/odom", 10); // NEW
        
        // Setup UDP
        init_udp_socket();
        
        // Timer 100Hz
        timer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&PeplinkGPSDriver::read_udp, this));
        
        RCLCPP_INFO(this->get_logger(), "🚀 Peplink GPS Driver (with Local Odom) Started!");
        RCLCPP_INFO(this->get_logger(), "Listening UDP Port: %d | Publishing to: /fix and /peplink/odom", MY_UDP_PORT);
    }

    ~PeplinkGPSDriver() {
        if (sockfd_ > 0) close(sockfd_);
    }

private:
    void init_udp_socket() {
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to create socket!");
            return;
        }

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(MY_UDP_PORT); 

        if (bind(sockfd_, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            RCLCPP_ERROR(this->get_logger(), "Bind failed on port %d", MY_UDP_PORT);
            return;
        }
        
        fcntl(sockfd_, F_SETFL, O_NONBLOCK); 
    }

    void read_udp() {
        char buffer[2048];
        struct sockaddr_in cliaddr; 
        socklen_t len = sizeof(cliaddr);
        
        while (true) {
            int n = recvfrom(sockfd_, buffer, 2048, 0, (struct sockaddr *)&cliaddr, &len);
            if (n < 0) break;
            
            buffer[n] = '\0';
            process_nmea_stream(std::string(buffer));
        }
    }

    void process_nmea_stream(std::string data) {
        std::stringstream ss(data);
        std::string line;
        
        while (std::getline(ss, line, '\n')) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            
            if (line.find("$GPGGA") != std::string::npos) parse_gpgga(line);
            else if (line.find("$GPGSA") != std::string::npos) parse_gpgsa(line);
            else if (line.find("$GPGST") != std::string::npos) parse_gpgst(line);
        }
    }

    void parse_gpgsa(const std::string& line) {
        auto p = split_nmea(line);
        if (p.size() >= 18) {
             try {
                if (!p[16].empty()) last_hdop_ = std::stod(p[16]); 
                if (!p[17].empty()) last_vdop_ = std::stod(p[17]);
             } catch (...) {}
        }
    }

    void parse_gpgst(const std::string& line) {
        auto p = split_nmea(line);
        if (p.size() >= 9) {
            try {
                if (!p[6].empty()) gst_lat_err_ = std::stod(p[6]); 
                if (!p[7].empty()) gst_lon_err_ = std::stod(p[7]); 
                if (!p[8].empty()) gst_alt_err_ = std::stod(p[8]); 
                has_gst_ = true;
            } catch (...) {}
        }
    }

    void parse_gpgga(const std::string& line) {
        auto p = split_nmea(line);
        if (p.size() > 9 && !p[2].empty() && !p[4].empty()) {
            auto stamp = this->now();

            try {
                double lat = convert_nmea_to_deg(p[2], p[3]);
                double lon = convert_nmea_to_deg(p[4], p[5]);
                double alt = p[9].empty() ? 0.0 : std::stod(p[9]);

                // ----------------------------------------------------
                // 1. Publish Raw GPS (/fix)
                // ----------------------------------------------------
                auto msg_fix = sensor_msgs::msg::NavSatFix();
                msg_fix.header.stamp = stamp;
                msg_fix.header.frame_id = MY_FRAME_ID;
                msg_fix.latitude = lat;
                msg_fix.longitude = lon;
                msg_fix.altitude = alt;

                double var_e = 0.0, var_n = 0.0, var_u = 0.0;
                if (has_gst_) {
                    var_n = std::pow(gst_lat_err_, 2);
                    var_e = std::pow(gst_lon_err_, 2);
                    var_u = std::pow(gst_alt_err_, 2);
                    msg_fix.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_DIAGONAL_KNOWN;
                } else {
                    double hdop = p[8].empty() ? last_hdop_ : std::stod(p[8]);
                    var_n = std::pow(hdop * GPS_UERE, 2); 
                    var_e = std::pow(hdop * GPS_UERE, 2); 
                    var_u = std::pow(last_vdop_ * GPS_UERE, 2); 
                    msg_fix.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_APPROXIMATED;
                }

                msg_fix.position_covariance[0] = var_e; 
                msg_fix.position_covariance[4] = var_n; 
                msg_fix.position_covariance[8] = var_u; 
                pub_fix_->publish(msg_fix);

                // ----------------------------------------------------
                // 2. Process Local Odom XY (Simulate navsat_transform)
                // ----------------------------------------------------
                double utm_northing, utm_easting;
                LLtoUTM(lat, lon, utm_northing, utm_easting);

                // Set Datum if it's the first time receiving GPS
                // Set Datum ONLY IF GPS signal is good enough (Stable)
                // 1. Must have good HDOP (< 1.5) OR GST error < 1.0 meter
                bool is_stable = (has_gst_ && gst_lat_err_ < 4.0 && gst_lon_err_ < 4.0) || (!has_gst_ && last_hdop_ < 2.0);

                if (!has_datum_) {
                    if (!is_stable) {
                        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 2000,
                            "⏳ Waiting for STABLE GPS to set Datum... (HDOP: %.2f, GST Err: %.2f m)",
                            last_hdop_, has_gst_ ? gst_lat_err_ : -1.0);
                        return; // Skip calculating Local Odom until we get a good Datum
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

                // ----------------------------------------------------
                // 3. Publish Local Odom (/peplink/odom)
                // ----------------------------------------------------
                auto msg_odom = nav_msgs::msg::Odometry();
                msg_odom.header.stamp = stamp;
                msg_odom.header.frame_id = "map";           // Base frame (Datum)
                msg_odom.child_frame_id = MY_FRAME_ID;      // Robot GPS frame

                msg_odom.pose.pose.position.x = robot_x;
                msg_odom.pose.pose.position.y = robot_y;
                msg_odom.pose.pose.position.z = robot_z;

                // Use the same covariance (approximate diagonal for XY)
                msg_odom.pose.covariance[0] = var_e;  // X variance
                msg_odom.pose.covariance[7] = var_n;  // Y variance
                msg_odom.pose.covariance[14] = var_u; // Z variance

                pub_odom_->publish(msg_odom);

            } catch (const std::exception& e) {
                RCLCPP_WARN(this->get_logger(), "Error parsing GPGGA: %s", e.what());
            }
        }
    }

    // --- Helpers ---
    std::vector<std::string> split_nmea(const std::string& line) {
        std::vector<std::string> result;
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, ',')) {
            size_t asterisk = item.find('*');
            if (asterisk != std::string::npos) item = item.substr(0, asterisk);
            result.push_back(item);
        }
        return result;
    }

    double convert_nmea_to_deg(const std::string& val, const std::string& dir) {
        if (val.empty()) return 0.0;
        double raw = std::stod(val);
        int deg = static_cast<int>(raw / 100);
        double dec = deg + ((raw - (deg * 100)) / 60.0);
        return (dir == "S" || dir == "W") ? -dec : dec;
    }

    // UDP & State Members
    int sockfd_ = -1;
    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr pub_fix_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_odom_;
    rclcpp::TimerBase::SharedPtr timer_;

    // DOP / GST
    double last_hdop_ = 5.0; 
    double last_vdop_ = 5.0;
    bool has_gst_ = false;
    double gst_lat_err_ = 5.0, gst_lon_err_ = 5.0, gst_alt_err_ = 10.0;

    // Datum State
    bool has_datum_ = false;
    double datum_easting_ = 0.0;
    double datum_northing_ = 0.0;
    double datum_yaw_ = 0.0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PeplinkGPSDriver>());
    rclcpp::shutdown();
    return 0;
}

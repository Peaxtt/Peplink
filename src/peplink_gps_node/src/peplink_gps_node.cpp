#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

class PeplinkGPSDriver : public rclcpp::Node {
    // =========================================================
    //  USER CONFIGURATION
    // =========================================================
    const int MY_UDP_PORT = 8384;               // Port ที่ตั้งใน Peplink (Router)
    const std::string MY_FRAME_ID = "gps_link"; // TF Frame ID ของ GPS
    const double GPS_UERE = 3.0;                // ความคลาดเคลื่อนมาตรฐานของ Peplink BR2 Pro (เมตร)
    // =========================================================

public:
    PeplinkGPSDriver() : Node("peplink_gps_driver") {
        // Init Components
        publisher_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/fix", 10);
        
        // Setup UDP Socket
        init_udp_socket();
        
        // Timer 100Hz (10ms) เพื่ออ่านข้อมูลให้ทัน Real-time
        timer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&PeplinkGPSDriver::read_udp, this));
        
        RCLCPP_INFO(this->get_logger(), " Peplink GPS Driver Started!");
        RCLCPP_INFO(this->get_logger(), " Listening on Port: %d | Frame: %s", MY_UDP_PORT, MY_FRAME_ID.c_str());
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
            RCLCPP_ERROR(this->get_logger(), "Bind failed on port %d (Check if port is already used)", MY_UDP_PORT);
            return;
        }
        
        // Set Non-blocking mode
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
        // (Dilution of Precision)
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
                if (!p[6].empty()) gst_lat_err_ = std::stod(p[6]); // Latitude Error (SD)
                if (!p[7].empty()) gst_lon_err_ = std::stod(p[7]); // Longitude Error (SD)
                if (!p[8].empty()) gst_alt_err_ = std::stod(p[8]); // Altitude Error (SD)
                has_gst_ = true;
            } catch (...) {}
        }
    }

    void parse_gpgga(const std::string& line) {
        auto p = split_nmea(line);
        if (p.size() > 9 && !p[2].empty() && !p[4].empty()) {
            auto msg = sensor_msgs::msg::NavSatFix();
            msg.header.stamp = this->now();
            msg.header.frame_id = MY_FRAME_ID;

            try {
                // 1. Position
                msg.latitude = convert_nmea_to_deg(p[2], p[3]);
                msg.longitude = convert_nmea_to_deg(p[4], p[5]);
                if (!p[9].empty()) msg.altitude = std::stod(p[9]);

                // 2. Covariance Calculation (Unit: m^2)
                double var_e = 0.0, var_n = 0.0, var_u = 0.0;

                if (has_gst_) {
                    // Case A: มีข้อมูล Error จริงจาก GPGST (แม่นยำสูง)
                    var_n = std::pow(gst_lat_err_, 2);
                    var_e = std::pow(gst_lon_err_, 2);
                    var_u = std::pow(gst_alt_err_, 2);
                    msg.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_DIAGONAL_KNOWN;
                } else {
                    // Case B: ใช้ค่า DOP * UERE (ประมาณการ)
                    // Variance = (DOP * UERE)^2
                    double hdop = p[8].empty() ? last_hdop_ : std::stod(p[8]);
                    
                    var_n = std::pow(hdop * GPS_UERE, 2); // North (Lat)
                    var_e = std::pow(hdop * GPS_UERE, 2); // East (Lon)
                    var_u = std::pow(last_vdop_ * GPS_UERE, 2); // Up (Alt)
                    msg.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_APPROXIMATED;
                }

                // Map ลง Covariance Matrix 9 ช่อง (Row-major)
                // [0]=EE, [4]=NN, [8]=UU
                msg.position_covariance[0] = var_e; 
                msg.position_covariance[4] = var_n; 
                msg.position_covariance[8] = var_u; 

                publisher_->publish(msg);

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

    // Members
    int sockfd_ = -1;
    double last_hdop_ = 5.0; 
    double last_vdop_ = 5.0;
    
    // GPGST values
    bool has_gst_ = false;
    double gst_lat_err_ = 5.0;
    double gst_lon_err_ = 5.0;
    double gst_alt_err_ = 10.0;

    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PeplinkGPSDriver>());
    rclcpp::shutdown();
    return 0;
}
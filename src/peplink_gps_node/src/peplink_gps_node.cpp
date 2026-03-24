#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <thread>
#include <atomic>

class PeplinkGPSDriver : public rclcpp::Node {
    // =========================================================
    //  USER CONFIGURATION
    // =========================================================
    const int MY_UDP_PORT = 8384;               // Port ที่ตั้งใน Peplink
    const std::string MY_FRAME_ID = "gps_link"; // TF Frame ID ของ GPS
    const double GPS_UERE = 3.0;                // ความคลาดเคลื่อนมาตรฐาน
    // =========================================================

public:
    PeplinkGPSDriver() : Node("peplink_gps_driver"), running_(true) {
        // Publishers
        pub_fix_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/fix", 10);
        
        // Setup UDP
        init_udp_socket();
        
        // Start UDP Receiver Thread (Data-driven, matches GPS Hz perfectly)
        udp_thread_ = std::thread(&PeplinkGPSDriver::receive_loop, this);
        
        RCLCPP_INFO(this->get_logger(), "🚀 Peplink GPS Driver (Data-driven Hz) Started!");
        RCLCPP_INFO(this->get_logger(), "Listening UDP Port: %d | Publishing to: /fix", MY_UDP_PORT);
    }

    ~PeplinkGPSDriver() {
        running_ = false;
        if (sockfd_ > 0) {
            close(sockfd_); // This will unblock recvfrom
        }
        if (udp_thread_.joinable()) {
            udp_thread_.join();
        }
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
        
        // Blocking mode is kept (no O_NONBLOCK), so the thread will sleep until data arrives.
    }

    void receive_loop() {
        char buffer[2048];
        struct sockaddr_in cliaddr; 
        socklen_t len = sizeof(cliaddr);
        
        while (running_ && rclcpp::ok()) {
            // recvfrom will block until data is received. This ensures 0% CPU usage while waiting,
            // and immediate execution (matching the GPS Hz) when data arrives.
            int n = recvfrom(sockfd_, buffer, 2048, 0, (struct sockaddr *)&cliaddr, &len);
            
            if (n < 0) {
                if (running_) RCLCPP_WARN(this->get_logger(), "UDP recvfrom error");
                break;
            }
            
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
                
                // Publish immediately upon parsing GPGGA (Data-driven Hz)
                pub_fix_->publish(msg_fix);

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

    int sockfd_ = -1;
    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr pub_fix_;
    std::thread udp_thread_;
    std::atomic<bool> running_;

    // DOP / GST
    double last_hdop_ = 5.0; 
    double last_vdop_ = 5.0;
    bool has_gst_ = false;
    double gst_lat_err_ = 5.0, gst_lon_err_ = 5.0, gst_alt_err_ = 10.0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PeplinkGPSDriver>());
    rclcpp::shutdown();
    return 0;
}
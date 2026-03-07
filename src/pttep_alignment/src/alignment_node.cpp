#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <pttep_alignment/srv/calculate_transformation.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <iomanip>

class AlignmentSolver : public rclcpp::Node {
    // =========================================================
    //  USER CONFIGURATION
    // =========================================================
    const std::string TOPIC_LIDAR = "/current_pose"; 
    const std::string TOPIC_GPS   = "/odom";         
    const std::string TOPIC_ALIGNED_GPS = "/aligned_odom"; // NEW
    const double MAX_TIME_DIFF    = 1.0;             
    // =========================================================

public:
    AlignmentSolver() : Node("alignment_solver") {
        // 1. Subscribe Lidar
        sub_lidar_ = create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
            TOPIC_LIDAR, 10, 
            [this](const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg) {
                last_lidar_msg_ = msg;
            });
            
        // 2. Subscribe GPS
        sub_gps_ = create_subscription<nav_msgs::msg::Odometry>(
            TOPIC_GPS, 10, 
            [this](const nav_msgs::msg::Odometry::SharedPtr msg) {
                last_gps_msg_ = msg;
                if (is_aligned_) {
                    publish_aligned_gps(msg);
                }
            });

        // 3. Publisher for Aligned GPS
        pub_aligned_gps_ = create_publisher<nav_msgs::msg::Odometry>(TOPIC_ALIGNED_GPS, 10);

        // 4. Service Save
        srv_save_ = create_service<std_srvs::srv::Trigger>(
            "/save_location", 
            std::bind(&AlignmentSolver::on_save_location, this, std::placeholders::_1, std::placeholders::_2));
            
        // 5. Service Calculate
        srv_calc_ = create_service<pttep_alignment::srv::CalculateTransformation>(
            "/calculate_transformation",
            std::bind(&AlignmentSolver::on_calculate, this, std::placeholders::_1, std::placeholders::_2));

        RCLCPP_INFO(this->get_logger(), "Alignment Solver Ready");
        RCLCPP_INFO(this->get_logger(), "Listening to GPS: %s | Lidar: %s", TOPIC_GPS.c_str(), TOPIC_LIDAR.c_str());
        RCLCPP_INFO(this->get_logger(), "Publishing Aligned Odom to: %s", TOPIC_ALIGNED_GPS.c_str());
    }

private:
    geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr last_lidar_msg_;
    nav_msgs::msg::Odometry::SharedPtr last_gps_msg_;
    
    std::vector<Eigen::Vector2d> pts_lidar_;
    std::vector<Eigen::Vector2d> pts_gps_;
    std::vector<double> weights_;

    rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr sub_lidar_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_gps_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_aligned_gps_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_save_;
    rclcpp::Service<pttep_alignment::srv::CalculateTransformation>::SharedPtr srv_calc_;

    // --- State for Real-time Alignment ---
    bool is_aligned_ = false;
    Eigen::Matrix2d R_align_;
    Eigen::Vector2d t_align_;

    // --- Core Logic: Publish Real-time Aligned GPS ---
    void publish_aligned_gps(const nav_msgs::msg::Odometry::SharedPtr msg) {
        if (!is_aligned_) return;

        // 1. Get current GPS XY (from /odom topic)
        Eigen::Vector2d curr_gps(msg->pose.pose.position.x, msg->pose.pose.position.y);

        // 2. Transform Position: p_aligned = R * p_gps + t
        Eigen::Vector2d aligned_pos = R_align_ * curr_gps + t_align_;

        // 3. Transform Covariance: Cov_aligned = R * Cov_gps * R^T
        Eigen::Matrix2d cov_gps;
        cov_gps << msg->pose.covariance[0], msg->pose.covariance[1],
                   msg->pose.covariance[6], msg->pose.covariance[7];
        
        Eigen::Matrix2d cov_aligned = R_align_ * cov_gps * R_align_.transpose();

        // 4. Transform Orientation (Yaw)
        // Extract original yaw from quaternion
        double siny_cosp = 2 * (msg->pose.pose.orientation.w * msg->pose.pose.orientation.z + msg->pose.pose.orientation.x * msg->pose.pose.orientation.y);
        double cosy_cosp = 1 - 2 * (msg->pose.pose.orientation.y * msg->pose.pose.orientation.y + msg->pose.pose.orientation.z * msg->pose.pose.orientation.z);
        double original_yaw = std::atan2(siny_cosp, cosy_cosp);

        // Add matrix rotation angle
        double rotation_angle = std::atan2(R_align_(1,0), R_align_(0,0));
        double new_yaw = original_yaw + rotation_angle;

        // 5. Construct new message
        auto aligned_msg = std::make_shared<nav_msgs::msg::Odometry>(*msg); // Copy original (header, twist, etc.)
        
        // Update Header Frame ID (Usually to match Lidar/Map frame)
        aligned_msg->header.frame_id = last_lidar_msg_ ? last_lidar_msg_->header.frame_id : "map";

        // Update Position
        aligned_msg->pose.pose.position.x = aligned_pos.x();
        aligned_msg->pose.pose.position.y = aligned_pos.y();
        aligned_msg->pose.pose.position.z = msg->pose.pose.position.z; // Keep Z same

        // Update Orientation (Quaternion from new yaw)
        aligned_msg->pose.pose.orientation.x = 0.0;
        aligned_msg->pose.pose.orientation.y = 0.0;
        aligned_msg->pose.pose.orientation.z = std::sin(new_yaw / 2.0);
        aligned_msg->pose.pose.orientation.w = std::cos(new_yaw / 2.0);

        // Update Covariance Elements (Keep others unchanged or set to 0 as needed)
        // Row-major 6x6 matrix: index = row * 6 + col
        aligned_msg->pose.covariance[0] = cov_aligned(0,0); // XX
        aligned_msg->pose.covariance[1] = cov_aligned(0,1); // XY
        aligned_msg->pose.covariance[6] = cov_aligned(1,0); // YX
        aligned_msg->pose.covariance[7] = cov_aligned(1,1); // YY

        pub_aligned_gps_->publish(*aligned_msg);
    }

    // --- Save Location Logic ---
    void on_save_location(const std_srvs::srv::Trigger::Request::SharedPtr, 
                          std_srvs::srv::Trigger::Response::SharedPtr res) {
        
        bool missing = false;
        if (!last_lidar_msg_) {
            RCLCPP_WARN(this->get_logger(), "❌ MISSING: Lidar Data (%s) is NULL!", TOPIC_LIDAR.c_str()); 
            missing = true;
        }
        if (!last_gps_msg_) {
            RCLCPP_WARN(this->get_logger(), "❌ MISSING: GPS Data (%s) is NULL!", TOPIC_GPS.c_str());
            missing = true;
        }

        if (missing) {
            res->success = false;
            res->message = "Error: Missing sensor data (Check terminal)";
            return;
        }

        // 2. เช็คเวลา (Time Sync)
        rclcpp::Time t_lidar(last_lidar_msg_->header.stamp);
        rclcpp::Time t_gps(last_gps_msg_->header.stamp);
        double time_diff = std::abs((t_lidar - t_gps).seconds());

        RCLCPP_INFO(this->get_logger(), "Sync -> Lidar: %.2f | GPS: %.2f | Diff: %.2f s", 
            t_lidar.seconds(), t_gps.seconds(), time_diff);

        if (time_diff > MAX_TIME_DIFF) { 
            res->success = false;
            res->message = "Sync Error! Diff: " + std::to_string(time_diff) + "s";
            
            if (t_lidar.seconds() > t_gps.seconds()) {
                RCLCPP_WARN(this->get_logger(), "Error: GPS lags behind!");
            } else {
                RCLCPP_WARN(this->get_logger(), "Error: Lidar lags behind!");
            }
            return;
        }

        // Weight & Save
        double w = 1.0 / (last_lidar_msg_->pose.covariance[0] + last_lidar_msg_->pose.covariance[7] + 1e-6); 
        pts_lidar_.push_back({last_lidar_msg_->pose.pose.position.x, last_lidar_msg_->pose.pose.position.y});
        pts_gps_.push_back({last_gps_msg_->pose.pose.position.x, last_gps_msg_->pose.pose.position.y});
        weights_.push_back(w);

        res->success = true;
        res->message = "Saved Point #" + std::to_string(pts_lidar_.size());
        RCLCPP_INFO(this->get_logger(), "Saved Point #%ld", pts_lidar_.size());
    }

    // --- Calculate Logic ---
    void on_calculate(const pttep_alignment::srv::CalculateTransformation::Request::SharedPtr req,
                      pttep_alignment::srv::CalculateTransformation::Response::SharedPtr res) {
        
        if (req->reset) {
            pts_lidar_.clear(); pts_gps_.clear(); weights_.clear();
            is_aligned_ = false;
            res->success = true; 
            res->message = "Memory Cleared and Alignment Stopped.";
            RCLCPP_INFO(this->get_logger(), "Memory Cleared.");
            return;
        }

        if (pts_lidar_.size() < 3) {
            res->success = false; 
            res->message = "Need > 3 points.";
            RCLCPP_WARN(this->get_logger(), "Error: Need at least 3 points (Have: %ld)", pts_lidar_.size());
            return;
        }

        auto [R, t] = compute_weighted_svd();
        
        // Save to state for real-time publishing
        R_align_ = R;
        t_align_ = t;
        is_aligned_ = true;
        
        res->transform.header.stamp = this->now();
        res->transform.header.frame_id = last_gps_msg_->header.frame_id;
        res->transform.child_frame_id = last_lidar_msg_->header.frame_id;
        
        res->transform.transform.translation.x = t.x();
        res->transform.transform.translation.y = t.y();
        res->transform.transform.translation.z = 0.0;
        
        double yaw = std::atan2(R(1,0), R(0,0));
        res->transform.transform.rotation.z = std::sin(yaw / 2.0);
        res->transform.transform.rotation.w = std::cos(yaw / 2.0);

        res->success = true;
        res->message = "Matrix Calculated & Real-time Alignment Started.";

        // print Matrix
        std::stringstream ss;
        ss << "\n--- CALCULATED MATRIX (For Human) ---\n";
        ss << std::fixed << std::setprecision(4);
        ss << "[ " << R(0,0) << ", " << R(0,1) << ", " << t.x() << " ]\n";
        ss << "[ " << R(1,0) << ", " << R(1,1) << ", " << t.y() << " ]\n";
        ss << "[ 0.0000, 0.0000, 1.0000 ]\n";
        RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
        RCLCPP_INFO(this->get_logger(), "Real-time alignment active on %s", TOPIC_ALIGNED_GPS.c_str());
    }

    std::pair<Eigen::Matrix2d, Eigen::Vector2d> compute_weighted_svd() {
        int n = pts_lidar_.size();
        double sum_w = 0;
        Eigen::Vector2d c_l(0,0), c_g(0,0);
        for(int i=0; i<n; i++) {
            c_l += weights_[i] * pts_lidar_[i];
            c_g += weights_[i] * pts_gps_[i];
            sum_w += weights_[i];
        }
        c_l /= sum_w; c_g /= sum_w;
        Eigen::Matrix2d H = Eigen::Matrix2d::Zero();
        for(int i=0; i<n; i++) H += weights_[i] * (pts_lidar_[i] - c_l) * (pts_gps_[i] - c_g).transpose();
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::Matrix2d R = svd.matrixV() * svd.matrixU().transpose();
        if (R.determinant() < 0) {
            Eigen::Matrix2d V = svd.matrixV(); V.col(1) *= -1;
            R = V * svd.matrixU().transpose();
        }
        return {R, c_l - R * c_g}; // NOTE: Target is Lidar(c_l), Source is GPS(c_g)
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AlignmentSolver>());
    rclcpp::shutdown();
    return 0;
}
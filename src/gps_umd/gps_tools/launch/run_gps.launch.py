from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    container = ComposableNodeContainer(
        name='gps_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container', # ใช้ตัวรันกลางของ ROS 2
        composable_node_descriptions=[
            ComposableNode(
                package='gps_tools',
                plugin='gps_tools::UtmOdometryComponent', # เรียกผ่าน Plugin Class
                name='utm_odometry_node',
                parameters=[{'append_zone': True}],
                remappings=[
                     ('fix', '/gps/fix'), # แก้ Topic ขาเข้าตรงนี้ (ซ้าย=ชื่อในโค้ด, ขวา=ชื่อจริง)
                     ('odom', '/odometry/utm') # แก้ Topic ขาออกตรงนี้
                ]
            ),
        ],
        output='screen',
    )

    return LaunchDescription([container])
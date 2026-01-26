import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # ---------------------------------------------------------
        # Node 1: Peplink GPS Driver (รับ UDP -> /fix)
        # ---------------------------------------------------------
        Node(
            package='peplink_gps_driver',
            executable='peplink_gps_node',
            name='peplink_driver',
            output='screen'
        ),

        # ---------------------------------------------------------
        # Node 2: UTM Converter (แปลง /fix -> /odom)
        # ---------------------------------------------------------
        Node(
            package='gps_tools',
            executable='utm_odometry_node',
            name='utm_converter',
            output='screen',
            parameters=[{
                'append_zone': True,       # สำคัญ! ต้องมี Zone
                'use_odometry_yaw': False,
                'rot_covariance': 99999.0
            }],
            remappings=[
                ('fix', '/fix'),
                ('odom', '/odom')  # ส่งออกไปชื่อ /odom รอไว้
            ]
        )
    ])
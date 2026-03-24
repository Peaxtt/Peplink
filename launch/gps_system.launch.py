from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # ---------------------------------------------------------
        # Node 1: Peplink GPS Driver (Raw GPS via UDP)
        # ---------------------------------------------------------
        Node(
            package='peplink_gps_driver',
            executable='peplink_gps_node',
            name='peplink_driver',
            output='screen'
        ),

        # ---------------------------------------------------------
        # Node 2: UTM Local Odom (Auto-Datum & XY Conversion)
        # ---------------------------------------------------------
        Node(
            package='utm_local_odom',
            executable='utm_local_odom_node',
            name='utm_converter',
            output='screen',
            remappings=[
                ('/fix', '/fix'),
                ('/peplink/odom', '/peplink/odom')
            ]
        ),

        # ---------------------------------------------------------
        # Node 3: PTTEP Alignment (SVD Calibration)
        # ---------------------------------------------------------
        Node(
            package='pttep_alignment',
            executable='alignment_node',
            name='alignment_node',
            output='screen'
        )
    ])

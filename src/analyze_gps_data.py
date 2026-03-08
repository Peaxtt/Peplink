#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
import utm
import os
import math
import argparse
from typing import List, Dict, Any, Optional

class GpsDataAnalyzer(Node):
    """
    ROS 2 Node for analyzing GPS log files. 
    It evaluates position covariance, selects the most accurate datum, 
    and computes local Cartesian coordinates (Local Odometry).
    """
    def __init__(self):
        super().__init__('gps_data_analyzer')

    def parse_gps_log(self, file_path: str) -> List[Dict[str, Any]]:
        """Parses the custom GPS log file and calculates error magnitude."""
        if not os.path.exists(file_path):
            self.get_logger().error(f"File not found: {file_path}")
            return []

        self.get_logger().info(f"Reading GPS log file: {file_path}")
        waypoints = []
        
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                
            docs = content.split('---')
            count = 1
            
            for doc in docs:
                if 'latitude' not in doc:
                    continue
                    
                lines = doc.strip().split('\n')
                lat, lon = None, None
                cov_start = False
                cov = []
                cov_type = None
                
                for line in lines:
                    line = line.strip()
                    if line.startswith('latitude:'):
                        lat = float(line.split(':')[1].strip())
                    elif line.startswith('longitude:'):
                        lon = float(line.split(':')[1].strip())
                    elif line.startswith('position_covariance:'):
                        cov_start = True
                    elif cov_start and line.startswith('-'):
                        cov.append(float(line.replace('-', '').strip()))
                    elif line.startswith('position_covariance_type:'):
                        cov_start = False
                        cov_type = int(line.split(':')[1].strip())
                        
                if lat is not None and lon is not None and len(cov) == 9:
                    # Calculate Root Mean Square Error from Covariance X and Y
                    error_magnitude = math.sqrt(cov[0] + cov[4])
                    waypoints.append({
                        'id': count,
                        'lat': lat,
                        'lon': lon,
                        'cov': cov,
                        'cov_type': cov_type,
                        'error_mag': error_magnitude
                    })
                    count += 1
                    
        except Exception as e:
            self.get_logger().error(f"Failed to parse file: {e}")
            
        return waypoints

    def find_optimal_datum(self, waypoints: List[Dict[str, Any]]) -> Optional[Dict[str, Any]]:
        """Finds the waypoint with the lowest error magnitude to serve as the datum."""
        if not waypoints:
            return None
        return min(waypoints, key=lambda x: x['error_mag'])

    def compute_and_log_local_odom(self, waypoints: List[Dict[str, Any]], datum: Dict[str, Any]) -> None:
        """Computes local XY coordinates relative to the datum and logs the results."""
        datum_easting, datum_northing, zone_num, zone_letter = utm.from_latlon(datum['lat'], datum['lon'])
        
        self.get_logger().info("=======================================================")
        self.get_logger().info("[DATUM SELECTED] Optimal origin found.")
        self.get_logger().info(f"Origin Point ID : {datum['id']}")
        self.get_logger().info(f"Lat/Lon         : {datum['lat']:.7f}, {datum['lon']:.7f}")
        self.get_logger().info(f"UTM Origin      : X={datum_easting:.2f}, Y={datum_northing:.2f} (Zone {zone_num}{zone_letter})")
        self.get_logger().info(f"Error Magnitude : {datum['error_mag']:.4f}")
        self.get_logger().info("=======================================================")
        self.get_logger().info("Local Odometry (XY) relative to Origin:")

        for pt in waypoints:
            e, n, _, _ = utm.from_latlon(pt['lat'], pt['lon'])
            local_x = e - datum_easting
            local_y = n - datum_northing
            
            cov_xx = pt['cov'][0]
            cov_yy = pt['cov'][4]
            
            log_msg = (
                f"ID: {pt['id']:02d} | "
                f"XY: [{local_x:7.2f}, {local_y:7.2f}] m | "
                f"Cov(XX,YY): [{cov_xx:5.2f}, {cov_yy:5.2f}] | "
                f"Type: {pt['cov_type']} | "
                f"Err: {pt['error_mag']:.2f}"
            )
            
            if pt['id'] == datum['id']:
                self.get_logger().info(f"  -> {log_msg}  [ORIGIN]")
            else:
                self.get_logger().info(f"     {log_msg}")

    def process_data(self, file_path: str) -> None:
        """Main execution flow for processing the GPS log."""
        waypoints = self.parse_gps_log(file_path)
        
        if not waypoints:
            self.get_logger().warning("No valid GPS data found in the provided file.")
            return
            
        self.get_logger().info(f"Successfully loaded {len(waypoints)} valid data points.")
        
        datum = self.find_optimal_datum(waypoints)
        if datum:
            self.compute_and_log_local_odom(waypoints, datum)
            self.get_logger().info("Processing complete. Local XY coordinates are ready to be compared with Lidar Odometry.")

def main(args=None):
    rclpy.init(args=args)
    
    # Use argparse for robust command-line argument handling
    parser = argparse.ArgumentParser(description="Analyze GPS logs and compute local Cartesian coordinates.")
    parser.add_argument(
        '--file', 
        type=str, 
        default='/home/peaxtt/PaYae/Fibo-work/peplink/src/data',
        help='Path to the GPS log file.'
    )
    
    # Parse arguments specifically for the node, ignoring default ROS 2 args
    parsed_args, ros_args = parser.parse_known_args()
    
    node = GpsDataAnalyzer()
    node.process_data(parsed_args.file)
    
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
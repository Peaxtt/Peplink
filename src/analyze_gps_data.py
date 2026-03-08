#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
import utm
import sys
import os
import math

class GPSDataAnalyzer(Node):
    def __init__(self):
        super().__init__('gps_data_analyzer')
        
    def process_file(self, file_path):
        if not os.path.exists(file_path):
            self.get_logger().error(f"❌ File not found: {file_path}")
            return

        self.get_logger().info(f"📂 กำลังอ่านไฟล์ GPS Log: {file_path}")
        
        waypoints = []
        
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                
            docs = content.split('---')
            count = 1
            
            # --- PHASE 1: กวาดข้อมูลทั้งหมดและคำนวณ Error ---
            for doc in docs:
                if 'latitude' in doc:
                    lines = doc.strip().split('\n')
                    lat, lon = None, None
                    cov_start = False
                    cov = []
                    cov_type = None
                    
                    for line in lines:
                        if line.startswith('latitude:'):
                            lat = float(line.split(':')[1].strip())
                        elif line.startswith('longitude:'):
                            lon = float(line.split(':')[1].strip())
                        elif line.startswith('position_covariance:'):
                            cov_start = True
                        elif cov_start and line.startswith('-'):
                            cov.append(float(line.replace('-','').strip()))
                        elif line.startswith('position_covariance_type:'):
                            cov_start = False
                            cov_type = int(line.split(':')[1].strip())
                            
                    if lat and lon and len(cov) == 9:
                        # คำนวณ Error รวบยอดจาก Covariance (X และ Y)
                        # variance_x = cov[0], variance_y = cov[4]
                        # เอามาหา RMS Error (Root Mean Square) เป็นตัวแทนความคลาดเคลื่อน
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

            if not waypoints:
                self.get_logger().warning("⚠️ ไม่พบข้อมูล GPS ที่สมบูรณ์ในไฟล์")
                return

            self.get_logger().info(f"✅ โหลดข้อมูลสำเร็จทั้งหมด {len(waypoints)} จุด")

            # --- PHASE 2: หาจุดที่ Error น้อยที่สุด เพื่อตั้งเป็น Origin ---
            # เรียงลำดับจาก error น้อยไปมาก
            best_point = min(waypoints, key=lambda x: x['error_mag'])
            
            # แปลงจุดที่ดีที่สุดเป็น UTM เพื่อใช้เป็น Datum (0,0)
            datum_easting, datum_northing, zone_num, zone_letter = utm.from_latlon(best_point['lat'], best_point['lon'])
            
            self.get_logger().info("\n=======================================================")
            self.get_logger().info(" 🎯 [DATUM SELECTED] เจอจุดกำเนิดที่แม่นยำที่สุดแล้ว!")
            self.get_logger().info(f"    ใช้จุดที่ #{best_point['id']} เป็น Origin (0,0)")
            self.get_logger().info(f"    Lat/Lon : {best_point['lat']:.7f}, {best_point['lon']:.7f}")
            self.get_logger().info(f"    UTM     : X={datum_easting:.2f}, Y={datum_northing:.2f} (Zone {zone_num}{zone_letter})")
            self.get_logger().info(f"    Error   : {best_point['error_mag']:.2f}")
            self.get_logger().info("=======================================================\n")

            # --- PHASE 3: คำนวณ XY ทุกจุดอ้างอิงกับ Origin และแสดงผล ---
            self.get_logger().info("📍 สรุปพิกัด Local Odom (XY) ของทุกจุดเทียบกับ Origin:")
            for pt in waypoints:
                # 1. แปลงเป็น UTM
                e, n, _, _ = utm.from_latlon(pt['lat'], pt['lon'])
                
                # 2. หักลบ Origin เพื่อให้เป็น Local XY (เมตร)
                local_x = e - datum_easting
                local_y = n - datum_northing
                
                # 3. จัดการแสดงผล Covariance (เอาแค่ X, Y โชว์ให้ดูง่าย)
                cov_xx = pt['cov'][0]
                cov_yy = pt['cov'][4]
                
                # พิมพ์ ROS Log
                log_msg = (
                    f"Point #{pt['id']:02d} | "
                    f"Local XY: [{local_x:7.2f}, {local_y:7.2f}] m | "
                    f"Cov (XX,YY): [{cov_xx:5.2f}, {cov_yy:5.2f}] | "
                    f"Type: {pt['cov_type']} | "
                    f"Error Mag: {pt['error_mag']:.2f}"
                )
                
                if pt['id'] == best_point['id']:
                    self.get_logger().info(f"⭐ {log_msg}  <-- [ORIGIN]")
                else:
                    self.get_logger().info(f"   {log_msg}")

            self.get_logger().info("\n🏁 ประมวลผลเสร็จสิ้น สามารถนำค่า Local XY ไปเทียบกับ Lidar Odom ได้เลย!")

        except Exception as e:
            self.get_logger().error(f"❌ Failed to parse file: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = GPSDataAnalyzer()
    
    target_file = sys.argv[1] if len(sys.argv) > 1 else '/home/peaxtt/PaYae/Fibo-work/peplink/src/data'
    
    node.process_file(target_file)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

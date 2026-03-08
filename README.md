# 🛰️ Peplink GPS to Odometry Alignment System

![ROS 2](https://img.shields.io/badge/ROS_2-Humble-blue?logo=ros)
![C++](https://img.shields.io/badge/C++-14-blue?logo=c%2B%2B)
![Python](https://img.shields.io/badge/Python-3.10-yellow?logo=python)

ระบบประมวลผลและแปลงพิกัดข้อมูลดาวเทียมความแม่นยำสูง (High-Precision GPS) สำหรับหุ่นยนต์อัตโนมัติ (AMR) ที่ปฏิบัติงานนอกอาคาร ระบบถูกออกแบบภายใต้สถาปัตยกรรม **Stand-alone** เพื่อลดความซับซ้อนในการตั้งค่า โดยสามารถรับข้อมูล NMEA จากอุปกรณ์ **Peplink BR2 Pro** ผ่านโปรโตคอล UDP และทำการแปลงพิกัดภูมิศาสตร์ (Lat/Lon) เป็นพิกัดคาร์ทีเซียน (Local XY) พร้อมประมวลผลการหมุนและทาบทับแกน (Alignment) ให้สอดคล้องกับแผนที่ Lidar ของหุ่นยนต์ได้อย่างสมบูรณ์

---

## 🌟 คุณสมบัติเด่น (Key Features)

1. **UDP NMEA Streamer & Covariance Extraction**
   อ่านข้อมูล GPS แบบ Real-time ผ่าน UDP Port `8384` พร้อมดึงค่าความคลาดเคลื่อน (Error Ellipse) เชิงลึกจากประโยค GPGST และ DOP
2. **Built-in UTM Transformation & Auto-Datum**
   แปลงพิกัด Lat/Lon เป็น UTM (Easting/Northing) ให้อัตโนมัติ พร้อมระบบ **Stable Auto-Datum** ที่จะคัดกรองเฉพาะสัญญาณ GPS ที่มีความเสถียร (เช่น Error < 4m หรือ HDOP < 2.0) เพื่อตั้งเป็นจุดกำเนิดพิกัด (Origin: 0,0) ทันทีเมื่อเปิดระบบ
3. **Automated Alignment (SVD)**
   ระบบคำนวณการจัดแนวมุมและตำแหน่ง (Rotation & Translation Matrix) โดยอิงจากอัลกอริทึม Weighted SVD (Singular Value Decomposition) เพื่อให้พิกัด GPS ทาบทับกับแผนที่ Lidar ได้อย่างแม่นยำที่สุด
4. **Real-time Covariance & Pose Rotation**
   เผยแพร่พิกัดแกน XY พร้อมค่า Covariance ที่ผ่านการหมุนปรับแก้ทิศทางแล้ว ผ่าน Topic `/aligned_odom` (10Hz) เพื่อให้ระบบ Navigation Stack (เช่น EKF) นำไปใช้งานได้ทันที
5. **Data Analysis & Validation Script**
   เครื่องมือ Python สำหรับวิเคราะห์ไฟล์ Log เพื่อประเมินความแม่นยำย้อนหลัง โดยระบบจะค้นหาจุด Origin ที่ดีที่สุดโดยอัตโนมัติ และแสดงค่าพิกัด Local XY สำหรับการตรวจสอบ

---

## 🛠️ การติดตั้ง (Installation)

### 1. ติดตั้ง Dependencies
เปิด Terminal และรันคำสั่งเพื่อติดตั้งเครื่องมือพื้นฐานและไลบรารีที่จำเป็น:
```bash
sudo apt update
sudo apt install -y build-essential git tmux libeigen-dev python3-pip
sudo apt install -y ros-humble-geometry-msgs ros-humble-nav-msgs ros-humble-std-srvs
pip install pyproj utm
```

### 2. โคลนและ Build โปรเจกต์
```bash
# โคลน Repository ไปยัง Workspace ของคุณ
cd ~
git clone https://github.com/Peaxtt/Peplink.git

# รันสคริปต์เพื่อ Build และเริ่มต้นระบบ (ระบบจะ Build อัตโนมัติในครั้งแรก)
cd Peplink
./start_peplink.sh
```

---

## 🚀 การเริ่มต้นระบบ (Usage)

เพื่อความสะดวกในการปฏิบัติงานหน้างาน ระบบถูกตั้งค่าให้รันผ่าน `tmux` โดยอัตโนมัติ:

```bash
cd ~/Peplink
./start_peplink.sh
```
*ระบบจะแบ่งหน้าต่าง Terminal ออกเป็น 2 ส่วน (ซ้าย: โหนดประมวลผล GPS, ขวา: โหนด Alignment)*  
*(หากต้องการยุติการทำงานทั้งหมด ให้ใช้คำสั่ง: `./start_peplink.sh stop`)*

> **⚠️ ข้อควรระวัง (Auto-Datum Initialization):**
> ทันทีที่ระบบเริ่มทำงาน โหนดจะรอจนกว่า **"คุณภาพของสัญญาณ GPS จะอยู่ในเกณฑ์ที่เสถียร"** ก่อนที่จะยึดตำแหน่งปัจจุบันเป็นพิกัดอ้างอิง (0,0)
> หากพบข้อความ `Waiting for STABLE GPS...` โปรดรอจนกว่าระบบจะแจ้งเตือนว่า `📍 INITIAL DATUM SET!`

---

## 🕹️ กระบวนการปรับแก้พิกัด (Alignment Process)

เมื่อระบบกำหนดจุด Datum (0,0) สำเร็จแล้ว หากต้องการดัดแกน X, Y ของ GPS ให้ขนานและทาบทับกับแผนที่ Lidar ให้เปิด **Terminal ใหม่** และทำตามขั้นตอนดังนี้:

### 1. บันทึกจุดอ้างอิง (Save Waypoints)
เคลื่อนที่หุ่นยนต์ไปยังตำแหน่งต่างๆ และหยุดนิ่ง **อย่างน้อย 3 จุด** (ระยะห่างระหว่างจุดที่มากขึ้น จะช่วยเพิ่มความแม่นยำ) 
ณ แต่ละจุด ให้รันคำสั่งเพื่อบันทึกพิกัด:
```bash
ros2 service call /save_location std_srvs/srv/Trigger
```

### 2. คำนวณสมการ (Calculate Matrix)
เมื่อบันทึกจุดครบถ้วนแล้ว ให้สั่งรันการคำนวณ SVD เพื่อเริ่มกระบวนการดัดพิกัดแบบ Real-time:
```bash
ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation "{reset: false}"
```
*(กรณีที่ต้องการล้างค่าที่บันทึกไว้เพื่อเริ่มต้นใหม่ ให้ปรับพารามิเตอร์เป็น `reset: true`)*

---

## 📊 การวิเคราะห์ข้อมูลย้อนหลัง (Data Analysis)

สำหรับทีม Data หรือผู้ที่ต้องการวิเคราะห์ค่าความคลาดเคลื่อนจากไฟล์ Log (YAML/TXT) สามารถใช้สคริปต์วิเคราะห์ได้ทันที:

```bash
python3 src/analyze_gps_data.py src/data
```
*สคริปต์จะทำการค้นหาจุดที่มี Covariance ต่ำที่สุดเพื่อตั้งเป็น Origin และรายงานพิกัด Local XY พร้อมค่า Error ของทุกจุดในรูปแบบที่นำไปพลอตเปรียบเทียบได้ทันที*

---

## ⚙️ โครงสร้างข้อมูล (Topic Interface)

ระบบเผยแพร่ข้อมูล (Topics) ที่พร้อมให้ทีม Navigation นำไปปรับใช้ตามความเหมาะสมของสถาปัตยกรรมหุ่นยนต์:

| ทิศทาง | ชื่อ Topic | ROS 2 Type | คำอธิบาย / การนำไปใช้งาน |
| :---: | :--- | :--- | :--- |
| 📤 **Output** | `/fix` | `sensor_msgs/NavSatFix` | **พิกัดระดับโลก (Lat/Lon):** ข้อมูลดิบพร้อมค่า Covariance เหมาะสำหรับการนำไปพลอตบน Google Maps หรือ Web UI |
| 📤 **Output** | `/peplink/odom` | `nav_msgs/Odometry` | **พิกัดระดับท้องถิ่น (Local XY):** ข้อมูลที่ถูกแปลงเป็น UTM และหักลบจุดกำเนิดเป็น (0,0) แล้ว สามารถพลอตเส้นทางใน Rviz2 ได้อิสระ |
| 📥 **Input** | `/current_pose` | `geometry_msgs/PoseWithCovarianceStamped` | **พิกัดเป้าหมาย:** ข้อมูลตำแหน่งจากระบบ Lidar/SLAM เพื่อใช้เป็นเป้าหมายอ้างอิงในการคำนวณ Alignment |
| 📤 **Output** | `/aligned_odom` | `nav_msgs/Odometry` | **(⭐ ข้อมูลหลัก)** พิกัด `/peplink/odom` ที่ผ่านกระบวนการหมุนด้วย SVD ให้ทาบทับกับแผนที่ Lidar แล้ว **พร้อมนำไปป้อนเข้า Robot Localization (EKF) ทันที** |

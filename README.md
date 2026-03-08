# 🛰️ Peplink GPS to Odometry Alignment System

![ROS 2 Humble](https://img.shields.io/badge/ROS_2-Humble-blue)
![C++](https://img.shields.io/badge/C++-14-blue)
![Python](https://img.shields.io/badge/Python-3-yellow)

โปรเจกต์นี้คือระบบประมวลผลและแปลงพิกัดข้อมูลดาวเทียมความแม่นยำสูง (High-Precision GPS) สำหรับหุ่นยนต์อัตโนมัติ (AMR) ที่ทำงานในสภาพแวดล้อมนอกอาคาร ระบบถูกออกแบบมาให้ **"ทำงานจบในตัว (Stand-alone)"** โดยอ่านข้อมูล NMEA 0183 ผ่านโปรโตคอล UDP จากอุปกรณ์ **Peplink BR2 Pro** แล้วแปลงพิกัดโลก (Lat/Lon) ให้กลายเป็นพิกัดบนพื้นราบ (Local Cartesian XY) พร้อมระบบดัดพิกัด (Alignment) เพื่อทาบทับกับแผนที่ Lidar ของหุ่นยนต์ได้อย่างสมบูรณ์ โดยไม่ต้องพึ่งพาระบบ TF ภายนอกในขั้นตอนการเริ่มต้น

## 🌟 ฟีเจอร์หลัก (Key Features)

1. **UDP NMEA Streamer & Covariance Extraction:** อ่านข้อมูล GPS สดจาก Peplink ผ่าน Port `8384` และดึงค่าความคลาดเคลื่อน (Error Ellipse) แบบเรียลไทม์จาก GPGST/DOP
2. **Built-in UTM Transformation & Auto-Datum:** แปลง Lat/Lon เป็น UTM (Easting/Northing) ให้อัตโนมัติ พร้อมระบบ **"Stable Auto-Datum"** ที่จะคัดกรองสัญญาณ GPS ที่นิ่งพอ (เช่น Error < 4m หรือ HDOP < 2.0) เพื่อยึดเป็นจุดกำเนิด (0,0) ทันทีตอนเปิดเครื่อง
3. **Automated Alignment (SVD):** สามารถเดินหุ่นยนต์เก็บจุด Waypoint แล้วกดคำนวณด้วยสมการ Weighted SVD เพื่อหา Matrix การหมุนและเลื่อน (Rotation & Translation) ที่แม่นยำที่สุด
4. **Real-time Covariance & Pose Rotation:** พ่นค่าพิกัด XY พร้อมค่า Covariance ที่ดัดให้ตรงกับทิศทางแผนที่หุ่นยนต์แล้วผ่าน Topic `/aligned_odom` (10Hz) เพื่อให้ระบบนำทาง (เช่น EKF) ใช้งานได้ทันที
5. **Data Analysis Script:** มาพร้อมสคริปต์ Python สำหรับรันวิเคราะห์ไฟล์ Log GPS ย้อนหลัง เพื่อหาจุด Origin ที่ดีที่สุดและแปลงค่า XY สำหรับการประเมินความแม่นยำ

---

## 🛠️ 1. การติดตั้ง (Installation)

### 1.1 ติดตั้ง Library ที่จำเป็น (Dependencies)
เปิด Terminal และรันคำสั่งเพื่อติดตั้งเครื่องมือพื้นฐาน:
```bash
sudo apt update
sudo apt install -y build-essential git tmux libeigen-dev python3-pip
sudo apt install -y ros-humble-geometry-msgs ros-humble-nav-msgs ros-humble-std-srvs
pip install pyproj utm
```

### 1.2 โคลนและ Build โปรเจค (Clone & Build)
```bash
# 1. โคลน Repository ไปไว้ในเครื่อง
cd ~
git clone https://github.com/Peaxtt/Peplink.git

# 2. เข้าไปในโฟลเดอร์ และสั่งรันระบบ (ครั้งแรกระบบจะ Build ให้อัตโนมัติ)
cd Peplink
./start_peplink.sh
```

---

## 🚀 2. การเริ่มต้นระบบ (How to Run)

เราออกแบบระบบให้เริ่มต้นทำงานได้ง่ายที่สุดผ่าน `tmux`:

### รันด้วยสคริปต์อัตโนมัติ (แนะนำ ⭐)
เปิด Terminal และใช้คำสั่งนี้คำสั่งเดียว:
```bash
cd ~/Peplink
./start_peplink.sh
```
*ระบบจะเปิดหน้าต่าง 2 ฝั่ง (ซ้าย: GPS, ขวา: Alignment Solver) หากต้องการปิด ให้รัน `./start_peplink.sh stop`*

> **⚠️ ข้อควรระวังตอนเริ่มต้น (Auto-Datum):**
> ทันทีที่รันสคริปต์ Node จะรอจนกว่า **"สัญญาณ GPS นิ่ง"** แล้วมันจะยึดจุดที่คุณยืนอยู่ตอนนั้นเป็นพิกัด (0,0) อัตโนมัติ หากเห็นข้อความเตือน `Waiting for STABLE GPS...` ให้รอสักพักจนกว่าจะขึ้นข้อความว่า `📍 INITIAL DATUM SET!`

---

## 🕹️ 3. การจูนพิกัดให้เข้ากับแผนที่ (Alignment Process)

เมื่อ Node ทำงานและปัก Datum (0,0) สำเร็จแล้ว หากแกน X, Y ของ GPS ยังไม่ตรงกับแผนที่ Lidar ของหุ่นยนต์ ให้เปิด **Terminal หน้าต่างใหม่** เพื่อส่งคำสั่ง:

### Step 1: เก็บจุดอ้างอิง (Save Waypoints)
ให้คุณบังคับหุ่นยนต์ไปในพื้นที่ต่างๆ แล้วจอดนิ่งๆ **อย่างน้อย 3 จุด** (ยิ่งไกลกัน ยิ่งแม่นยำ) 
เมื่อหุ่นจอดนิ่ง ให้รันคำสั่ง:
```bash
ros2 service call /save_location std_srvs/srv/Trigger
```

### Step 2: คำนวณ (Calculate Matrix)
เมื่อเก็บจุดครบแล้ว ให้สั่งระบบคำนวณและเริ่มการดัดพิกัด:
```bash
ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation "{reset: false}"
```
*(หากต้องการรีเซ็ตจุดที่เก็บไว้ให้ส่งค่า `reset: true`)*

---

## 📊 4. การวิเคราะห์ข้อมูลย้อนหลัง (Data Analysis)
หากต้องการทดสอบความแม่นยำจากไฟล์ Log ที่เก็บมา สามารถรันสคริปต์วิเคราะห์ได้เลย:
```bash
python3 src/analyze_gps_data.py src/data
```
สคริปต์จะหาจุดที่ดีที่สุดเป็น Origin ให้อัตโนมัติ และแสดงค่า Local XY พร้อม Covariance ของทุกจุด

---

## ⚙️ โครงสร้างของข้อมูล (Topic Interface)

ระบบนี้มี Topic ที่พร้อมให้ทีม Navigation นำไปใช้งานได้ตามความเหมาะสม:

| ประเภท | ชื่อ Topic | ROS 2 Type | หน้าที่ / วิธีนำไปใช้ |
| :--- | :--- | :--- | :--- |
| **Output** | `/fix` | `sensor_msgs/NavSatFix` | **พิกัดโลกดิบ (Lat/Lon):** พร้อมค่า Covariance สำหรับเอาไปพล็อตลง Google Maps |
| **Output** | `/peplink/odom` | `nav_msgs/Odometry` | **พิกัดท้องถิ่นเบื้องต้น (Local XY):** แปลงเป็น UTM และหักลบจุดเริ่มให้เป็น 0,0 แล้ว สามารถเอาไปพล็อตเส้นทางใน Rviz2 ดูได้เลยโดยไม่ต้องง้อ TF |
| **Input** | `/current_pose` | `geometry_msgs/PoseWithCovarianceStamped` | **แผนที่หุ่นยนต์:** ข้อมูลจาก Lidar/SLAM เพื่อใช้เป็นเป้าหมายอ้างอิงในการทาบทับพิกัด |
| **Output** | `/aligned_odom` | `nav_msgs/Odometry` | **(✨ ตัวจบงาน)** ข้อมูล `/peplink/odom` ที่ผ่านการหมุน (SVD Rotation) ให้ทาบทับกับแผนที่ Lidar แล้ว **ทีมหุ่นยนต์สามารถนำ Topic นี้ไปฟีดเข้า EKF (Robot Localization) ได้ทันที** |

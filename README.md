# 🛰️ Peplink GPS to Odometry Alignment System

![ROS 2 Humble](https://img.shields.io/badge/ROS_2-Humble-blue)
![C++](https://img.shields.io/badge/C++-14-blue)
![Python](https://img.shields.io/badge/Python-3-yellow)

โปรเจคนี้คือระบบประมวลผลและแปลงพิกัดข้อมูลดาวเทียมความแม่นยำสูง (High-Precision GPS) สำหรับหุ่นยนต์อัตโนมัติ (AMR) ที่ทำงานในสภาพแวดล้อมนอกอาคาร โดยระบบจะอ่านข้อมูล NMEA 0183 ผ่านโปรโตคอล UDP จากอุปกรณ์ **Peplink BR2 Pro** และทำการแปลงพิกัดภูมิศาสตร์ (Lat/Lon) ให้ทาบทับกับระบบพิกัดอ้างอิง (Odometry / Lidar) ของหุ่นยนต์โดยอัตโนมัติ

## 🌟 ฟีเจอร์หลัก (Key Features)

1. **UDP NMEA Streamer:** อ่านข้อมูล GPS สดๆ จากตัวกระจายสัญญาณ Peplink ผ่าน Port `8384` 
2. **Covariance Calculation:** คำนวณค่าความคลาดเคลื่อนทางตำแหน่ง (Error Ellipse) แบบเรียลไทม์โดยอ้างอิงจากค่า GPGST และ DOP
3. **Automated Transformation (SVD):** ผู้ใช้สามารถบังคับหุ่นยนต์ไปตามจุดต่างๆ และกดบันทึกพิกัด ระบบจะใช้ระเบียบวิธี SVD (Singular Value Decomposition) เพื่อหาความสัมพันธ์ของ Matrix $R$ (หมุน) และ $t$ (เลื่อน)
4. **Real-time Alignment Publisher:** ระบบสามารถหมุนแกนพิกัดและแกนของความคลาดเคลื่อน (Covariance Rotation) เพื่อ Publish พิกัดใหม่ที่พร้อมให้ Navigation Stack นำไปใช้งานได้ทันทีใน Topic `/aligned_odom` (10Hz)

---

## 🛠️ 1. การติดตั้ง (Installation)

### 1.1 ติดตั้ง Library ที่จำเป็น (Dependencies)
เปิด Terminal และรันคำสั่งเพื่อติดตั้งเครื่องมือพื้นฐาน:
```bash
sudo apt update
sudo apt install -y build-essential git tmux libeigen-dev
sudo apt install -y ros-humble-geometry-msgs ros-humble-nav-msgs ros-humble-std-srvs
```

### 1.2 โคลนและ Build โปรเจค (Clone & Build)
เนื่องจากโปรเจคนี้ประกอบด้วยหลาย ROS 2 Packages คุณสามารถรันสคริปต์อัตโนมัติที่เราเตรียมไว้ให้ได้เลย:

```bash
# 1. โคลน Repository ไปไว้ในเครื่อง (ตัวอย่างไว้ที่ Home)
cd ~
git clone https://github.com/Peaxtt/Peplink.git

# 2. เข้าไปในโฟลเดอร์ และสั่งรันระบบ (ครั้งแรกระบบจะ Build ให้อัตโนมัติ)
cd Peplink
./start_peplink.sh
```

---

## 🚀 2. การเริ่มต้นระบบ (How to Run)

เราออกแบบระบบให้เริ่มต้นทำงานได้ง่ายที่สุดผ่าน `tmux` โดยแบ่งการควบคุมออกเป็น 2 วิธี:

### วิธีที่ 1: รันด้วยสคริปต์อัตโนมัติ (แนะนำ ⭐)
เปิด Terminal และใช้คำสั่งนี้คำสั่งเดียว:
```bash
cd ~/Peplink
./start_peplink.sh
```
*ระบบจะเปิดหน้าต่างขึ้นมา 2 ฝั่ง (ซ้าย: GPS, ขวา: Alignment Solver) หากต้องการปิดการทำงานทุกอย่าง ให้รันคำสั่ง ` ./start_peplink.sh stop ` ใน Terminal หน้าต่างใหม่*

### วิธีที่ 2: รันแยกทีละหน้าต่าง (Manual Mode)
หากคุณไม่สะดวกใช้สคริปต์ ให้เปิด Terminal ใหม่ 2 หน้าต่าง:
**Terminal 1:** 
```bash
source /opt/ros/humble/setup.bash
source ~/Peplink/install/setup.bash
ros2 run peplink_gps_driver peplink_gps_node
```
**Terminal 2:**
```bash
source /opt/ros/humble/setup.bash
source ~/Peplink/install/setup.bash
ros2 run pttep_alignment alignment_node
```

---

## 🕹️ 3. การทำ Alignment (Calibration Process)

เมื่อโหนดทั้งสองทำงานแล้ว ให้เปิด **Terminal ที่ 3** (หน้าต่างว่างๆ) สำหรับส่งคำสั่งเพื่อตั้งค่าความแม่นยำให้กับระบบ

### Step 1: เก็บจุดอ้างอิง (Save Waypoints)
ให้คุณบังคับหุ่นยนต์ไปในพื้นที่ต่างๆ แล้วจอดนิ่งๆ **อย่างน้อย 3 จุด** (ยิ่งไกลกัน ยิ่งแม่นยำ) 
เมื่อหุ่นจอดนิ่ง ให้รันคำสั่งนี้เพื่อบันทึกจุด:
```bash
ros2 service call /save_location std_srvs/srv/Trigger
```
*(ถ้าขึ้นว่า success = true แสดงว่าบันทึกสำเร็จ)*

### Step 2: คำนวณและแปลงพิกัด (Calculate Matrix)
เมื่อเก็บจุดครบแล้ว ให้สั่งระบบคำนวณสมการเพื่อเริ่มต้นกระบวนการ Real-time Alignment:
```bash
ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation "{reset: false}"
```

### Step 3: ตรวจสอบข้อมูล (Verify Output)
ระบบจะเริ่มทำงานและส่งข้อมูลพิกัด GPS ที่แปลงให้เข้ากับ Lidar/Odom แล้วออกมาทาง Topic ชื่อ `/aligned_odom` คุณสามารถตรวจสอบได้โดยพิมพ์:
```bash
ros2 topic echo /aligned_odom
```
*ข้อมูลนี้พร้อมถูกนำไปป้อนให้แพ็กเกจนำทาง (Navigation/Localization) เพื่อใช้ขับเคลื่อนอัตโนมัติได้ทันที*

---

## ⚙️ โครงสร้างของข้อมูล (Topic Interface)

| ประเภทข้อมูล | ชื่อ Topic | ชนิดของ Message (ROS 2) |
| :--- | :--- | :--- |
| **Input Lidar Odom** | `/current_pose` | `geometry_msgs/PoseWithCovarianceStamped` |
| **Input GPS Odom** | `/odom` | `nav_msgs/Odometry` |
| **Output Aligned Odom** | `/aligned_odom` | `nav_msgs/Odometry` |

*(คุณสามารถเปลี่ยนชื่อ Topic เหล่านี้ได้ในไฟล์ `src/pttep_alignment/src/alignment_node.cpp`)*
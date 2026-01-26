# 🛰️ Peplink GPS Driver (UDP → ROS 2)

**Author:** Peaxtt
**Hardware Target:** Peplink MAX BR2 Pro (and compatible UDP NMEA senders)
**ROS Distribution:** ROS 2 Humble Hawksbill

แพ็กเกจนี้เป็น **ROS 2 Driver** สำหรับรับข้อมูล GPS ในรูปแบบ **NMEA ผ่าน UDP** จาก Router Peplink แล้วแปลงให้อยู่ในรูปแบบมาตรฐานของ ROS 2 คือ `sensor_msgs/NavSatFix` พร้อมคำนวณค่า **Position Covariance** อย่างถูกต้อง เพื่อใช้งานต่อในระบบ **Localization / Mapping / Navigation**

---

## ✨ Features

* 📡 รับข้อมูล GPS จาก Peplink ผ่าน **UDP**
* 🧭 แปลง NMEA → `sensor_msgs/NavSatFix`
* 📐 คำนวณ **Covariance (m²)** ตามมาตรฐาน ROS
* 🔄 รองรับทั้งโหมด **High Precision (GPGST)** และ **Standard (GPGSA)**
* ⚙️ ตั้งค่า Port, Frame ID, UERE ได้จากโค้ดโดยตรง

---

## 📋 Prerequisites

### 1. Hardware

* **Peplink Router** ที่รองรับ GPS Forwarding (เช่น MAX BR2 Pro)
* **Computer** ที่เชื่อมต่อกับ Router (LAN หรือ Wi‑Fi)

  * ต้องทราบ IP Address ของเครื่อง (เช่น `192.168.50.xxx`)

### 2. Software

* **OS:** Ubuntu 22.04 LTS
* **ROS 2:** Humble Hawksbill
* **Dependencies:**

```bash
sudo apt update
sudo apt install ros-humble-rclcpp ros-humble-sensor-msgs
```

---

## ⚙️ Router Configuration (ตั้งค่าที่ Peplink)

ก่อนรัน Node ต้องตั้งค่าให้ Peplink ส่งข้อมูล GPS มายังเครื่อง ROS 2

1. เข้า Web Admin ของ Peplink (ปกติคือ `192.168.50.1`)
2. ไปที่เมนู **Advanced → GPS Forwarding**
3. ตั้งค่าดังนี้

* **Service:** Enable
* **IP Address:** IP ของเครื่องที่รัน ROS 2 (เช่น `192.168.50.100`)
* **Port:** `8384` (ต้องตรงกับในโค้ด)
* **Protocol:** UDP

4. กด **Save** และ **Apply Changes**

---

## 🛠️ Installation & Build

### 1. ไปที่ ROS 2 Workspace

```bash
cd ~/ros2_ws
```

### 2. เตรียม Package

> ข้ามขั้นตอนนี้ หากมีโฟลเดอร์ `peplink_gps_driver` อยู่ใน `src` แล้ว

### 3. Build Package

```bash
colcon build --packages-select peplink_gps_driver --symlink-install
```

### 4. Source Environment

> ต้องทำทุกครั้งที่เปิด Terminal ใหม่

```bash
source install/setup.bash
```

---

## 📝 Configuration (การตั้งค่า)

สามารถแก้ไขค่าหลักได้โดยตรงที่ไฟล์:

```
src/peplink_gps_node/src/peplink_gps_node.cpp
```

ส่วนที่ต้องแก้ไขจะอยู่ด้านบนของไฟล์:

```cpp
// ================= USER CONFIGURATION =================
const int MY_UDP_PORT = 8384;               // UDP Port (ต้องตรงกับ Router)
const std::string MY_FRAME_ID = "gps_link"; // TF Frame ID ของ GPS
const double GPS_UERE = 3.0;                // UERE (เมตร) สำหรับ Peplink
// ======================================================
```

⚠️ ทุกครั้งที่แก้ไขไฟล์ `.cpp` ต้อง **build ใหม่** ด้วย `colcon build`

---

## 🚀 Running the Driver

รัน Node ด้วยคำสั่ง:

```bash
ros2 run peplink_gps_driver peplink_gps_node
```

หากทำงานถูกต้อง จะเห็น Log เช่น:

```
[INFO] [peplink_gps_driver]: Peplink GPS Driver Started
[INFO] [peplink_gps_driver]: Listening on UDP Port 8384 (frame_id: gps_link)
```

---

## ✅ Verification (ตรวจสอบการทำงาน)

### 1. ตรวจสอบข้อมูล GPS

```bash
ros2 topic echo /fix
```

ควรตรวจสอบว่า:

* `latitude`, `longitude` เป็นค่าพิกัดจริง
* `position_covariance` มีค่าไม่เป็นศูนย์

### 2. ตรวจสอบความถี่ข้อมูล

```bash
ros2 topic hz /fix
```

โดยทั่วไปจะอยู่ประมาณ **1 Hz** (ขึ้นกับการตั้งค่า Peplink)

---

## 🧠 Technical Details: Covariance Calculation

Driver นี้จะคำนวณ **Variance (m²)** เพื่อใส่ใน `position_covariance` ของ `NavSatFix`

### สูตรพื้นฐาน

```
Variance = (DOP × UERE)²
```

### โหมดที่รองรับ

#### 1. High Precision Mode (`GPGST`)

* ใช้เมื่อ Peplink ส่งประโยค `GPGST`
* ใช้ค่า **Latitude / Longitude Error** จริงจาก GNSS
* นำมายกกำลังสองเป็น Variance โดยตรง

#### 2. Standard Mode (`GPGSA`)

* ใช้เมื่อไม่มี `GPGST`
* ใช้ค่า `HDOP × UERE`
* ค่า UERE เริ่มต้น = **3.0 m** (เหมาะกับ BR2 Pro)

### Covariance Matrix (Row‑major 3×3)

```
[ Var_East,   0,          0 ]
[ 0,          Var_North,  0 ]
[ 0,          0,          Var_Up ]
```

---

## ❓ Troubleshooting

### ไม่มีข้อมูลออกมา

* ตรวจสอบ Firewall ว่าไม่บล็อก UDP Port
* ตรวจสอบ IP Address ใน Router ให้ตรงกับเครื่อง ROS 2

### Error: `Bind failed on port`

* มีโปรแกรมอื่นใช้ Port เดียวกันอยู่
* หรือรัน Node ซ้อนกันหลายตัว

แก้ไขโดย:

```bash
killall -9 peplink_gps_node
```

แล้วรันใหม่

### ค่า Covariance สูงผิดปกติ

* สัญญาณ GPS ไม่ดี (อยู่ในอาคาร / มีสิ่งกีดขวาง)
* ค่า HDOP สูง → Covariance สูง (เป็นพฤติกรรมปกติ)

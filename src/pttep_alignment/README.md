# 🧠 PTTEP Alignment Solver (Real-time GPS Alignment)

![ROS 2 Humble](https://img.shields.io/badge/ROS_2-Humble-blue)
![Algorithm](https://img.shields.io/badge/Algorithm-Weighted_SVD-orange)

Node นี้ใช้สำหรับคำนวณหา **Transformation Matrix (T)** ระหว่างพิกัด:
* **Local Frame:** Lidar / SLAM (`/current_pose`)
* **Global Frame:** GPS / Odometry (`/odom`)

โดยอาศัยอัลกอริทึม **Weighted Singular Value Decomposition (SVD)** เพื่อหาค่า **Rotation (R)** และ **Translation (t)** และฟีเจอร์สำคัญคือการ **Publish พิกัดที่ถูกแปลงแล้ว (Aligned Odometry) แบบ Real-time** พร้อมกับการหมุนแกนความคลาดเคลื่อน (Covariance Rotation) ให้ถูกต้อง

---

## 🚀 ฟีเจอร์ที่ได้รับการอัปเกรด (Production Ready ✅)

1. **Real-time Alignment Publisher:**
   หลังจากเรียก Service คำนวณหา Matrix เสร็จสิ้น ระบบจะเข้าสู่โหมด "Aligned" อัตโนมัติ โดยทุกครั้งที่ได้รับข้อมูลใหม่จาก GPS ระบบจะทำการคูณ Matrix เพื่อแปลงพิกัดให้ทาบทับกับกรอบของ Lidar ทันที และส่งออกไปที่ Topic `/aligned_odom`
2. **Covariance Rotation:**
   ระบบจะทำการดึงค่า Covariance 2x2 (North-East) จาก GPS มาหมุนตามสมการ $\Sigma_{new} = R \cdot \Sigma_{gps} \cdot R^T$ ทำให้ทิศทางของ Error Ellipse หมุนไปตรงกับทิศทางจริงของหุ่นยนต์ (Orientation/Yaw ก็ถูกหมุนด้วยเช่นกัน)

---

## 🛠️ 1. Setup & Build

### ติดตั้ง Library ที่จำเป็น และ Build Package

```bash
# Install dependencies
sudo apt update && sudo apt install -y \
  libeigen-dev \
  ros-humble-geometry-msgs \
  ros-humble-nav-msgs \
  ros-humble-std-srvs

# Build package
cd ~/peplink
colcon build --packages-select pttep_alignment
source install/setup.bash
```

---

## ⚙️ 2. Configuration (การตั้งค่า Topic)

หากต้องการเปลี่ยนชื่อ Topic สามารถแก้ไขได้โดยตรงที่ไฟล์:
`src/pttep_alignment/src/alignment_node.cpp`

```cpp
// ================= USER CONFIGURATION =================
const std::string TOPIC_LIDAR       = "/current_pose"; // Topic จาก SLAM / Lidar (PoseWithCovarianceStamped)
const std::string TOPIC_GPS         = "/odom";         // Topic จาก GPS / Peplink (Odometry)
const std::string TOPIC_ALIGNED_GPS = "/aligned_odom"; // Topic ขาออกหลังการแปลงพิกัด (Odometry)
const double MAX_TIME_DIFF          = 1.0;             // Time diff threshold
// ======================================================
```

> ⚠️ **หมายเหตุ:** หลังแก้ไขไฟล์ `.cpp` ต้องรัน `colcon build` ใหม่ทุกครั้ง

---

## 🎮 3. How to Use (วิธีใช้งาน)

การใช้งานระบบเพื่อทำ Alignment มี 3 ขั้นตอน ดังนี้:

### Step 1: รัน Node
```bash
ros2 run pttep_alignment alignment_node
```
*ระบบจะเปิดรอรับข้อมูล หากมีข้อมูลเข้า ระบบจะแจ้งเตือนว่า "Alignment Solver Ready"*

---

### Step 2: บันทึกจุดอ้างอิง (Save Points)
บังคับหุ่นยนต์ไปตามจุดต่างๆ เมื่อหุ่นยนต์ **หยุดนิ่ง** ให้เรียก Service นี้เพื่อบันทึกคู่พิกัด (ทำซ้ำอย่างน้อย **3 ครั้ง** ในพิกัดที่ต่างกัน):
```bash
ros2 service call /save_location std_srvs/srv/Trigger
```

---

### Step 3: คำนวณและเปิดระบบ Real-time (Calculate)
เมื่อเก็บจุดครบแล้ว ให้เรียก Service คำนวณ Matrix:
```bash
ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation "{reset: false}"
```
*เมื่อคำนวณสำเร็จ ระบบจะแจ้งว่า `Real-time alignment active on /aligned_odom` และเริ่มทำการ Publish ข้อมูล GPS ที่ถูกแปลงเข้าสู่เฟรม Odom ให้ทันทีที่ความถี่ของ GPS ปกติ!*

---

### Option: Reset ข้อมูล (เริ่มใหม่)
หากต้องการล้างค่าที่เคยบันทึกไว้และหยุดการทำงาน Real-time:
```bash
ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation "{reset: true}"
```

---

## ⚠️ Constraints (เงื่อนไขการทำงาน)
* **Data Synchronization:** ข้อมูลจาก `/odom` (GPS) และ `/current_pose` (Lidar) ต้องมี Timestamp ใกล้เคียงกันไม่เกิน `MAX_TIME_DIFF` ตอนที่กด Save
* **Minimum Points:** ต้องบันทึกอย่างน้อย **3 จุด** จึงจะหา Matrix ได้อย่างสมบูรณ์
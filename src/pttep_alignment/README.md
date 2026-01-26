# 🧠 PTTEP Alignment Solver

![ROS 2 Humble](https://img.shields.io/badge/ROS_2-Humble-blue)
![Algorithm](https://img.shields.io/badge/Algorithm-Weighted_SVD-orange)

Node นี้ใช้สำหรับคำนวณหา **Transformation Matrix (T)** ระหว่างพิกัด

* **Local Frame:** Lidar / SLAM
* **Global Frame:** GPS / Odometry

โดยอาศัยอัลกอริทึม **Weighted Singular Value Decomposition (SVD)** เพื่อให้ได้ค่า
**Rotation (R)** และ **Translation (t)** ที่เหมาะสมที่สุดจากชุดจุดอ้างอิง

---

## 🛠️ 1. Setup & Build

### ติดตั้ง Library ที่จำเป็น และ Build Package

```bash
# Install dependencies
sudo apt update && sudo apt install -y \
  libeigen-dev \
  ros-humble-geometry-msgs \
  ros-humble-std-srvs

# Build package
cd ~/ros2_ws
colcon build --packages-select pttep_alignment
source install/setup.bash
```

---

## ⚙️ 2. Configuration (การตั้งค่า)

หากต้องการเปลี่ยนชื่อ Topic หรือค่าความคลาดเคลื่อนของเวลา (Time Sync)
สามารถแก้ไขได้โดยตรงที่ไฟล์:

```
src/pttep_alignment/src/alignment_node.cpp
```

ส่วนที่ตั้งค่าได้จะอยู่ด้านบนของไฟล์:

```cpp
// ================= USER CONFIGURATION =================
const std::string TOPIC_LIDAR = "/current_pose"; // Topic จาก SLAM / Lidar
const std::string TOPIC_GPS   = "/odom";         // Topic จาก GPS / Odometry
const double MAX_TIME_DIFF    = 1.0;              // time diffs
// ======================================================
```

> ⚠️ **หมายเหตุ:** หลังแก้ไขไฟล์ `.cpp` ต้องรัน `colcon build` ใหม่ทุกครั้ง

---

## 🎮 3. How to Use (วิธีใช้งาน)

### Step 1: รัน Node

```bash
ros2 run pttep_alignment alignment_node
```

---

### Step 2: บันทึกจุดอ้างอิง (Save Points)

เมื่อหุ่นยนต์ **หยุดนิ่ง** ให้เรียก Service นี้เพื่อบันทึกคู่พิกัด
(Lidar + GPS)

```bash
ros2 service call /save_location std_srvs/srv/Trigger
```

* ทำซ้ำอย่างน้อย **3 ครั้ง**
* แต่ละครั้งควรอยู่ในตำแหน่งที่แตกต่างกัน

---

### Step 3: คำนวณ Transformation (Calculate)

เมื่อมีจุดครบแล้ว สั่งคำนวณหา Transformation Matrix (T)

```bash
ros2 service call /calculate_transformation \
  pttep_alignment/srv/CalculateTransformation "{reset: false}"
```

---

### Option: Reset ข้อมูล (เริ่มใหม่)

ลบจุดอ้างอิงทั้งหมดที่บันทึกไว้

```bash
ros2 service call /calculate_transformation \
  pttep_alignment/srv/CalculateTransformation "{reset: true}"
```

---

## ⚠️ Constraints (เงื่อนไขการทำงาน)

* **Data Synchronization**
  ข้อมูลจาก GPS และ Lidar ต้องมาถึงภายในช่วงเวลาไม่เกิน `MAX_TIME_DIFF`

* **Minimum Points**
  ต้องมีจุดอ้างอิงอย่างน้อย **3 จุด** จึงจะสามารถคำนวณ Transformation ได้

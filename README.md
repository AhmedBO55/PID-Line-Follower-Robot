# PID Line Follower Robot
This project is a fully custom **PID-based line follower robot** built using an **ESP32**, a **BFD-1000 line sensor**, and **N20 motors** driven by a **TB6612FNG motor driver**.  
The goal was to create a fast, stable, and precise robot capable of following a line smoothly using a tuned PID algorithm.

---

## 🚀 Project Overview
The robot reads the line using a 6-channel **BFD-1000 IR sensor**.  
The ESP32 processes the sensor data and applies a **PID control algorithm** to adjust motor speeds and keep the robot centered on the track.

To make tuning easier, the project includes a **Wi-Fi configuration interface** where PID values can be adjusted live from a local webpage — no need to re-upload the code.

---

## 🔧 Main Features
- **ESP32 microcontroller** for fast processing and Wi-Fi support  
- **PID control** for smooth and accurate line following  
- **Real-time PID tuning** via a web page hosted by the robot  
- **Custom soldered circuit board**  
- **BFD-1000 line sensor** for reliable and stable detection  
- **TB6612FNG motor driver** for precise PWM control  
- **3D-printed chassis** adapted for the BFD-1000  
- **Bluetooth test mode** for quick motor debugging  

---

## 🛠️ How It Works
1. The BFD-1000 sensor reads the position of the line.  
2. The ESP32 computes an error value from the sensor readings.  
3. A PID algorithm generates the correction output.  
4. The TB6612FNG driver adjusts each motor’s speed accordingly.  
5. The robot’s Wi-Fi interface allows live tuning of **Kp**, **Ki**, and **Kd** values.

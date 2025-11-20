# PID Line Follower Robot
##  Project Overview
PID Line Follower Robot is a project with the objective to understand the line follower concept and PID system. It’s an affordable version of a competitive robot, and v0; the next version will have better components and a PCB design.

![91e5619f-f241-466d-bc85-fe74e6d3bd5d](https://github.com/user-attachments/assets/82b212c1-7230-4afb-88ce-0324d2bd9a1d)
---
## Why ?
I did this project because I am very interested in robotics competitions, and I want to create my own line follower robot and compete with it in future competitions. This is why I started this project: to understand the fundamentals before buying fancy components. I decided to make my mistakes now, and I am willing to learn and create a PCB in the future for this project.

<img width="1902" height="869" alt="{8EE49D31-0FA8-4DDD-9549-9236D85A650A}" src="https://github.com/user-attachments/assets/d14ca05c-23ca-48f0-ad2a-0e62ecf5e370" />

![68747470733a2f2f626c75657072696e742e6861636b636c75622e636f6d2f757365722d6174746163686d656e74732f626c6f62732f70726f78792f65794a66636d467062484d694f6e73695a474630595349364e7a49774e79776963485679496a6f69596d](https://github.com/user-attachments/assets/d7a6d274-70f1-4fb0-a42b-07101b2581d3)
---
## Circuit diagram

<img width="1640" height="924" alt="Circuit diagram" src="https://github.com/user-attachments/assets/651e5d64-bb64-4bee-9149-7360b508ca1b" />

--- 

## Features
- **PID control** for smooth and accurate line following 
- **Real-time PID tuning** via a web page hosted by the robot
- **3D-printed chassis** adapted for the BFD-1000  
- **Bluetooth test mode** for quick motor debugging
  
<img width="1920" height="872" alt="{E63CA688-48E1-4CCD-8186-648663711028}" src="https://github.com/user-attachments/assets/d2342cbd-b9da-4101-8fbe-d95367e4c777" />

---
##  How It Works
1-The BFD-1000 reads the position of the line. 

2-The ESP32 computes an error value from the sensor readings. 

3-A PID algorithm generates the correction output.  

4-The TB6612FNG driver adjusts each motor’s speed. 

5-The robot’s Wi-Fi interface allows live tuning of Kp, Ki, and Kd values. 

---
## BOM (Bill of Materials)
I've created a BOM for the components used in this project. I didn’t include the cost and source links of the vendors, as I live in Tunisia and prices are not like the rest of the world, and our shops are not international.
But I've included the description and notes to explain the use of each component. If you want to change any of them, feel free to do so.





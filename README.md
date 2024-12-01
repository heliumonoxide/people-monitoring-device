# Smart IoT Monitoring and AI System

![Banner Github People Monitoring](https://github.com/user-attachments/assets/1b55aad2-3db4-4e4d-94a2-c7644a923621)

This project is a complete IoT-based monitoring and AI solution designed to collect, process, and analyze data from ESP32 sensors and cameras, store it in Firebase, and deploy trained AI models for industrial automation. The entire system is architected with modern technologies, ensuring scalability, ease of use, and high performance.

---

## Table of Contents

- [Features](#features)
- [Project Directory Structure](#project-directory-structure)
- [Architecture Overview](#architecture-overview)
- [Technologies Used](#technologies-used)
- [System Flow](#system-flow)
  - [1. User Level](#1-user-level)
  - [2. Software Level](#2-software-level)
  - [3. Database Level](#3-database-level)
  - [4. Hardware Level](#4-hardware-level)
  - [5. AI Level](#5-ai-level)
- [Setup and Deployment](#setup-and-deployment)
  - [Frontend](#frontend)
  - [Backend](#backend)
  - [Database Configuration](#database-configuration)
  - [Google Cloud VM](#google-cloud-vm)
  - [AI Model Deployment](#ai-model-deployment)
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

---

## **Project Directory Structure**

```plaintext
people-monitoring-device/
├── .git/                    # Git repository for version control
├── client/                  # Frontend application built with React Vite and Tailwind CSS
├── esp32_cam/               # ESP32 CAM scripts for capturing and transferring images
├── esp32_tof/               # ESP32 ToF scripts for distance measurement and data transfer
├── test_takepic/            # Test scripts for camera functionality and debugging usinng serial
├── vm_config_notes/         # Configuration notes for the Google Cloud VM instance
├── webapi/                  # Backend API built with Express.js for handling data and Firebase integration
├── yolov5/                  # Python scripts for handling YOLOv5 model inference and Firebase integration
├── FlowDiagram.drawio       # Editable flow diagram of the system
├── FlowDiagram.drawio.png   # PNG version of the system flow diagram
├── LICENSE                  # License file for the project
└── README.md                # Project documentation (this file)
```

---

### Folder Descriptions

1. **`.git/`**  
   - Contains the Git metadata for version control.

2. **`client/`**  
   - Houses the source code for the frontend application.  
   - Built with **React Vite** and styled using **Tailwind CSS**.

3. **`esp32_cam/`**  
   - Scripts for handling the **ESP32 CAM module**, including capturing images and sending them via FTP.

4. **`esp32_tof/`**  
   - Scripts for managing the **ESP32 ToF (Time-of-Flight) sensor**, focusing on distance measurement and data upload to firebase.

5. **`test_takepic/`**  
   - Contains testing utilities for capturing pictures with the ESP32 CAM to ensure proper functionality using Serial before developing.

6. **`vm_config_notes/`**  
   - Notes and scripts for configuring the **Google Cloud VM**, including FTP, Docker, and other services.

7. **`webapi/`**  
   - Backend code for the **Express.js API**, responsible for fetching data to Firebase.

8. **`yolov5/`**  
   - Python scripts for deploying the **YOLOv5 AI model** inference within Docker containers and integrated firebase system for sending data.

9. **`FlowDiagram.drawio`**  
   - The editable **flow diagram** of the system, created in **Draw.io**.

10. **`FlowDiagram.drawio.png`**  
    - Exported PNG image of the system flow diagram for documentation purposes.

11. **`LICENSE`**  
    - Specifies the licensing terms for using and distributing this project.

12. **`README.md`**  
    - This file serves as the main documentation for the project.

---

## Features

- **Real-time data collection**: Collects data from ESP32 ToF and ESP32 CAM sensors.
- **Efficient storage and processing**: Stores data in Firestore and Firebase Storage.
- **Seamless AI integration**: Processes data using YOLOv5 models deployed via Docker.
- **Scalable architecture**: Modular design deployed across user, software, and AI levels.
- **Accessible front-end**: Responsive interface built with React Vite and Tailwind CSS, deployed on Vercel.

---

## Architecture Overview

The project is divided into five distinct layers:
1. **User Level**: Interaction through a React-based web app.
2. **Software Level**: Backend services using Express.js with Firebase Admin SDK.
3. **Database Level**: Firebase for data storage and triggering workflows.
4. **Hardware Level**: Sensors for data collection and file transfers via FTP.
5. **AI Level**: Dockerized training, testing, and deployment of AI models.

Refer to the system flow diagram for a visual overview of the process.

---

## Technologies Used

- **Frontend**: React Vite, Tailwind CSS
- **Backend**: Express.js, Firebase Admin SDK
- **Database**: Firestore Database, Firebase Storage
- **Cloud Infrastructure**: Google Cloud Compute Engine (VM Instance)
- **Hardware**: ESP32 ToF, ESP32 CAM, 3D handle, and 3D case.
- **AI Frameworks**: YOLOv5, Docker
- **Deployment**: Vercel, SCP, and FTP protocols

---

## System Flow

### **1. User Level**
- Users interact with the system through a **React Vite + Tailwind CSS** web application.
- Deployed on **Vercel**, ensuring high availability and scalability.

### **2. Software Level**
- The backend is a serverless function developed using **Express.js** and deployed on **Vercel Serverless Functions**.
- It fetches and processes data from the Firebase database, responding to the frontend requests.

### **3. Database Level**
- **Firestore Database** and **Firebase Storage** store and manage data received from the hardware level.
- Automated workflows triggered by a **watchdog** script running on the Google Cloud VM send processed results back to the database.

### **4. Hardware Level**
- **ESP32 ToF** and **ESP32 CAM**:
  - Collect data and transfer files via **HTTP POST (Firebase SDK)** and **FTP**.
  - Data is sent to Google Cloud VM for further processing.
- FTP transfers are managed securely with Google Cloud VM's SCP configuration.
- For handle camera, I use this link: https://www.printables.com/model/3407-articulating-raspberry-pi-camera-mount-for-prusa-m
- For case, I use this link: https://www.printables.com/model/919846-esp32-s3-wroom-1-case-ali-express-style-not-freeno

### **5. AI Level**
- Data collected is used for training a **YOLOv5 model** within a Docker container.
- Trained models are deployed on the VM instance, monitoring results uploaded back to Firebase via **Watchdog**.

---

## Setup and Deployment

### **Frontend**
1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo.git
   cd your-repo/frontend
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Run locally:
   ```bash
   npm run dev
   ```
4. Deploy on Vercel:
   - Connect your GitHub repository.
   - Deploy the `frontend` directory.

### **Backend**
1. Navigate to the backend directory:
   ```bash
   cd ../backend
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Deploy on Vercel Serverless Functions.

### **Database Configuration**
- Configure Firebase Admin SDK with your Firebase project.
- Set up Firestore Database and Storage buckets for storing sensor data and AI results.

### **Google Cloud VM**
1. Set up a **Debian 11** VM instance.
2. Install necessary packages:
   ```bash
   sudo apt update && sudo apt install docker.io vsftpd
   ```
3. Configure FTP and SCP for file transfers.
4. Deploy your Docker container with YOLOv5 using:
   ```bash
   docker run -v /path/to/data:/app/data your-docker-image
   ```

### **AI Model Deployment**
- Train YOLOv5 models locally and deploy the trained model on Google Cloud VM.
- Use **Watchdog** to monitor directories and process new inputs.

---

## Usage

1. **Real-time Monitoring**:
   - Access the React web app to view collected data and AI-generated results.
2. **Data Uploads**:
   - Sensor data automatically gets uploaded to Firebase and processed.
3. **AI Results**:
   - View predictions or results processed by the YOLOv5 model on the frontend.

---

## Credits
I would like to thank the following people and resources for their invaluable help and inspiration in the completion of this project:

- OpenAI's ChatGPT: For providing detailed guidance and troubleshooting during development.
- NORVI product team: For providing configuration of ESP32 S3 with OV5640 https://www.esp32.com/viewtopic.php?f=17&t=40731 information: https://github.com/IndustrialArduino/NORVI-ESP32-CAMERA
- Leonardo Bispo Team: big thanks to Leonardo Bispo in https://github.com/ldab for developing ESP32_FTPClient.
- Joshua Gordon: For making the 3D case model for community. source: https://www.printables.com/@JoshuaGordon_2186925
- Sneaks: for making the 3D model of handle for community. source: https://www.printables.com/@Sneaks

Without the contributions and knowledge shared by these resources and individuals, this project would not have been possible.

---

## License

This project is licensed under the MIT License. See `LICENSE` for more details.

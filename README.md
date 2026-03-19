# BLOOD BANK MANAGEMENT SYSTEM: Real-World Platform

This project has been evolved into a **Professional Web Application** using **Pure C** for the backend engine and **Modern HTML/CSS** for the dashboard, as specified in the project methodology (Slides 11-16).

## 🏆 Real-World Web Version (The Star Project)
Unlike most websites that use Python or Node.js, this system uses a **Custom High-Performance C Web Server** built with `WinSock2`. This allows it to run as a single, lightweight executable on any Windows machine.

### 🛠️ Technical Stack
- **Backend**: Pure C (`web_backend.c`) using Windows Sockets.
- **Frontend**: Modern SPA Dashboard (`index.html`) with Glassmorphism and CSS Animations.
- **Database**: High-speed Binary Persistence (`donors.dat`).
- **Standard Implementation**: Contribution to **SDG 3** (Good Health & Well-Being).

### 🚀 How to Launch the Web System
1. **Compile**:
   ```bash
   gcc web_backend.c -o web_server.exe -lws2_32
   ```
2. **Run**:
   ```bash
   ./web_server.exe
   ```
3. **Access**: Open your browser and go to: `http://localhost:8080`

---

## 💻 Console Engine Version
The original C console version is still available as the core system logic for learning and data verification.

### Features
- **Module 1**: Donor Registration with Age Validation.
- **Module 2**: Automated Stock Inventory Management.
- **Module 3**: Rapid Blood Group Search.
- **Module 4**: File Loading/Saving Logic.

### 🚀 How to Run Console Version
1. **Compile**: `gcc blood_bank.c -o blood_bank.exe`
2. **Run**: `./blood_bank.exe`

## 📂 Project Structure
- `web/index.html`: The stunning modern dashboard UI.
- `web/assets/hero.png`: Professional medical hero image.
- `web_backend.c`: The HTTP/JSON API server written in C.
- `blood_bank.c`: The original modular console application.
- `donors.dat`: Shared binary storage for both systems.

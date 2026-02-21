# 🖥 Linux System Monitor (C++)

A lightweight Linux system monitoring tool written in **C++17** with two implementations:

1. 📁 JSON Logging Version – Stores system metrics in JSON format  
2. 📊 Prometheus + Grafana Version – Real-time monitoring with visualization  

Tested on **Red Hat Enterprise Linux 9** using Docker / Podman.

---

# 📂 Project Structure

```
linux-monitor/
│
├── json-version/
├── prometheus-version/
├── docker-compose.yml
├── prometheus.yml
├── screenshots/
│   ├── Picture1.png
│   ├── Picture2.png
│   └── Picture3.png
└── README.md
```

---

# 🔹 Version 1 — JSON Logging Monitor

## 🎯 Overview

This version collects system metrics from the Linux `/proc` filesystem and stores them inside a JSON file.

Designed for:
- Offline analysis
- Log storage
- Lightweight monitoring

---

# 🔹 Version 2 — Prometheus + Grafana (Real-Time Monitoring)

## 🎯 Overview

This version exposes system metrics through an HTTP endpoint compatible with Prometheus.

Prometheus scrapes the metrics and Grafana visualizes them in real time.

---

# 🏗 Architecture

```
┌───────────────┐     scrape     ┌───────────────┐
│ C++ Monitor   │──────────────>│ Prometheus     │
│ (Port 9100)   │               │ (Port 9090)    │
└───────────────┘               └─────┬─────────┘
                                        │
                                        ▼
                                   ┌─────────────┐
                                   │ Grafana     │
                                   │ (Port 3000) │
                                   └─────────────┘
```

---

# 📊 Screenshots

## 🔵 Grafana Dashboard – CPU & Memory Monitoring

<img src="screenshots/Picture1.png" alt="Grafana Dashboard 1" width="800">

---

## 🔵 Grafana Dashboard – Detailed Metrics View

<img src="screenshots/Picture2.png" alt="Grafana Dashboard 2" width="800">

---

## 🔵 Prometheus Metrics Endpoint

<img src="screenshots/Picture3.png" alt="Prometheus UI" width="800">

---

# 🐳 Run Full Stack (Docker Compose)

```bash
docker-compose up -d
```

Access:

- Monitor: http://localhost:9100/metrics
- Prometheus: http://localhost:9090
- Grafana: http://localhost:3000

---

# 🛠 Technologies Used

- C++17
- Linux `/proc` filesystem
- Docker / Podman
- Prometheus
- Grafana

---

# 👤 Author

Mohammed Ahmed Ali

---

# 📜 License

This project is for educational and demonstration purposes.

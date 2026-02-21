============================================
# 🖥 Linux System Monitor (C++)
============================================

A lightweight Linux system monitoring tool written in **C++17** providing two distinct implementations:

1. 📁 **JSON Logging Version** – captures metrics to a JSON file for offline analysis.
2. 📊 **Prometheus + Grafana Version** – exposes metrics for real‑time scraping and visualization.

> **Tested on:** Red Hat Enterprise Linux 9 (works with Docker or Podman)

============================================
# 📂 Project Structure
============================================

```
linux-monitor/
│
├── json-version/            # JSON logger implementation
├── prometheus-version/      # Prometheus exporter implementation
├── docker-compose.yml       # Full stack orchestration
├── prometheus.yml           # Prometheus configuration
├── screenshots/             # Example dashboard images
└── README.md                # ← you are here
```

============================================
# 🔹 Version 1 — JSON Logging Monitor
============================================

## 🎯 Overview

This variant reads from the Linux `/proc` filesystem and writes periodic system metrics to a JSON file.

**Use cases:**
- Offline log analysis
- Archival monitoring
- Minimal runtime dependencies

### 🐳 Build & Run

```bash
cd json-version
# build the container
podman build -t linux-monitor-json .  # (or docker build)

# run the monitor
podman run --rm linux-monitor-json
```

> **Note:** logs are stored inside the container unless you mount a host volume.

============================================
# 🔹 Version 2 — Prometheus + Grafana (Real‑Time Monitoring)
============================================

## 🎯 Overview

This implementation runs an HTTP exporter on port `9100`. Prometheus scrapes it, and Grafana renders dashboards.

### 🏗 Architecture

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

### 🐳 Build & Run the Exporter

```bash
cd prometheus-version
podman build -t linux-monitor-prometheus .
podman run -p 9100:9100 linux-monitor-prometheus
```

Navigate to `http://localhost:9100/metrics` to verify output:
```
cpu_usage_total 23.4
memory_usage 61.2
```

### 🔹 Prometheus Setup

1. Download Prometheus: https://prometheus.io/download/
2. Edit `prometheus.yml`:

```yaml
global:
  scrape_interval: 2s

scrape_configs:
  - job_name: "linux_monitor"
    static_configs:
      - targets: ["localhost:9100"]
```

3. Start Prometheus:
```bash
./prometheus --config.file=prometheus.yml
```
4. Browse the UI at **http://localhost:9090**

### 🔹 Grafana Setup

1. Download Grafana: https://grafana.com/grafana/download
2. Launch and open **http://localhost:3000** (default `admin/admin`)
3. Add a Prometheus data source:
   - URL: `http://localhost:9090`
   - Click **Save & Test**
4. Create dashboard panels using metrics `cpu_usage_total` and `memory_usage`.
   - For CPU gauges set the **Max** value to `100`.

### 🔥 Generating Load for Testing

```bash
# Single core
yes > /dev/null

# All cores
for i in $(seq 1 $(nproc)); do yes > /dev/null & done

# stop
killall yes
```

============================================
# 📊 Screenshots
============================================

## Grafana Dashboard – CPU & Memory Monitoring

<img src="screenshots/Picture1.png" alt="Grafana Dashboard 1" width="800">

---

## Grafana Dashboard – Detailed Metrics View

<img src="screenshots/Picture2.png" alt="Grafana Dashboard 2" width="800">

---

## Prometheus Metrics Endpoint

<img src="screenshots/Picture3.png" alt="Prometheus UI" width="800">

============================================
# 🐳 Run Full Stack (Docker Compose)
============================================

```bash
docker-compose up -d
```

- **Monitor:** http://localhost:9100/metrics
- **Prometheus:** http://localhost:9090
- **Grafana:** http://localhost:3000

============================================
# 🛠 Technologies Used
============================================

- C++17
- Linux `/proc` filesystem
- Docker / Podman
- Prometheus & Grafana

============================================
# 👤 Author
============================================

Mohammed Ahmed Ali

============================================
# 📜 License
============================================

This project is provided for educational and demonstration purposes.

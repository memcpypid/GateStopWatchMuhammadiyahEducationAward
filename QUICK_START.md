# Gate Timer - Quick Start (5 Minutes)

## 🚀 Fast Setup

### Backend (Go Server)

```bash
cd backend
go mod tidy
COMM_MODE=websocket go run main.go
```

**Expected Output**:
```
Communication Mode: websocket
HTTP/WS Port: 8080
Starting WebSocket server on :8080
```

### Frontend (Vue 3 with Vue CLI)

In a new terminal:

```bash
npm install
npm run serve
```

Frontend will be available at: `http://localhost:8081`

**Expected Output**:
```
  App running at http://localhost:5173
```

### ESP32 (Choose One)

Select based on communication mode:

**WebSocket Mode**:
1. Open: `backend/esp32_examples/websocket_example.ino`
2. Update WiFi credentials and server IP
3. Upload to ESP32

**HTTP Mode**:
1. Open: `backend/esp32_examples/http_post_example.ino`
2. Update WiFi credentials and server IP
3. Upload to ESP32

**UDP Mode**:
1. Open: `backend/esp32_examples/udp_example.ino`
2. Update WiFi credentials and server IP
3. Upload to ESP32

## ✅ Verify Setup

### Test Backend
```bash
curl http://localhost:8080/health
# Should return: {"status":"ok","mode":"websocket"}
```

### Test Frontend
1. Open browser: `http://localhost:5173`
2. Click "Connect"
3. Should show "Status: Connected" (green light)

### Test Triggers (HTTP mode)
```bash
curl -X POST http://localhost:8080/trigger -d "start"
curl -X POST http://localhost:8080/trigger -d "finish"
```

## 📋 Switching Modes

### Method 1: Use Settings Button
Click ⚙️ **Settings** button on frontend → Select mode → Apply

### Method 2: Manual Configuration

**Backend**:
```bash
# Edit backend/.env
COMM_MODE=http    # or udp, websocket
PORT=8080
go run main.go
```

**Frontend**:
```bash
# Edit .env (Vue CLI uses VUE_APP_ prefix)
VUE_APP_COMM_MODE=http
VUE_APP_BACKEND_URL=http://localhost:8080
npm run serve
```

## 🔧 Configuration Quick Reference

| Component | Config File | Key Variable |
|-----------|-----------|--------------|
| Backend | `backend/.env` | `COMM_MODE` |
| Frontend | `.env` | `VUE_APP_COMM_MODE` |
| Frontend | `.env` | `VUE_APP_BACKEND_URL` |
| Frontend | `.env` | `VUE_APP_WS_URL` |

## 🎯 Communication Modes Explained

```
┌──────────────┐
│ WebSocket    │ ← Real-time, bi-directional (BEST for interactive)
│ :8080        │
└──────────────┘

┌──────────────┐
│ HTTP POST    │ ← Simple REST API (BEST for simple integration)
│ :8080        │
└──────────────┘

┌──────────────┐
│ UDP          │ ← Lightweight, fast (BEST for low-latency)
│ :9090        │
└──────────────┘
```

## 🐛 Troubleshooting

### "Cannot connect to backend"
```bash
# Check if backend is running
curl http://localhost:8080/health

# If not, restart backend
cd backend
go run main.go
```

### "ESP32 won't connect to WiFi"
- Verify SSID and password are correct
- Make sure it's a 2.4GHz network (not 5GHz)
- Check WiFi signal strength

### "Sensors not triggering"
- Verify sensor wiring
- Check sensor logic (active HIGH or LOW?)
- Test sensor with simple Arduino sketch

## 📚 Full Documentation

For detailed setup and troubleshooting:
- Backend: `backend/README.md`
- Complete Setup: `SETUP_GUIDE.md`

## 🆕 First Time Users

1. **Start Backend**: `cd backend && go run main.go`
2. **Start Frontend**: `npm run serve`
3. **Open Browser**: `http://localhost:5173`
4. **Click Connect**: See green status light ✅
5. **Test with cURL**: `curl -X POST http://localhost:8080/trigger -d "start"`

## ⚡ Commands Cheat Sheet

```bash
# Backend
cd backend && go run main.go           # Start WebSocket
COMM_MODE=http go run main.go          # Start HTTP
COMM_MODE=udp go run main.go           # Start UDP

# Frontend
npm run serve                          # Dev server
npm run build                          # Production build
npm run lint                           # Check code

# Testing
curl http://localhost:8080/health      # Health check
curl -X POST http://localhost:8080/trigger -d "start"
echo "start" | nc -u localhost 9090    # UDP test
```

## 🎮 Frontend UI Guide

| Button | Action |
|--------|--------|
| **Waktu Persiapan** | Start preparation countdown (60s) |
| **Start** | Enable start sensor trigger |
| **Deteksi Finish** | Enable finish sensor trigger |
| **Retry Finish** | Reset time lapse |
| **Reset** | Reset everything |
| **⚙️ Settings** | Change communication mode |
| **Connect** | Connect to backend |

## 📊 Timer Display

```
┌─────────────────────────┐
│   Timer Display         │
│    MM:SS:MS             │
│                         │
│  Nama Sekolah: Sekolah A│
└─────────────────────────┘

┌─────────────────────────┐
│   Time Lapse Display    │
│    MM:SS:MS             │
│  (Result after finish)  │
└─────────────────────────┘
```

## 🚢 For Boat Racing

1. **Preparation**: Click "Waktu Persiapan" (60 second countdown)
2. **Waiting**: Click "Start" button (system waits for ESP32 trigger)
3. **Race Start**: ESP32 #1 sensor detects boat, timer starts automatically
4. **Enable Finish**: Click "Deteksi Finish"
5. **Race Finish**: ESP32 #2 sensor detects boat, timer stops automatically
6. **Review**: Check "Time Lapse" for final time
7. **Next Race**: Click "Retry Finish" to reset and repeat

## 🎯 Next Steps

1. Read `SETUP_GUIDE.md` for detailed instructions
2. Check `backend/README.md` for backend configuration
3. Review ESP32 example code with comments
4. Deploy to production when ready

---

**Need help?** Check the full documentation files for detailed troubleshooting.

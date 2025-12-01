# Backend Logging Guide

The backend now includes comprehensive logging to track all incoming requests and timer state changes.

## What's Logged

### 1. **Startup Information**
```
================================================================================
🚀 Gate Timer Backend Starting
================================================================================
📡 Starting WebSocket server on :8080
Available endpoints:
  - WS     /ws           (WebSocket connection)
  - GET    /health       (Health check)
  - GET    /status       (Get timer status)
Communication Mode: websocket
HTTP/WS Port: 8080
UDP Port: 9090
```

### 2. **Request/Response Logging**
Every HTTP request is logged with:
- **Timestamp** (HH:MM:SS.ms format)
- **Request direction** (📥 for incoming, 📤 for outgoing)
- **HTTP method** (GET, POST, etc.)
- **URL/Path** (e.g., /trigger, /health)
- **Client IP address**
- **Response status code** (200, 400, 500, etc.)
- **Response time in milliseconds**
- **Status emoji** (✅ for success, ❌ for errors)

Example:
```
[12:35:45.123] 📥 REQUEST  | POST /trigger | From: 127.0.0.1:54321 | Content-Type: text/plain
⏱️  TRIGGER RECEIVED | Type: start | Device: 
✅ Trigger processed successfully: start
[12:35:45.456] 📤 RESPONSE | 200 | Duration: 333ms | ✅
```

### 3. **Timer State Changes**
When a trigger is received:
```
⏱️  🟢 Timer STARTED | Time: 12:35:45.123
⏱️  🔴 Timer FINISHED | Elapsed: 5000 ms (5.00 sec) | Result: 00:05:000
```

### 4. **WebSocket Logging**
Connection events:
```
🔌 WebSocket connected | Client: 127.0.0.1:54322 | Total clients: 1
⏱️  TRIGGER RECEIVED (WebSocket) | Type: start | From: 127.0.0.1:54322
📡 Status broadcast to 1/1 WebSocket clients | Status: Running
🔌 WebSocket disconnected | Client: 127.0.0.1:54322 | Remaining clients: 0
```

### 5. **UDP Logging** (when using UDP mode)
```
⏱️  TRIGGER RECEIVED (UDP) | Type: start | From: 127.0.0.1:9999 | Bytes: 5
✅ UDP ACK sent to: 127.0.0.1:9999
```

### 6. **Status Requests**
```
📊 Status requested | Running: true | Status: Running | Elapsed: 3500 ms
```

## Running with Logs

### HTTP Mode
```bash
cd backend
COMM_MODE=http go run main.go
```

### WebSocket Mode (Default)
```bash
cd backend
COMM_MODE=websocket go run main.go
# or just
go run main.go
```

### UDP Mode
```bash
cd backend
COMM_MODE=udp go run main.go
```

## Log Symbols Reference

| Symbol | Meaning |
|--------|---------|
| 🚀 | Server startup |
| 📡 | Network/Server info |
| 📥 | Incoming request |
| 📤 | Outgoing response |
| ⏱️ | Timer event |
| 🟢 | Timer started |
| 🔴 | Timer stopped/finished |
| 🔌 | WebSocket event |
| 📊 | Status query |
| ✅ | Success |
| ❌ | Error |
| ⚠️ | Warning |

## Testing the Backend with Logs

### 1. Start Backend
```bash
cd backend
go run main.go
```

Expected output:
```
================================================================================
🚀 Gate Timer Backend Starting
================================================================================
📡 Starting WebSocket server on :8080
Available endpoints:
  - WS     /ws           (WebSocket connection)
  - GET    /health       (Health check)
  - GET    /status       (Get timer status)
Communication Mode: websocket
HTTP/WS Port: 8080
UDP Port: 9090
```

### 2. Check Health (in another terminal)
```bash
curl http://localhost:8080/health
```

Backend logs:
```
[12:35:45.123] 📥 REQUEST  | GET /health | From: 127.0.0.1:54321 | Content-Type: 
[12:35:45.124] 📤 RESPONSE | 200 | Duration: 1ms | ✅
```

### 3. Send Trigger (HTTP mode)
Switch backend to HTTP mode:
```bash
COMM_MODE=http go run main.go
```

Send trigger:
```bash
curl -X POST http://localhost:8080/trigger -d "start"
```

Backend logs:
```
[12:35:50.123] 📥 REQUEST  | POST /trigger | From: 127.0.0.1:54322 | Content-Type: text/plain
⏱️  TRIGGER RECEIVED | Type: start | Device: 
⏱️  🟢 Timer STARTED | Time: 12:35:50.456
✅ Trigger processed successfully: start
[12:35:50.124] 📤 RESPONSE | 200 | Duration: 1ms | ✅
```

### 4. Get Status
```bash
curl http://localhost:8080/status
```

Backend logs:
```
[12:35:55.123] 📥 REQUEST  | GET /status | From: 127.0.0.1:54323 | Content-Type: 
📊 Status requested | Running: true | Status: Running | Elapsed: 5000 ms
[12:35:55.124] 📤 RESPONSE | 200 | Duration: 2ms | ✅
```

## Frontend Logs (Browser Console)

Open browser DevTools (F12) and check the console:

```
🚀 Timer app started
📡 Backend URL: http://localhost:8080
✅ Connected to backend: http://localhost:8080
✅ Trigger sent to backend: start
```

## Troubleshooting

### Backend won't start
```
❌ HTTP server error: listen tcp :8080: bind: address already in use
```
**Solution**: Port 8080 is in use. Either:
- Kill the process using port 8080
- Change PORT in backend/.env: `PORT=3000`

### Frontend can't reach backend
```
❌ Cannot reach backend: http://localhost:8080 Error: fetch failed
```
**Solution**:
1. Make sure backend is running: `cd backend && go run main.go`
2. Check backend URL in `.env`: `VUE_APP_BACKEND_URL=http://localhost:8080`
3. Check firewall settings

### No logs appearing
**Solution**:
- Make sure you're running the backend from the project directory
- Check that COMM_MODE matches your expected communication method
- Look for error messages at startup

## Real-time Monitoring

To see all logs in real-time, pipe output to a file or use `tee`:

```bash
# Save logs to file while viewing in terminal
cd backend
go run main.go | tee backend.log

# View logs in real-time from another terminal
tail -f backend.log
```

## Full Example Workflow

Terminal 1 - Start Backend:
```bash
cd backend
go run main.go
```

Terminal 2 - Open Browser:
```
http://localhost:36760  # Vue dev server
```

Terminal 3 - Watch Backend Logs:
```bash
# Keep watching the backend terminal
tail -f
```

Expected log sequence when clicking Start button in frontend:
```
[HH:MM:SS] 📥 REQUEST  | POST /trigger | From: 127.0.0.1:xxxxx | Content-Type: text/plain
⏱️  TRIGGER RECEIVED | Type: start | Device: 
⏱️  🟢 Timer STARTED | Time: HH:MM:SS.xxx
✅ Trigger processed successfully: start
[HH:MM:SS] 📤 RESPONSE | 200 | Duration: 1ms | ✅
```

## Performance Monitoring

From the response logs, you can see performance:
- **Duration: 1-5ms** - Normal and fast
- **Duration: 10-50ms** - Acceptable
- **Duration: >100ms** - Slow, check network or server load

Example of timing analysis:
```
[12:35:45.123] 📥 REQUEST  | POST /trigger | ...
⏱️  🟢 Timer STARTED | ...
[12:35:45.125] 📤 RESPONSE | 200 | Duration: 2ms | ✅
```
Processing time: 2ms (very fast!)

## Next Steps

1. ✅ Backend has comprehensive logging
2. ✅ Frontend connects to backend and sends triggers
3. ⬜ Monitor logs during testing to ensure everything works
4. ⬜ Program ESP32 with actual sensors
5. ⬜ Test with real ESP32 triggers

---

For more details, see:
- `backend/README.md` - Backend documentation
- `SETUP_GUIDE.md` - Complete setup instructions
- `QUICK_START.md` - Quick start guide

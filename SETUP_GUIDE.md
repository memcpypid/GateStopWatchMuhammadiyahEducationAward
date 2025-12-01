# Gate Timer - Complete Setup Guide

A comprehensive guide for setting up the Gate Timer system with 3 communication methods (HTTP, WebSocket, UDP) for boat racing competitions.

## System Architecture

```
┌─────────────┐
│   ESP32 #1  │ (Start Sensor)
│  ──────     │
└──────┬──────┘
       │
       ├─── HTTP POST ──────┐
       ├─── WebSocket ─────┐│
       │                   ││
┌──────────────────────────┘│
│                           │
│   ┌──────────────────┐    │
│   │  Go Backend      │◄───┘
│   │  :8080           │
│   │  :9090 (UDP)     │
│   └────────┬─────────┘
│            │
│   ┌────────▼─────────┐
│   │  Timer State     │
│   │  Management      │
│   └────────┬─────────┘
│            │
│   ┌────────▼─────────┐
│   │  Status & Data   │
│   │  Storage         │
│   └──────────────────┘
│
│   ┌────────────────────┐
│   │  Vue 3 Frontend    │
│   │  :5173             │
│   │  WebSocket Client  │
│   │  HTTP Client       │
│   └────────────────────┘
│
└─────────────────────────┐
                          │
                    ┌─────▼─────┐
                    │  ESP32 #2  │ (Finish Sensor)
                    │  ──────    │
                    └────────────┘
```

## Prerequisites

### For Backend
- Go 1.21 or higher
- GitHub, Git, or manual download

### For Frontend
- Node.js 16+ and npm/yarn
- Modern web browser

### For ESP32
- Arduino IDE or PlatformIO
- ESP32 board support
- USB cable for programming
- Optional: WebSocket, HTTP, or UDP libraries

### Hardware
- 2x ESP32 Development Boards
- 2x Infrared beam break sensors (or similar trigger sensors)
- Optional: Status LEDs, resistors, wiring

## Step-by-Step Setup

### Step 1: Setup Backend Server

#### 1.1 Navigate to backend directory

```bash
cd backend
```

#### 1.2 Install Go dependencies

```bash
go mod tidy
go get github.com/gorilla/websocket@latest
```

#### 1.3 Configure environment

Copy the example configuration:

```bash
cp .env.example .env
```

Edit `.env` to select communication mode:

```env
# Choose one: websocket, http, or udp
COMM_MODE=websocket

PORT=8080
UDP_PORT=9090
```

#### 1.4 Run the backend server

```bash
go run main.go
```

Expected output:
```
Communication Mode: websocket
HTTP/WS Port: 8080
Starting WebSocket server on :8080
```

### Step 2: Setup Frontend

#### 2.1 Navigate to frontend directory

```bash
cd ..
```

#### 2.2 Install dependencies

```bash
npm install
# or
yarn install
# or
pnpm install
```

#### 2.3 Configure frontend environment

Copy the example configuration:

```bash
cp .env.example .env
```

Edit `.env` to match your backend setup:

```env
VITE_COMM_MODE=websocket
VITE_BACKEND_URL=http://localhost:8080
VITE_WS_URL=ws://localhost:8080
```

#### 2.4 Run development server

```bash
npm run serve
# or
yarn serve
# or
pnpm serve
```

Frontend will be available at: `http://localhost:5173`

### Step 3: Setup ESP32 Sensors

#### 3.1 Install Arduino IDE

Download from: https://www.arduino.cc/en/software

#### 3.2 Install ESP32 Board Support

1. Go to **File** → **Preferences**
2. Add to "Additional Boards Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools** → **Board Manager**
4. Search for "ESP32" and install

#### 3.3 Install Required Libraries

1. Go to **Sketch** → **Include Library** → **Manage Libraries**
2. Search and install:
   - WebSocketsClient (for WebSocket mode)
   - ArduinoJson (recommended for both modes)

#### 3.4 Select Communication Method

Choose one of the example sketches based on your backend configuration:

- **WebSocket Mode**: `backend/esp32_examples/websocket_example.ino`
- **HTTP Mode**: `backend/esp32_examples/http_post_example.ino`
- **UDP Mode**: `backend/esp32_examples/udp_example.ino`

#### 3.5 Configure ESP32 Code

Edit the selected sketch and update:

```cpp
// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Server Configuration (update to match your setup)
const char* websocket_server = "192.168.1.100";  // Your server IP
const uint16_t websocket_port = 8080;

// Sensor Pins (adjust based on your hardware)
const int START_SENSOR_PIN = 34;   // GPIO34
const int FINISH_SENSOR_PIN = 35;  // GPIO35
```

#### 3.6 Upload to ESP32

1. Connect ESP32 via USB
2. Select **Tools** → **Board** → **ESP32 Dev Module**
3. Select correct COM port
4. Click **Upload**

## Testing the System

### Test Backend Health

```bash
# Check if backend is running
curl http://localhost:8080/health

# Expected response:
# {"status":"ok","mode":"websocket"}
```

### Test Timer Status

```bash
# Get current timer status
curl http://localhost:8080/status

# Expected response includes running status, elapsed time, etc.
```

### Test HTTP Trigger (HTTP mode only)

```bash
# Start timer
curl -X POST http://localhost:8080/trigger -d "start"

# Finish timer
curl -X POST http://localhost:8080/trigger -d "finish"
```

### Test with Frontend

1. Open browser: `http://localhost:5173`
2. Enter backend IP address (or leave empty for localhost)
3. Click "Connect"
4. You should see "Status: Connected" with green indicator

### Test with ESP32

1. Ensure ESP32 is powered and connected to WiFi
2. Open Arduino Serial Monitor (9600 baud)
3. Watch for connection messages
4. Trigger sensor and observe:
   - Serial output
   - Frontend timer starting/stopping
   - Backend logs

## Switching Between Communication Modes

### To Change Mode

1. **Backend**: Edit `backend/.env`
   ```env
   COMM_MODE=http  # Change from websocket to http
   ```

2. **Frontend**: Edit `.env`
   ```env
   VITE_COMM_MODE=http
   VITE_BACKEND_URL=http://localhost:8080
   ```

3. **Restart both servers**

4. **Update ESP32 code** to use appropriate example

### Quick Mode Switch Table

| Mode | Command | Backend Config | Frontend Config |
|------|---------|---|---|
| WebSocket | `go run main.go` | `COMM_MODE=websocket` | `VITE_COMM_MODE=websocket` |
| HTTP | `go run main.go` | `COMM_MODE=http` | `VITE_COMM_MODE=http` |
| UDP | `go run main.go` | `COMM_MODE=udp` | `VITE_COMM_MODE=udp` |

## Deployment

### Docker Deployment

#### Build Docker image

```bash
docker build -t gatetiimer .
```

#### Run with Docker

```bash
docker run -p 8080:8080 -p 9090:9090 \
  -e COMM_MODE=websocket \
  gatetiimer
```

### Production Deployment

For production, consider:

1. **HTTPS/WSS**: Use SSL certificates for secure communication
2. **Authentication**: Add token validation
3. **CORS**: Configure proper CORS headers
4. **Rate Limiting**: Prevent abuse
5. **Logging**: Implement proper logging
6. **Monitoring**: Add error tracking (Sentry, etc.)

Example production config:

```env
COMM_MODE=websocket
PORT=8443
TLS_CERT=/etc/certs/cert.pem
TLS_KEY=/etc/certs/key.pem
ALLOWED_ORIGINS=https://timer.example.com
```

## Troubleshooting

### Backend won't start

**Error**: `bind: address already in use`
- Port 8080 is already in use
- Solution: Change PORT in `.env` or kill existing process

**Error**: `no such file or directory`
- Missing `go.mod` or `main.go`
- Solution: Ensure you're in `backend` directory

### Frontend won't connect

**Error**: `Failed to connect to http://localhost:8080`
- Backend not running
- Solution: Start backend server first
- Check with: `curl http://localhost:8080/health`

**Error**: `WebSocket connection failed`
- WebSocket server not running
- Check backend configuration: `COMM_MODE=websocket`
- Verify firewall allows port 8080

### ESP32 won't connect

**Error**: `Connecting to WiFi: .....`
- WiFi credentials wrong
- WiFi network is 5GHz (ESP32 needs 2.4GHz)
- Solution: Check SSID and password in sketch

**Error**: `Cannot connect to backend server`
- Wrong IP address
- Firewall blocking connection
- Solution: Verify IP with: `ipconfig` (Windows) or `ifconfig` (Linux/Mac)

**Error**: Sensor not triggering
- Check sensor wiring
- Verify sensor logic (active HIGH or LOW?)
- Test with simple sketch:
  ```cpp
  void setup() {
    Serial.begin(115200);
    pinMode(34, INPUT);
  }
  void loop() {
    Serial.println(digitalRead(34));
    delay(100);
  }
  ```

### Timer not starting

1. **Verify connection**: Check green status indicator
2. **Check firewall**: Allow port 8080 and 9090
3. **Test with curl**: `curl -X POST http://localhost:8080/trigger -d "start"`
4. **Check logs**: Watch serial output and backend console

## Common Commands

```bash
# Backend
cd backend
go run main.go                    # Run server
COMM_MODE=http go run main.go    # Run with HTTP mode

# Frontend
npm install                       # Install dependencies
npm run serve                     # Dev server
npm run build                     # Production build
npm run lint                      # Check code style

# Testing
curl http://localhost:8080/health           # Health check
curl -X POST http://localhost:8080/trigger -d "start"
echo "finish" | nc -u localhost 9090        # UDP test
```

## Architecture Decisions

### Why 3 Communication Methods?

1. **WebSocket**: Real-time, bidirectional, best for interactive apps
2. **HTTP**: Simple, stateless, easy to integrate
3. **UDP**: Minimal latency, best for time-critical applications

Choose based on your needs:
- **Reliability important**: Use WebSocket or HTTP
- **Latency critical**: Use UDP
- **Simple integration**: Use HTTP
- **Real-time feedback**: Use WebSocket

### Why Go?

- Fast and efficient
- Excellent for concurrent connections
- Built-in networking support
- Easy to deploy (single binary)
- Good for high-performance applications

### Why Vue 3?

- Modern, reactive UI framework
- Easy state management
- Good performance
- Great developer experience
- Responsive design out of the box

## Security Considerations

### Never commit secrets!

If using authentication tokens or API keys:

```bash
# Use environment variables instead
export API_KEY="your-secret-key"
# Or use .env.local (local development only)
```

### CORS Security

In production, configure CORS properly:

```go
// In main.go, add:
w.Header().Set("Access-Control-Allow-Origin", "https://timer.example.com")
w.Header().Set("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
```

### Firewall Rules

Allow only necessary ports:
- Port 8080: HTTP/WebSocket (backend)
- Port 9090: UDP (backend)
- Port 5173: Frontend (dev only)

### Network Segmentation

In production, keep devices on a private network if possible.

## Performance Optimization

### WebSocket Optimization
- Keep connections alive with heartbeat
- Compress messages for large data
- Implement backpressure for high-frequency updates

### HTTP Optimization
- Use connection pooling
- Minimize payload size
- Consider caching for status endpoint

### UDP Optimization
- Use fire-and-forget model
- Implement retry logic if needed
- Monitor packet loss

## Next Steps

1. ✅ Setup backend Go server
2. ✅ Setup Vue 3 frontend
3. ✅ Program ESP32 sensors
4. ✅ Test communication
5. ⬜ Deploy to production
6. ⬜ Monitor and maintain

## Support and Resources

- Go Documentation: https://golang.org/doc/
- Vue 3 Documentation: https://vuejs.org/
- ESP32 Documentation: https://docs.espressif.com/
- WebSocket Protocol: https://tools.ietf.org/html/rfc6455
- UDP Protocol: https://tools.ietf.org/html/rfc768

## License

MIT License - Feel free to modify and distribute

---

For questions or issues, check the troubleshooting section or review the backend/README.md for more details.

# Gate Timer Backend - Go

A flexible backend server for handling ESP32 triggers in a gate timer system. Supports 3 different communication methods.

## Features

- **HTTP POST**: Simple REST API for trigger events
- **WebSocket**: Real-time bidirectional communication
- **UDP**: Lightweight UDP protocol for ESP32 direct communication

## Prerequisites

- Go 1.21 or higher
- Gorilla WebSocket library (auto-installed via `go get`)

## Installation

```bash
cd backend
go get
```

## Configuration

Edit the `.env` file in the root directory to select communication mode:

```env
# Options: http, websocket, or udp
COMM_MODE=websocket
PORT=8080
UDP_PORT=9090
```

## Running the Server

```bash
# From backend directory
go run main.go
```

Or:

```bash
# From root directory
cd backend && go run main.go
```

## Communication Methods

### 1. WebSocket (Default)

**Best for**: Real-time updates, browser integration

**Configuration**:
```env
COMM_MODE=websocket
PORT=8080
```

**ESP32 Code Example**:
```cpp
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

void connectWebSocket() {
  webSocket.begin("192.168.1.100", 8080, "/ws");
  webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*) &payload[0]);
    if (message == "start") {
      // Handle start
    } else if (message == "finish") {
      // Handle finish
    }
  }
}

void sendTrigger(String trigger) {
  DynamicJsonDocument doc(200);
  doc["trigger"] = trigger;
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);
}
```

**Frontend Integration**:
```javascript
const ws = new WebSocket('ws://localhost:8080/ws');

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  if (data.running) {
    // Timer is running
  }
  if (data.status === 'Finished') {
    // Timer finished
  }
};

// Send trigger
ws.send(JSON.stringify({ trigger: 'start' }));
```

### 2. HTTP POST

**Best for**: Simple integration, stateless communication

**Configuration**:
```env
COMM_MODE=http
PORT=8080
```

**Endpoints**:

- `POST /trigger` - Send trigger event
- `GET /health` - Health check
- `GET /status` - Get current timer status

**ESP32 Code Example**:
```cpp
#include <WiFi.h>
#include <HTTPClient.h>

void sendTrigger(String trigger) {
  HTTPClient http;
  http.begin("http://192.168.1.100:8080/trigger");
  http.addHeader("Content-Type", "text/plain");
  
  int httpResponseCode = http.POST(trigger); // "start" or "finish"
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  }
  http.end();
}
```

**cURL Example**:
```bash
# Send start trigger
curl -X POST http://localhost:8080/trigger -d "start"

# Send finish trigger
curl -X POST http://localhost:8080/trigger -d "finish"

# Check status
curl http://localhost:8080/status
```

### 3. UDP

**Best for**: Minimal overhead, fire-and-forget

**Configuration**:
```env
COMM_MODE=udp
UDP_PORT=9090
```

**ESP32 Code Example**:
```cpp
#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP udp;

void setup() {
  udp.begin(9090);
}

void sendTrigger(String trigger) {
  udp.beginPacket("192.168.1.100", 9090);
  udp.write(trigger.c_str());
  udp.endPacket();
}

void loop() {
  // For receiving ACK from server
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    incomingPacket[len] = 0;
    Serial.println(incomingPacket);
  }
}
```

**netcat Example**:
```bash
# Send trigger via UDP
echo "start" | nc -u localhost 9090
echo "finish" | nc -u localhost 9090
```

## API Reference

### GET /health
Returns server health status and current communication mode.

**Response**:
```json
{
  "status": "ok",
  "mode": "websocket"
}
```

### POST /trigger (HTTP mode only)
Send a trigger event to the timer.

**Request Body**:
- Plain text: `start` or `finish`
- Or JSON: `{"trigger_type": "start", "device_id": "esp32_1"}`

**Response**:
```json
{
  "success": true,
  "message": "Received start trigger",
  "timestamp": "2024-01-15T10:30:45Z"
}
```

### GET /status
Get current timer status.

**Response**:
```json
{
  "running": true,
  "start_time": "2024-01-15T10:30:45Z",
  "finish_time": "0001-01-01T00:00:00Z",
  "elapsed_ms": 5000,
  "status": "Running",
  "mode": "websocket"
}
```

## Switching Between Modes

To switch communication method:

1. Edit `.env` file and change `COMM_MODE`
2. Restart the backend server
3. Update frontend/ESP32 configuration accordingly

Example workflow:

```bash
# Test with WebSocket (default)
COMM_MODE=websocket go run backend/main.go

# Test with HTTP
COMM_MODE=http go run backend/main.go

# Test with UDP
COMM_MODE=udp go run backend/main.go
```

## Docker Deployment

Create a `Dockerfile`:

```dockerfile
FROM golang:1.21-alpine

WORKDIR /app
COPY go.mod go.sum ./
RUN go mod download

COPY . .
RUN go build -o gatetiimer main.go

EXPOSE 8080 9090
CMD ["./gatetiimer"]
```

Build and run:

```bash
docker build -t gatetiimer .
docker run -p 8080:8080 -p 9090:9090 -e COMM_MODE=websocket gatetiimer
```

## Troubleshooting

**ESP32 can't connect to server**:
- Check firewall settings
- Verify IP address and port are correct
- Ensure backend is running: `curl http://<IP>:8080/health`

**WebSocket connection fails**:
- Check browser console for CORS issues
- Verify WS URL matches backend address
- Ensure no proxy is blocking WebSocket upgrade

**UDP messages not received**:
- Check UDP port is not blocked by firewall
- Verify ESP32 sends to correct IP and port
- Test with `netcat`: `echo "start" | nc -u <IP> 9090`

## Testing

```bash
# Test WebSocket mode
# In one terminal
COMM_MODE=websocket go run backend/main.go

# In another terminal, use wscat or similar
# npm install -g wscat
# wscat -c ws://localhost:8080/ws

# Test HTTP mode
COMM_MODE=http go run backend/main.go
curl -X POST http://localhost:8080/trigger -d "start"

# Test UDP mode
COMM_MODE=udp go run backend/main.go
echo "start" | nc -u localhost 9090
```

## License

MIT

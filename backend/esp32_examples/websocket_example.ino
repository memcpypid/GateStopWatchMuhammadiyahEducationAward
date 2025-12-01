/*
 * ESP32 Gate Timer - WebSocket Example
 * 
 * This example shows how to connect an ESP32 to the Gate Timer backend
 * using WebSocket communication.
 * 
 * Hardware:
 * - ESP32 Development Board
 * - Start sensor (e.g., infrared beam break sensor)
 * - Finish sensor (e.g., infrared beam break sensor)
 * - Optional: Status LED
 * 
 * Libraries Required:
 * - WebSocketsClient by Markus Sattler (install via Arduino IDE Library Manager)
 * - ArduinoJson by Benoit Blanchon
 */

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// WebSocket Configuration
const char* websocket_server = "192.168.1.100";
const uint16_t websocket_port = 8080;

// Pin Configuration
const int START_SENSOR_PIN = 34;   // GPIO34 - Start sensor
const int FINISH_SENSOR_PIN = 35;  // GPIO35 - Finish sensor
const int STATUS_LED_PIN = 2;      // GPIO2 - Status LED (optional)

// WebSocket client
WebSocketsClient webSocket;

// State variables
bool start_triggered = false;
bool finish_triggered = false;
unsigned long last_trigger_time = 0;
const unsigned long DEBOUNCE_TIME = 100; // milliseconds

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nESP32 Gate Timer - WebSocket Mode");
  Serial.println("==================================");
  
  // Initialize pins
  pinMode(START_SENSOR_PIN, INPUT);
  pinMode(FINISH_SENSOR_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Setup WebSocket client
  setupWebSocket();
}

void loop() {
  // WebSocket client must be called regularly
  webSocket.loop();
  
  // Check sensors
  checkStartSensor();
  checkFinishSensor();
  
  delay(10);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi Connected! IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void setupWebSocket() {
  Serial.print("Setting up WebSocket connection to ");
  Serial.print(websocket_server);
  Serial.print(":");
  Serial.println(websocket_port);
  
  // Server address, port, and path
  webSocket.begin(websocket_server, websocket_port, "/ws");
  
  // Event handler
  webSocket.onEvent(webSocketEvent);
  
  // Reconnect automatically
  webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      digitalWrite(STATUS_LED_PIN, LOW);
      break;
      
    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected");
      digitalWrite(STATUS_LED_PIN, HIGH);
      // Send initial status
      webSocket.sendTXT("{\"device\": \"esp32_start_sensor\"}");
      break;
      
    case WStype_TEXT:
      Serial.print("[WebSocket] Received: ");
      Serial.println((char *)payload);
      // Handle messages from server if needed
      break;
      
    case WStype_BIN:
      Serial.println("[WebSocket] Received binary data");
      break;
      
    case WStype_ERROR:
      Serial.println("[WebSocket] Error");
      digitalWrite(STATUS_LED_PIN, LOW);
      break;
  }
}

void checkStartSensor() {
  int sensorValue = digitalRead(START_SENSOR_PIN);
  
  // LOW when sensor is triggered (beam broken)
  if (sensorValue == LOW && !start_triggered) {
    if (millis() - last_trigger_time > DEBOUNCE_TIME) {
      start_triggered = true;
      last_trigger_time = millis();
      
      Serial.println("[START] Sensor triggered!");
      sendTrigger("start");
    }
  } else if (sensorValue == HIGH) {
    start_triggered = false;
  }
}

void checkFinishSensor() {
  int sensorValue = digitalRead(FINISH_SENSOR_PIN);
  
  // LOW when sensor is triggered (beam broken)
  if (sensorValue == LOW && !finish_triggered) {
    if (millis() - last_trigger_time > DEBOUNCE_TIME) {
      finish_triggered = true;
      last_trigger_time = millis();
      
      Serial.println("[FINISH] Sensor triggered!");
      sendTrigger("finish");
    }
  } else if (sensorValue == HIGH) {
    finish_triggered = false;
  }
}

void sendTrigger(const char* trigger_type) {
  if (webSocket.isConnected()) {
    // Create JSON message
    DynamicJsonDocument doc(200);
    doc["trigger"] = trigger_type;
    doc["device"] = "esp32_sensor";
    doc["timestamp"] = millis();
    
    // Serialize to string
    String json;
    serializeJson(doc, json);
    
    // Send via WebSocket
    webSocket.sendTXT(json);
    
    Serial.print("[WebSocket] Sent trigger: ");
    Serial.println(json);
  } else {
    Serial.println("[WebSocket] Not connected, cannot send trigger");
  }
}

/*
 * Alternative: Simpler version without JSON (if ArduinoJson not available)
 * 
 * void sendTrigger(const char* trigger_type) {
 *   if (webSocket.isConnected()) {
 *     webSocket.sendTXT(trigger_type);
 *     Serial.print("[WebSocket] Sent: ");
 *     Serial.println(trigger_type);
 *   }
 * }
 */

/*
 * Troubleshooting:
 * 1. Board not connecting to WiFi?
 *    - Check SSID and password
 *    - Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
 * 
 * 2. WebSocket won't connect?
 *    - Verify backend is running and listening
 *    - Check IP address and port
 *    - Ensure firewall allows WebSocket connections
 *    - Check Serial output for error messages
 * 
 * 3. Sensors not triggering?
 *    - Check sensor wiring
 *    - Verify sensor is active LOW (beam break triggers LOW)
 *    - Test sensor with simple digital read sketch
 * 
 * 4. Trigger not received by backend?
 *    - Check WebSocket connection status (LED should be on)
 *    - Verify trigger type is "start" or "finish" (lowercase)
 *    - Check backend logs for incoming messages
 */

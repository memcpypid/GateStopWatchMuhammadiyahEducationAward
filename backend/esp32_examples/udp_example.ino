/*
 * ESP32 Gate Timer - UDP Example
 * 
 * This example shows how to connect an ESP32 to the Gate Timer backend
 * using UDP (User Datagram Protocol) communication.
 * 
 * Advantages:
 * - Very low latency
 * - Minimal overhead
 * - Fire-and-forget approach
 * - Built-in WiFi library support
 * 
 * Disadvantages:
 * - No guarantee of message delivery
 * - No connection state
 * - May lose packets on poor networks
 * 
 * Best for: Time-critical applications where speed matters more than reliability
 * 
 * Hardware:
 * - ESP32 Development Board
 * - Start sensor (e.g., infrared beam break sensor)
 * - Finish sensor (e.g., infrared beam break sensor)
 * - Optional: Status LED
 * 
 * Libraries Required:
 * - Built-in WiFi and WiFiUdp
 */

#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// UDP Configuration
const char* backend_ip = "192.168.1.100";
const uint16_t backend_port = 9090;

// Pin Configuration
const int START_SENSOR_PIN = 34;   // GPIO34 - Start sensor
const int FINISH_SENSOR_PIN = 35;  // GPIO35 - Finish sensor
const int STATUS_LED_PIN = 2;      // GPIO2 - Status LED

// UDP client
WiFiUDP udp;

// State variables
bool start_triggered = false;
bool finish_triggered = false;
unsigned long last_trigger_time = 0;
const unsigned long DEBOUNCE_TIME = 100;
unsigned long last_packet_time = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nESP32 Gate Timer - UDP Mode");
  Serial.println("=============================");
  
  // Initialize pins
  pinMode(START_SENSOR_PIN, INPUT);
  pinMode(FINISH_SENSOR_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize UDP
  Serial.print("Starting UDP on port: ");
  Serial.println(backend_port);
  udp.begin(backend_port);
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectToWiFi();
  }
  
  // Check for incoming UDP packets (ACK from server)
  checkUDPPackets();
  
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
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    Serial.println("Failed to connect to WiFi");
    digitalWrite(STATUS_LED_PIN, LOW);
  }
}

void checkUDPPackets() {
  int packetSize = udp.parsePacket();
  
  if (packetSize) {
    // Receive incoming UDP packet
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    
    if (len > 0) {
      incomingPacket[len] = 0;
      Serial.print("[UDP] Received ACK: ");
      Serial.println(incomingPacket);
      last_packet_time = millis();
    }
  }
}

void checkStartSensor() {
  int sensorValue = digitalRead(START_SENSOR_PIN);
  
  if (sensorValue == LOW && !start_triggered) {
    if (millis() - last_trigger_time > DEBOUNCE_TIME) {
      start_triggered = true;
      last_trigger_time = millis();
      
      Serial.println("[START] Sensor triggered!");
      sendTriggerUDP("start");
    }
  } else if (sensorValue == HIGH) {
    start_triggered = false;
  }
}

void checkFinishSensor() {
  int sensorValue = digitalRead(FINISH_SENSOR_PIN);
  
  if (sensorValue == LOW && !finish_triggered) {
    if (millis() - last_trigger_time > DEBOUNCE_TIME) {
      finish_triggered = true;
      last_trigger_time = millis();
      
      Serial.println("[FINISH] Sensor triggered!");
      sendTriggerUDP("finish");
    }
  } else if (sensorValue == HIGH) {
    finish_triggered = false;
  }
}

void sendTriggerUDP(const char* trigger_type) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[UDP] Not connected to WiFi!");
    return;
  }
  
  // Send UDP packet
  udp.beginPacket(backend_ip, backend_port);
  udp.write((uint8_t*)trigger_type, strlen(trigger_type));
  int result = udp.endPacket();
  
  if (result == 1) {
    Serial.print("[UDP] Sent trigger: ");
    Serial.print(trigger_type);
    Serial.print(" to ");
    Serial.print(backend_ip);
    Serial.print(":");
    Serial.println(backend_port);
  } else {
    Serial.println("[UDP] Failed to send packet");
  }
}

/*
 * Advanced: Send structured UDP message
 * 
 * void sendTriggerUDP(const char* trigger_type) {
 *   if (WiFi.status() != WL_CONNECTED) return;
 *   
 *   // Create message with device ID and timestamp
 *   String message = String(trigger_type) + "|esp32_01|" + String(millis());
 *   
 *   udp.beginPacket(backend_ip, backend_port);
 *   udp.write((uint8_t*)message.c_str(), message.length());
 *   udp.endPacket();
 *   
 *   Serial.print("[UDP] Sent: ");
 *   Serial.println(message);
 * }
 */

/*
 * Comparing UDP with HTTP and WebSocket:
 * 
 * +--------+----------+----------+--------+
 * |Feature | WebSocket| HTTP     | UDP    |
 * +--------+----------+----------+--------+
 * |Latency | Low      | Medium   | Very   |
 * |        |          |          | Low    |
 * +--------+----------+----------+--------+
 * |Overhead| Medium   | High     | Low    |
 * +--------+----------+----------+--------+
 * |Reliable| Yes      | Yes      | No     |
 * +--------+----------+----------+--------+
 * |Bi-direc| Yes      | Request  | No     |
 * |tional  |          | Response |        |
 * +--------+----------+----------+--------+
 * 
 * For Gate Timer:
 * - WebSocket: Best for real-time feedback and status updates
 * - HTTP: Best for simple integration, no special libraries
 * - UDP: Best for minimum latency on reliable local networks
 * 
 * Troubleshooting:
 * 1. UDP packets not reaching server?
 *    - Check firewall allows UDP port 9090
 *    - Verify IP address is correct
 *    - Check backend is running
 * 
 * 2. No ACK received from server?
 *    - UDP is connectionless, ACK may be lost
 *    - This is normal for UDP
 *    - Verify backend logs show trigger received
 * 
 * 3. Sensors not triggering?
 *    - Check sensor wiring
 *    - Verify sensor is active LOW
 *    - Test with simple digital read
 * 
 * 4. WiFi issues?
 *    - Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
 *    - Check signal strength
 *    - Reduce distance from router
 */

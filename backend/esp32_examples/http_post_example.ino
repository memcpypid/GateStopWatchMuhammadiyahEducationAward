/*
 * ESP32 Gate Timer - HTTP POST Example
 * 
 * This example shows how to connect an ESP32 to the Gate Timer backend
 * using HTTP POST requests.
 * 
 * Advantages:
 * - Simpler to implement
 * - No WebSocket library needed
 * - Works with simple HTTP communication
 * 
 * Disadvantages:
 * - Slightly higher latency (request-response cycle)
 * - Less efficient for high-frequency updates
 * 
 * Hardware:
 * - ESP32 Development Board
 * - Start sensor (e.g., infrared beam break sensor)
 * - Finish sensor (e.g., infrared beam break sensor)
 * - Optional: Status LED
 * 
 * Libraries Required:
 * - Built-in WiFi and HTTPClient
 * - Optional: ArduinoJson for JSON formatting
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// HTTP Configuration
const char* backend_url = "http://192.168.1.100:8080/trigger";

// Pin Configuration
const int START_SENSOR_PIN = 34;   // GPIO34 - Start sensor
const int FINISH_SENSOR_PIN = 35;  // GPIO35 - Finish sensor
const int STATUS_LED_PIN = 2;      // GPIO2 - Status LED

// State variables
bool start_triggered = false;
bool finish_triggered = false;
unsigned long last_trigger_time = 0;
const unsigned long DEBOUNCE_TIME = 100;
const unsigned long HTTP_TIMEOUT = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nESP32 Gate Timer - HTTP POST Mode");
  Serial.println("==================================");
  
  // Initialize pins
  pinMode(START_SENSOR_PIN, INPUT);
  pinMode(FINISH_SENSOR_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectToWiFi();
  }
  
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

void checkStartSensor() {
  int sensorValue = digitalRead(START_SENSOR_PIN);
  
  if (sensorValue == LOW && !start_triggered) {
    if (millis() - last_trigger_time > DEBOUNCE_TIME) {
      start_triggered = true;
      last_trigger_time = millis();
      
      Serial.println("[START] Sensor triggered!");
      sendTriggerHTTP("start");
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
      sendTriggerHTTP("finish");
    }
  } else if (sensorValue == HIGH) {
    finish_triggered = false;
  }
}

void sendTriggerHTTP(const char* trigger_type) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[HTTP] Not connected to WiFi!");
    return;
  }
  
  HTTPClient http;
  
  Serial.print("[HTTP] Connecting to: ");
  Serial.println(backend_url);
  
  http.begin(backend_url);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("Content-Type", "text/plain");
  
  // Send POST request with trigger type as plain text
  int httpResponseCode = http.POST(trigger_type);
  
  if (httpResponseCode > 0) {
    Serial.print("[HTTP] Response Code: ");
    Serial.println(httpResponseCode);
    
    String response = http.getString();
    Serial.print("[HTTP] Response: ");
    Serial.println(response);
  } else {
    Serial.print("[HTTP] Error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }
  
  http.end();
}

/*
 * Alternative: Send as JSON
 * 
 * void sendTriggerHTTP(const char* trigger_type) {
 *   HTTPClient http;
 *   http.begin(backend_url);
 *   http.addHeader("Content-Type", "application/json");
 *   
 *   // Create JSON payload
 *   DynamicJsonDocument doc(200);
 *   doc["trigger_type"] = trigger_type;
 *   doc["device_id"] = "esp32_01";
 *   doc["timestamp"] = millis();
 *   
 *   String json;
 *   serializeJson(doc, json);
 *   
 *   int httpResponseCode = http.POST(json);
 *   
 *   if (httpResponseCode > 0) {
 *     Serial.print("HTTP Response Code: ");
 *     Serial.println(httpResponseCode);
 *     String response = http.getString();
 *     Serial.println(response);
 *   } else {
 *     Serial.print("HTTP Error: ");
 *     Serial.println(http.errorToString(httpResponseCode));
 *   }
 *   
 *   http.end();
 * }
 */

/*
 * Troubleshooting:
 * 1. HTTP Post fails with error?
 *    - Check WiFi connection
 *    - Verify backend URL is correct
 *    - Ensure backend server is running
 *    - Check firewall settings
 * 
 * 2. Slow response?
 *    - HTTP has more overhead than WebSocket
 *    - Consider using WebSocket for better latency
 *    - Check network speed
 * 
 * 3. Connection timeout?
 *    - Increase HTTP_TIMEOUT value
 *    - Check backend server is responsive
 *    - Verify network connectivity
 * 
 * 4. Sensors not working?
 *    - Check sensor wiring
 *    - Verify sensor is active LOW
 *    - Test with simple digital read sketch
 */

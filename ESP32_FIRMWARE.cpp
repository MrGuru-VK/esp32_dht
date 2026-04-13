/*
 * ESP32 DHT Sensor Firmware
 * 
 * This code runs on the ESP32 with a DHT sensor connected to GPIO4
 * It provides a JSON API endpoint for the Rust web server to fetch sensor data
 * 
 * Install Dependencies:
 * - DHT sensor library: https://github.com/adafruit/DHT-sensor-library
 * - ArduinoJson: https://github.com/bblanchon/ArduinoJson
 * 
 * Upload with:
 * - Arduino IDE or PlatformIO
 * - Board: ESP32 Dev Module
 * - Flash Frequency: 80MHz
 

#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ============= Configuration =============

// DHT Sensor
#define DHTPIN 4           // GPIO pin where DHT data line is connected
#define DHTTYPE DHT22      // DHT22 sensor (or DHT11, DHT21)
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char* ssid = "your_ssid";           // Change to your WiFi SSID
const char* password = "your_password";   // Change to your WiFi password

// Web Server
WebServer server(80);

// ============= Setup =============

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("🌡️  DHT Sensor ESP32 Firmware Starting...");
    
    // Initialize DHT sensor
    dht.begin();
    Serial.println("✅ DHT sensor initialized");
    
    // Connect to WiFi
    connectToWiFi();
    
    // Setup web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/sensor", HTTP_GET, handleSensor);
    server.on("/status", HTTP_GET, handleStatus);
    server.onNotFound(handleNotFound);
    
    // Start server
    server.begin();
    Serial.println("🚀 Web server started on port 80");
    Serial.print("📍 Access the sensor API at: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/sensor");
}

// ============= Main Loop =============

void loop() {
    server.handleClient();
    delay(10);
}

// ============= WiFi Connection =============

void connectToWiFi() {
    Serial.print("📡 Connecting to WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" ✅");
        Serial.print("📍 IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println(" ❌");
        Serial.println("⚠️  WiFi connection failed!");
    }
}

// ============= HTTP Request Handlers =============

void handleRoot() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 DHT Sensor API</title>
    <style>
        body { font-family: Arial; margin: 40px; background: #f0f0f0; }
        .container { background: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h1 { color: #333; }
        code { background: #eee; padding: 2px 5px; border-radius: 3px; }
        .endpoint { margin: 15px 0; padding: 10px; background: #f9f9f9; border-left: 4px solid #4CAF50; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌡️ ESP32 DHT Sensor API</h1>
        <p>This ESP32 is running the DHT sensor firmware and exposing sensor data via REST API.</p>
        
        <h2>Available Endpoints:</h2>
        
        <div class="endpoint">
            <strong>GET /sensor</strong>
            <p>Returns current sensor readings in JSON format</p>
            <code>[GET] http://192.168.1.100/sensor</code>
            <p>Response:</p>
            <code>{"temperature": 24.5, "humidity": 65.0, "rain_probability": 30.0, "timestamp": 123456}</code>
        </div>
        
        <div class="endpoint">
            <strong>GET /status</strong>
            <p>Returns ESP32 status information</p>
            <code>[GET] http://192.168.1.100/status</code>
        </div>
        
        <hr>
        <p><small>Connected to WiFi network showing this page. Configure your Rust web server to fetch from <code>/sensor</code> endpoint.</small></p>
    </div>
</body>
</html>
    )";
    
    server.send(200, "text/html", html);
}

void handleSensor() {
    // Read sensor values
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("❌ Failed to read from DHT sensor!");
        server.send(500, "application/json", "{\"error\": \"Sensor read failed\"}");
        return;
    }
    
    // Calculate rain probability based on humidity
    // Higher humidity = higher chance of rain
    float rainProbability = 0;
    if (humidity > 60) {
        rainProbability = (humidity - 60) * 6.67;  // 0-100%
        rainProbability = constrain(rainProbability, 0, 100);
    }
    
    // Create JSON response
    StaticJsonDocument<200> doc;
    doc["temperature"] = round(temperature * 10) / 10.0;  // Round to 1 decimal
    doc["humidity"] = round(humidity * 10) / 10.0;
    doc["rain_probability"] = round(rainProbability * 10) / 10.0;
    doc["timestamp"] = millis();
    
    // Serialize JSON
    String output;
    serializeJson(doc, output);
    
    // Send response
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", output);
    
    // Log to serial
    Serial.print("📊 Sensor data sent - Temp: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Rain Prob: ");
    Serial.print(rainProbability);
    Serial.println("%");
}

void handleStatus() {
    StaticJsonDocument<300> doc;
    doc["device"] = "ESP32 DHT Sensor";
    doc["firmware_version"] = "1.0.0";
    doc["wifi_ssid"] = WiFi.SSID();
    doc["ip_address"] = WiFi.localIP().toString();
    doc["signal_strength"] = WiFi.RSSI();
    doc["uptime_ms"] = millis();
    doc["dht_type"] = "DHT22";
    doc["dht_pin"] = DHTPIN;
    
    String output;
    serializeJson(doc, output);
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", output);
}

void handleNotFound() {
    String message = "404 Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    
    server.send(404, "text/plain", message);
}

/*
 * Troubleshooting:
 * 
 * 1. DHT sensor not reading:
 *    - Check GPIO pin number matches DHTPIN
 *    - Verify sensor is properly wired (3V3, GND, DATA)
 *    - Add 4.7k resistor between DATA and 3V3
 * 
 * 2. WiFi connection fails:
 *    - Check SSID and password are correct
 *    - Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
 *    - Check WiFi signal strength
 * 
 * 3. Cannot access /sensor endpoint:
 *    - Verify ESP32 IP from serial monitor
 *    - Check firewall allows port 80
 *    - Ping ESP32 IP to verify connection
 *    - Use browser to access http://[ESP32_IP]/sensor
 */

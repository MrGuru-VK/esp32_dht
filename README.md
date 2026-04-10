# 🌡️ DHT Sensor Weather Station Dashboard

A modern, attractive web-based weather station built with Rust (Actix-web backend) that displays real-time temperature, humidity, and rain probability data from an ESP32 DHT sensor.

## ✨ Features

- **Beautiful Responsive Dashboard** - Works on desktop, tablet, and mobile devices
- **Real-time Data Updates** - Automatically refreshes sensor data every 5 seconds
- **Interactive Gauges** - Temperature, humidity, and rain probability displays with smooth animations
- **Live Status Indicator** - Shows connection status to the sensor
- **Modern UI Design** - Dark theme with gradient accents and smooth transitions
- **No External Dependencies for Frontend** - Pure HTML, CSS, and JavaScript (no framework bloat)

## 📋 Project Structure

```
rust/web/
├── Cargo.toml              # Rust dependencies
├── src/
│   └── main.rs             # Rust backend server
└── static/
    ├── index.html          # Frontend webpage
    ├── style.css           # Styling
    └── script.js           # Data fetching and DOM updates
```

## 🚀 Getting Started

### Prerequisites

- Rust 1.70+ (install from https://rustup.rs/)
- ESP32 with DHT sensor (any DHT11, DHT22, or DHTXX model)

### Build & Run

```bash
# Build the project
cargo build --release

# Run the server
cargo run --release
```

The server will start at `http://localhost:8080`

**Output:**
```
🌡️ DHT Sensor Web Server starting...
📍 Server running at http://localhost:8080
```

Open your browser and navigate to `http://localhost:8080` to see the dashboard.

## 🔌 Connecting to ESP32

### Step 1: Set Up ESP32 with DHT Sensor

Use PlatformIO or Arduino IDE to flash this code to your ESP32:

```cpp
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// DHT Sensor Configuration
#define DHTPIN 4
#define DHTTYPE DHT22  // DHT22 (or DHT11/DHT21)
DHT dht(DHTPIN, DHTTYPE);

// WiFi Configuration
const char* ssid = "your_ssid";
const char* password = "your_password";

// Web Server
WebServer server(80);

void setup() {
    Serial.begin(115200);
    dht.begin();
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.localIP());
    
    // API endpoint
    server.on("/sensor", HTTP_GET, handleSensor);
    server.begin();
}

void loop() {
    server.handleClient();
}

void handleSensor() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Simple rain probability (can be enhanced with pressure/humidity analysis)
    float rainProbability = (humidity > 70) ? (humidity - 70) * 3.33 : 0;
    
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["rain_probability"] = rainProbability;
    doc["timestamp"] = millis();
    
    String output;
    serializeJson(doc, output);
    
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", output);
}
```

### Step 2: Configure Backend to Fetch from ESP32

Edit `src/main.rs` and locate the `fetch_sensor_data()` function. Replace the mock data with:

```rust
async fn fetch_sensor_data() -> SensorData {
    let client = reqwest::Client::new();
    let esp32_ip = "http://192.168.1.100"; // Replace with your ESP32's IP
    
    match client
        .get(format!("{}/sensor", esp32_ip))
        .timeout(Duration::from_secs(5))
        .send()
        .await
    {
        Ok(response) => {
            match response.json::<SensorData>().await {
                Ok(data) => data,
                Err(_) => get_mock_data()
            }
        }
        Err(_) => {
            eprintln!("⚠️ Failed to fetch from ESP32, using mock data");
            get_mock_data()
        }
    }
}

fn get_mock_data() -> SensorData {
    SensorData {
        temperature: 24.5,
        humidity: 65.0,
        rain_probability: 30.0,
        timestamp: Local::now().format("%H:%M:%S").to_string(),
    }
}
```

Also add to `main.rs`:

```rust
use std::time::Duration;
```

### Step 3: Update ESP32 IP

Find your ESP32's IP address from the serial monitor output and update it in the code above (replace `192.168.1.100`).

### Step 4: Rebuild and Run

```bash
cargo build --release
cargo run --release
```

## 📊 Dashboard Features

### Temperature Card 🌡️
- Displays current temperature in Celsius
- Color-coded gauge (blue → cold, green → cool, orange → warm, red → hot)
- Range indicator showing temperature zones

### Humidity Card 💧
- Displays current humidity percentage
- Interactive progress bar
- Status descriptions: Very Dry, Comfortable, Moderate, High

### Rain Probability Card 🌧️
- Shows estimated rain probability
- Animated weather icon
- Rotating probability wheel
- Descriptions from "Clear Sky" to "Rainstorm Warning"

### Connection Status 📡
- Green dot = Connected to ESP32
- Red dot = Connection failed (using mock data)
- Auto-refresh status updates

## 🎨 Customization

### Change Update Interval

Edit `static/script.js`:
```javascript
const UPDATE_INTERVAL = 5000; // milliseconds
```

### Modify Temperature Range

Edit `static/script.js` in `updateTemperature()`:
```javascript
const minTemp = -20;  // Minimum temperature
const maxTemp = 50;   // Maximum temperature
```

### Change Color Scheme

Edit `static/style.css` color variables:
```css
:root {
    --primary-color: #4da6ff;
    --secondary-color: #87ceeb;
    --accent-color: #ff6b6b;
    /* ... more colors ... */
}
```

## 🔧 Troubleshooting

### ESP32 Not Connecting
- Verify ESP32 is on the same network as the Rust server
- Check firewall allows connections to port 80 on ESP32
- Verify ESP32 IP address is correct in the code
- Add CORS headers if needed

### Dashboard Shows "Disconnected"
- Check browser console for errors (F12)
- Ensure API endpoint `/api/sensor` is accessible
- Verify ESP32 is responding with proper JSON format

### Port Already in Use
Change the port in `src/main.rs`:
```rust
.bind("0.0.0.0:3000")?  // Change 8080 to 3000 or another port
```

## 📱 Browser Support

- Chrome/Edge (latest)
- Firefox (latest)
- Safari (latest)
- Mobile browsers (iOS Safari, Chrome Mobile)

## 🔒 Security Notes

For production deployment:
1. Use HTTPS instead of HTTP
2. Implement authentication
3. Add rate limiting
4. Use environment variables for ESP32 IP
5. Add input validation
6. Consider CORS headers

## 📄 License

This project is free to use and modify.

## 🎯 Future Enhancements

- [ ] Historical data graphs and charts
- [ ] Weather forecast integration
- [ ] Alerts and notifications
- [ ] Data export (CSV/JSON)
- [ ] Multi-sensor support
- [ ] Dark/Light theme toggle
- [ ] Mobile app
- [ ] Cloud synchronization
- [ ] Machine learning for rain prediction

## 🤝 Support

For issues or questions, refer to the code comments or modify the mock data in `src/main.rs` to test without an actual ESP32.

---

**Happy monitoring!** 🌡️💧🌧️

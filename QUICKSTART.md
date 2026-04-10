# 🚀 Quick Start Guide

## 5-Minute Setup

### Option 1: Run with Mock Data (Testing)

Perfect for testing the dashboard without an ESP32:

```bash
# 1. Build the project
cargo build --release

# 2. Run the server
cargo run --release

# 3. Open browser
# Navigate to http://localhost:8080
```

✅ Dashboard will show simulated sensor data that updates every 5 seconds.

---

## Option 2: Connect Real ESP32

### Prerequisites
- ESP32 dev board
- DHT22/DHT11 sensor
- USB cable for ESP32 programming
- Arduino IDE or PlatformIO

### Step-by-Step

#### 1. Flash ESP32 with Sensor Firmware

**Using Arduino IDE:**

```
1. Tools → Board → Select "ESP32 Dev Module"
2. Tools → Port → Select your COM port
3. Sketch → Include Library → Manage Libraries
4. Search and install:
   - DHT sensor library by Adafruit
   - ArduinoJson by Benoit Blanchon
5. Copy code from ESP32_FIRMWARE.cpp
6. Update WiFi SSID and password in the code
7. Upload to ESP32
8. Monitor → Serial Monitor (115200 baud)
   - Note the IP address shown (e.g., 192.168.1.100)
```

**Using PlatformIO:**

```
1. Create new project with "esp32doit-devkit-v1" board
2. Install libraries in platformio.ini:
   [env:esp32doit-devkit-v1]
   lib_deps =
       DHT sensor library
       ArduinoJson
3. Copy code from ESP32_FIRMWARE.cpp to src/main.cpp
4. Update WiFi credentials
5. Upload and Monitor
```

#### 2. Configure Rust Backend

Edit `src/main.rs` and update the `fetch_sensor_data()` function:

**Find this section:**
```rust
async fn fetch_sensor_data() -> SensorData {
    // TODO: Replace with actual ESP32 API call
    // Example: let response = reqwest::Client::new()...
```

**Replace with:**
```rust
use std::time::Duration;

async fn fetch_sensor_data() -> SensorData {
    let client = reqwest::Client::new();
    let esp32_ip = "http://192.168.1.100";  // ← Change to your ESP32 IP
    
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
        Err(e) => {
            eprintln!("⚠️  Failed to fetch from ESP32: {}", e);
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

**Add to imports:**
```rust
use std::time::Duration;
```

#### 3. Rebuild and Run

```bash
cargo build --release
cargo run --release
```

#### 4. Test the Connection

Open browser: `http://localhost:8080`

**Expected Results:**
- ✅ Green connected indicator (if ESP32 is online)
- 📊 Sensor values updating every 5 seconds
- 🌡️ Temperature, humidity, rain probability displayed
- ⏰ Last updated timestamp changes

---

## 🔍 Troubleshooting

### Dashboard shows "Disconnected"

**Check ESP32 IP:**
```bash
# Use ping to verify ESP32 is reachable
ping 192.168.1.100
```

**Verify sensor API:**
```bash
# Use curl or browser to test endpoint
curl http://192.168.1.100/sensor
```

**Output should be:**
```json
{"temperature": 24.5, "humidity": 65.0, "rain_probability": 30.0, "timestamp": 123456}
```

### ESP32 won't connect to WiFi

1. Reboot ESP32 (press RST button)
2. Check WiFi SSID and password in code
3. Verify network is 2.4GHz (ESP32 doesn't support 5GHz)
4. Check serial monitor for errors

### DHT sensor not reading

1. Verify GPIO pin (default GPIO4):
   ```cpp
   #define DHTPIN 4  // Change if using different pin
   ```

2. Check wiring:
   - VCC → 3V3
   - GND → GND
   - DATA → GPIO4 (or your pin)

3. Add 4.7kΩ resistor between DATA and 3V3

4. Try with different DHT type:
   ```cpp
   #define DHTTYPE DHT11   // Change from DHT22 if needed
   ```

### Port 8080 already in use

Change port in `src/main.rs`:
```rust
.bind("0.0.0.0:3000")?  // Change 8080 to available port
```

---

## 📊 Understanding the Dashboard

| Component | Shows | Range |
|-----------|-------|-------|
| **Temperature** | Current temp with visual gauge | -20°C to 50°C |
| **Humidity** | Moisture level with status | 0% to 100% |
| **Rain Probability** | Estimated rain chance | 0% to 100% |
| **Status** | Connection to sensor | Connected/Disconnected |

---

## 🎨 Customization Tips

### Change refresh rate
Edit `static/script.js`:
```javascript
const UPDATE_INTERVAL = 5000;  // milliseconds
```

### Change temperature scale
Edit Celsius to Fahrenheit in `static/script.js`:
```javascript
// Replace:
tempValue.textContent = temp.toFixed(1) + '°C';

// With:
tempValue.textContent = ((temp * 9/5) + 32).toFixed(1) + '°F';
```

### Modify dashboard colors
Edit `static/style.css`:
```css
:root {
    --primary-color: #4da6ff;      /* Light blue */
    --secondary-color: #87ceeb;    /* Sky blue */
    --accent-color: #ff6b6b;       /* Red */
}
```

---

## 🌐 Network Setup

### Finding your PC's IP (for ESP32 to connect back)

**Windows:**
```powershell
ipconfig
# Look for IPv4 Address under WiFi adapter
```

**Linux/Mac:**
```bash
ifconfig | grep inet
```

### Setting Static IP for ESP32

In `ESP32_FIRMWARE.cpp` after WiFi connection:
```cpp
WiFi.config(
    IPAddress(192, 168, 1, 100),      // Static IP
    IPAddress(192, 168, 1, 1),        // Gateway
    IPAddress(255, 255, 255, 0)       // Subnet
);
```

---

## 🐛 Debug Mode

### Enable verbose logging

Edit `static/script.js`:
```javascript
// Already enabled! Check browser console (F12)
// You'll see:
// ✅ Data updated: {...}
// 📡 DHT Sensor Frontend Script Loaded
```

### Monitor API calls in browser

1. Press `F12` to open Developer Tools
2. Go to `Network` tab
3. Watch requests to `/api/sensor`
4. Expected: 200 OK, JSON response every 5 seconds

---

## 📚 Next Steps

- Add multiple sensor support
- Create historical data charts
- Set up alerts for temperature thresholds
- Export data to CSV
- Connect to cloud services
- Build mobile app

---

## 🆘 Still Need Help?

1. Check [README.md](README.md) for detailed documentation
2. Review serial monitor output on ESP32
3. Use browser DevTools (F12) to check network requests
4. Verify all IP addresses are on same network

**Happy monitoring!** 🌡️

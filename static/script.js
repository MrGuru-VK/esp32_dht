// Configuration
const API_URL = '/api/sensor';
const UPDATE_INTERVAL = 5000; // 5 seconds

// DOM Elements
const tempValue = document.getElementById('tempValue');
const tempGauge = document.getElementById('tempGauge');
const humidityValue = document.getElementById('humidityValue');
const humidityGauge = document.getElementById('humidityGauge');
const humidityBar = document.getElementById('humidityBar');
const humidityDesc = document.getElementById('humidityDesc');
const rainValue = document.getElementById('rainValue');
const rainDesc = document.getElementById('rainDesc');
const rainWheel = document.getElementById('rainWheel');
const lastUpdate = document.getElementById('lastUpdate');
const statusDot = document.getElementById('statusDot');
const statusText = document.getElementById('statusText');

/**
 * Update temperature display
 */
function updateTemperature(temp) {
    tempValue.textContent = temp.toFixed(1) + '°C';
    
    // Calculate gauge position (0-360 degrees, range: -20 to 50°C)
    const minTemp = -20;
    const maxTemp = 50;
    const percentage = Math.max(0, Math.min(100, ((temp - minTemp) / (maxTemp - minTemp)) * 100));
    const offset = 565.48 * (1 - percentage / 100);
    
    tempGauge.style.strokeDashoffset = offset;
    
    // Set color based on temperature
    let color = '#3498db'; // cold (blue)
    if (temp >= 5 && temp < 15) {
        color = '#52be80'; // cool (green)
    } else if (temp >= 15 && temp < 25) {
        color = '#f39c12'; // warm (orange)
    } else if (temp >= 25) {
        color = '#e74c3c'; // hot (red)
    }
    tempGauge.style.stroke = color;
}

/**
 * Update humidity display
 */
function updateHumidity(humidity) {
    humidityValue.textContent = humidity.toFixed(1) + '%';
    
    // Calculate gauge position
    const offset = 565.48 * (1 - humidity / 100);
    humidityGauge.style.strokeDashoffset = offset;
    humidityBar.style.width = humidity + '%';
    
    // Update humidity description
    let description = '';
    if (humidity < 30) {
        description = 'Very Dry';
        humidityDesc.style.color = '#ff6b6b';
    } else if (humidity < 50) {
        description = 'Comfortable';
        humidityDesc.style.color = '#ffa500';
    } else if (humidity < 70) {
        description = 'Moderate';
        humidityDesc.style.color = '#4da6ff';
    } else {
        description = 'High';
        humidityDesc.style.color = '#51cf66';
    }
    humidityDesc.textContent = description;
}

/**
 * Update rain probability display
 */
function updateRainProbability(rainProb) {
    rainValue.textContent = rainProb.toFixed(0) + '%';
    
    // Update wheel rotation based on probability
    const rotation = (rainProb / 100) * 360;
    rainWheel.style.transform = `rotate(${rotation}deg)`;
    
    // Update rain description
    let description = '';
    if (rainProb < 20) {
        description = 'Clear Sky';
        rainDesc.style.color = '#ffa500';
    } else if (rainProb < 40) {
        description = 'Partly Cloudy';
        rainDesc.style.color = '#b0b8c1';
    } else if (rainProb < 60) {
        description = 'Cloudy';
        rainDesc.style.color = '#87ceeb';
    } else if (rainProb < 80) {
        description = 'Rain Likely';
        rainDesc.style.color = '#4da6ff';
    } else {
        description = 'Rainstorm Warning';
        rainDesc.style.color = '#ff4757';
    }
    rainDesc.textContent = description;
}

/**
 * Fetch sensor data from API
 */
async function fetchSensorData() {
    try {
        const response = await fetch(API_URL);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        
        // Update all displays
        updateTemperature(data.temperature);
        updateHumidity(data.humidity);
        updateRainProbability(data.rain_probability);
        
        // Update timestamp
        const now = new Date();
        lastUpdate.textContent = now.toLocaleTimeString();
        
        // Update connection status
        setStatus(true);
        
        console.log(' Data updated:', data);
    } catch (error) {
        console.error(' Error fetching data:', error);
        setStatus(false);
        
        // Show error message
        statusText.textContent = 'Error connecting';
    }
}

/**
 * Update connection status indicator
 */
function setStatus(connected) {
    if (connected) {
        statusDot.classList.add('connected');
        statusText.textContent = 'Connected';
        statusText.style.color = '#51cf66';
    } else {
        statusDot.classList.remove('connected');
        statusText.textContent = 'Disconnected';
        statusText.style.color = '#ff6b6b';
    }
}

/**
 * Initialize the dashboard
 */
function initDashboard() {
    console.log('Initializing DHT Sensor Dashboard...');
    
    // Fetch data immediately
    fetchSensorData();
    
    // Set up auto-refresh
    setInterval(fetchSensorData, UPDATE_INTERVAL);
}

/**
 * Add SVG gradient for temperature gauge
 */
function addSVGGradient() {
    const svg = document.querySelector('.gauge');
    const defs = document.createElementNS('http://www.w3.org/2000/svg', 'defs');
    const gradient = document.createElementNS('http://www.w3.org/2000/svg', 'linearGradient');
    
    gradient.setAttribute('id', 'tempGradient');
    gradient.setAttribute('x1', '0%');
    gradient.setAttribute('y1', '0%');
    gradient.setAttribute('x2', '100%');
    gradient.setAttribute('y2', '100%');
    
    const stop1 = document.createElementNS('http://www.w3.org/2000/svg', 'stop');
    stop1.setAttribute('offset', '0%');
    stop1.setAttribute('stop-color', '#3498db');
    
    const stop2 = document.createElementNS('http://www.w3.org/2000/svg', 'stop');
    stop2.setAttribute('offset', '100%');
    stop2.setAttribute('stop-color', '#e74c3c');
    
    gradient.appendChild(stop1);
    gradient.appendChild(stop2);
    defs.appendChild(gradient);
    
    if (svg && !svg.querySelector('defs')) {
        svg.insertBefore(defs, svg.firstChild);
    }
}

/**
 * Simulate data updates for demo (remove in production)
 */
function simulateDataUpdates() {
    console.log(' Running in DEMO mode with simulated data');
    // This allows the dashboard to work without a real ESP32
    // In production, the API will fetch from the actual ESP32
}

// Start the dashboard when DOM is ready
document.addEventListener('DOMContentLoaded', function() {
    console.log(' DOM loaded, starting dashboard...');
    initDashboard();
    simulateDataUpdates();
});

// Optional: Add keyboard shortcut to manually refresh
document.addEventListener('keydown', function(e) {
    if (e.ctrlKey && e.key === 'r') {
        console.log(' Manual refresh triggered');
        fetchSensorData();
    }
});

// Log module loaded
console.log('📡 DHT Sensor Frontend Script Loaded');

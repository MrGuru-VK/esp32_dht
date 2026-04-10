use actix_web::{web, App, HttpServer, HttpResponse, Responder};
use serde::{Deserialize, Serialize};
use chrono::Local;

/// Sensor data structure
#[derive(Debug, Serialize, Deserialize, Clone)]
struct SensorData {
    temperature: f32,
    humidity: f32,
    rain_probability: f32,
    timestamp: String,
}

/// Mock sensor data - in production, this would fetch from ESP32
async fn fetch_sensor_data() -> SensorData {
    // TODO: Replace with actual ESP32 API call
    // Example: let response = reqwest::Client::new()
    //     .get("http://192.168.1.100/sensor")
    //     .send()
    //     .await
    //     .unwrap()
    //     .json::<SensorData>()
    //     .await
    //     .unwrap();

    SensorData {
        temperature: 24.5,
        humidity: 65.0,
        rain_probability: 30.0,
        timestamp: Local::now().format("%H:%M:%S").to_string(),
    }
}

/// API endpoint to get current sensor data
async fn get_sensor_data() -> impl Responder {
    let data = fetch_sensor_data().await;
    HttpResponse::Ok().json(data)
}

/// Serve the HTML frontend
async fn index() -> impl Responder {
    HttpResponse::Ok()
        .content_type("text/html; charset=utf-8")
        .body(include_str!("../static/index.html"))
}

/// Serve CSS
async fn style() -> impl Responder {
    HttpResponse::Ok()
        .content_type("text/css; charset=utf-8")
        .body(include_str!("../static/style.css"))
}

/// Serve JavaScript
async fn script() -> impl Responder {
    HttpResponse::Ok()
        .content_type("application/javascript; charset=utf-8")
        .body(include_str!("../static/script.js"))
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    println!("🌡️ DHT Sensor Web Server starting...");
    println!("📍 Server running at http://localhost:8080");

    HttpServer::new(|| {
        App::new()
            .route("/", web::get().to(index))
            .route("/style.css", web::get().to(style))
            .route("/script.js", web::get().to(script))
            .route("/api/sensor", web::get().to(get_sensor_data))
    })
    .bind("0.0.0.0:8080")?
    .run()
    .await
}

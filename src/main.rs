use actix_web::{web, App, HttpServer, HttpResponse, Responder};
use serde::{Deserialize, Serialize};
use chrono::Local;
#[derive(Debug, Serialize, Deserialize, Clone)]
struct SensorData {
    temperature: f32,
    humidity: f32,
    rain_probability: f32,
    timestamp: String,
}

async fn fetch_sensor_data() -> SensorData {
    // let response = request::Client::new()
    //     .get("http://192.168.1.100/sensor")
    //     .send()
    //     .await
    //     .unwrap()
    //     .json::<SensorData>()
    //     .await
    //     .unwrap();

    SensorData {
        temperature: 100.0,
        humidity: 0.0,
        rain_probability: 0.0,
        timestamp: Local::now().format("%H:%M:%S").to_string(),
    }
}

async fn get_sensor_data() -> impl Responder {
    let data = fetch_sensor_data().await;
    HttpResponse::Ok().json(data)
}
//html handler 
async fn index() -> impl Responder {
    HttpResponse::Ok()
        .content_type("text/html; charset=utf-8")
        .body(include_str!("../static/index.html"))
}

// css handler 
async fn style() -> impl Responder {
    HttpResponse::Ok()
        .content_type("text/css; charset=utf-8")
        .body(include_str!("../static/style.css"))
}

// JavaScript handler
async fn script() -> impl Responder {
    HttpResponse::Ok()
        .content_type("application/javascript; charset=utf-8")
        .body(include_str!("../static/script.js"))
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    println!(" DHT Sensor Web Server starting...");
    println!(" Server running at http://localhost:8080");

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

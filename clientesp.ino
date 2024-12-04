#include <WiFi.h>
#include <HTTPClient.h>

// Server IP and Wi-Fi credentials
const char* ssid = "ESP32";
const char* password = "00000001";
const char* serverIP = "192.168.4.1"; // Default IP of ESP32 AP
const float burdenResistor = 10.0;
const String clientID = "sensor2";

// Function to simulate sensor reading
float readCurrentSensor() {
  float current = analogRead(34) * (3.3 / 4096.0) / burdenResistor; 
  Serial.println(current);
  return current;
}

void setup() {
  Serial.begin(115200);

  // Connect to server Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Prepare data
    float sensorData = readCurrentSensor();
    String url = String("http://") + serverIP + "/update?clientID=" + clientID + "&sensorData=" + String(sensorData);

    // Send data to server
    http.begin(url);
    int httpResponseCode = http.POST("");
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Error sending data");
    }
    http.end();
  }
}

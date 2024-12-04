#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "ESP32";
const char* password = "00000001";
const float burdenResistor = 10.0;


WebServer server(80);

// Variables to store sensor data
float sensor1Data = 0.0;
float sensor2Data = 0.0;

// Function to simulate sensor reading
float readCurrentSensor() {
  float current = analogRead(34) * (3.3 / 4096.0) / burdenResistor;
  Serial.println(current);
  return current;
}



// Handle root page
void handleRoot() {
  String html = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Vertical Label</title>
    <style>
        .page-title {
            text-align: center;
            font-size: 24px;
            font-weight: bold;
            background-color: #4CAF50;
            color: white;
            padding: 10px;
            margin-bottom: 20px;
        }
        .plot-container {
            width: 80%;
            margin: 0 auto;
            padding: 20px;
            border: 2px solid #000;
            border-radius: 8px;
            background-color: #f9f9f9;
        }
        .row {
            display: flex;
            justify-content: center;
        }
        .point {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-top: 50px;
        }
        .point-label {
            writing-mode: vertical-rl;
            transform: rotate(180deg);
            margin-bottom: 10px;
            font-size: 16px;
            font-weight: bold;
        }
        .box {
            width: 100px;
            height: 100px;
            border: 1px solid #000;
            background-color: lightgray;
        }
        .bar {
            width: 200px;
            height: 50px;
            margin-bottom: 26px;
            align-self: end;
            transition: background-color 0.5s;
            display: flex;
            justify-content: center;
            align-items: center;
        }
    </style>
</head>
<body>
    <div class="page-title">Dashboard</div>
    <div class="plot-container">
        <div class="row">
            <div class="point">
                <div class="point-label">Sensor 1: <span id='sensor1'>0.0</span> A</div>
                <div class="box"></div>
            </div>
            <div class="bar" id="colorBar"></div>
            <div class="point">
                <div class="point-label">Sensor 2: <span id='sensor2'>0.0</span> A</div>
                <div class="box"></div>
            </div>
        </div>
    </div>

    <script>
        const MIN_VALUE = 0;
        const MAX_VALUE = 0.1;

        function getColor(value) {
            if (isNaN(value) || value < MIN_VALUE || value > MAX_VALUE) {
                return 'black';
            }
            const green = 255 - Math.round((value - MIN_VALUE) * (255/MAX_VALUE));
            const red = Math.round((value - MIN_VALUE) * (255/MAX_VALUE));
            return `rgb(${red}, ${green}, 0)`;
        }

        function updateSensorData() {
            fetch('/sensorData')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensor1').innerText = data.sensor1;
                    document.getElementById('sensor2').innerText = data.sensor2;
                    
                    // Calculate average of both sensors
                    diff = Math.abs(parseFloat(data.sensor1) - parseFloat(data.sensor2)).toFixed(2);
                    
                    // Update bar color based on difference
                    const color = getColor(diff);
                    document.getElementById('colorBar').style.backgroundColor = color;
                    document.getElementById('colorBar').innerText = `Diff = ${diff}`;
                });
        }
        
        setInterval(updateSensorData, 100);
    </script>
</body>
</html>)";
  server.send(200, "text/html", html);
}





// Endpoint to provide sensor data as JSON
void handleSensorData() {
  String json = "{";
  json += "\"sensor1\":\"" + String(sensor1Data, 2) + "\"";
  json += ",\"sensor2\":\"" + String(sensor2Data, 2) + "\"";
  json += "}";

  server.send(200, "application/json", json); //for debugging
}


// Update client data in the `handleClientData` function
void handleClientData() {
  if (server.hasArg("sensorData") && server.hasArg("clientID")) {
    String client = server.arg("clientID");
    float sensorData = server.arg("sensorData").toFloat();

    if (client == "sensor2") {
      sensor2Data = sensorData;
    }
    // else if (client == "sensor3") {
    //   sensor3Data = sensorData;
    // }

    Serial.print("Received data from ");
    Serial.print(client);
    Serial.print(": ");
    Serial.println(sensorData);
    
    server.send(200, "text/plain", "Data Received");
  } else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Start Wi-Fi Access Point
  WiFi.softAP(ssid, password);

  // Start Web Server
  server.on("/", handleRoot);
  server.on("/sensorData", HTTP_GET, handleSensorData);
  server.on("/update", HTTP_POST, handleClientData);    
  server.begin();

  Serial.println("Server started");
}


void loop() {
  // Read server sensor data
  sensor1Data = readCurrentSensor();

  // Handle incoming HTTP requests
  server.handleClient();
}

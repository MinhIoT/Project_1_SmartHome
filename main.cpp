#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DHT.h>
#include "led/config.h"
// Khai báo OLEN Monitor
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Khai báo tên WIFI
const char* ssid = "your ssid";
const char* password = "your password";
const char* ssidAP = "ESP32";
const char* passAP = "123456789";
// Tạo cổng kết nối server
WebServer server(80);
// Khai báo địa chỉ IP tĩnh
IPAddress local_IP(192, 168, 1, 184);    // Địa chỉ IP tĩnh 
IPAddress gateway(192, 168, 1, 1);       // Địa chỉ Gateway
IPAddress subnet(255, 255, 255, 0);      // Subnet Mask
IPAddress primaryDNS(1, 1, 1, 1);        // DNS Server 1
IPAddress secondaryDNS(1, 0, 0, 1);      // DNS Server 2 (optional)    
// MÃ HTML chính
const char MainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 WebServer</title>
    <style>
        body {
            text-align: center;
            font-family: 'Arial', sans-serif;
            font-size: 20px;
            margin: 0;
            padding: 0;
            background-color: #f0f0f0;
            color: #333;
        }
        .container {
            background-color: #fff;
            max-width: 960px;
            margin: 30px auto;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        .nav-tabs {
            display: flex;
            flex-wrap: wrap;
            list-style: none;
            margin: 0;
            padding: 0;
            border-bottom: 3px solid darkred;
            justify-content: center;
        }
        .tab-content {
            padding: 20px 0;
        }
        h1, h2 {
            color: #333;
            margin-bottom: 20px;
        }
        h1 {
            font-size: 32px;
        }
        h2 {
            font-size: 24px;
            color: #555;
        }
        .status {
            margin: 10px 0;
        }
        .status b {
            color: #007BFF;
        }
        .button-container {
            margin: 10px 0;
        }

        button {
            height: 50px;
            width: 100px;
            margin: 10px;
            background-color: #007BFF;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .button:hover {
            background-color: #0056b3;
        }
        .button:active {
            background-color: #004080;
            box-shadow: 0 3px #666;
            transform: translateY(2px);
        }
        .button_on {
            background-color: #28a745;
        }
        .button_on:active {
            background-color: #218838;
        }
        .button_off {
            background-color: #dc3545;
        }
        .button_off:active {
            background-color: #c82333;
        }
        .sensor-data {
            margin: 20px 0;
        }
        .sensor-data p {
            font-size: 18px;
            margin: 10px 0;
        }
        .camera-link {
            margin: 20px 0;
        }
        .camera-link a {
            text-decoration: none;
            color: #007BFF;
            font-weight: bold;
            font-size: 18px;
        }
        .camera-link a:hover {
            text-decoration: underline;
        }
        @media (max-width: 768px) {
            body {
                font-size: 18px;
            }
            .button {
                height: 40px;
                width: 80px;
                font-size: 16px;
            }
            h1 {
                font-size: 28px;
            }
            h2 {
                font-size: 22px;
            }
        }
        @media (max-width: 480px) {
            body {
                font-size: 16px;
            }
            .button {
                height: 35px;
                width: 70px;
                font-size: 14px;
            }
            h1 {
                font-size: 24px;
            }
            h2 {
                font-size: 20px;
            }
            .container {
                margin: 10px;
                padding: 15px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 WEBSERVER</h1>
        <h2>Smart Home Device Control</h2>
        <div class="tabs">
            <div class="nav-tabs">
                <button class="tablinks" onclick="openTab(event, 'Led')">Led Control</button>
                <button class="tablinks" onclick="openTab(event, 'Sensor')">Sensor Data</button>
                <button class="tablinks" onclick="openTab(event, 'Camera')">Camera Control</button>
            </div>
            <div class="tab-content">
                <div id="Led" class="tabcontents">
                    <h3>Led Control</h3>
                    <div class="status">State LED1: <b><span id="StateLED1"></span></b></div>
                    <div class="button-container">
                        <button class="button button_on" onclick="getdata('led1on')">ON</button>
                        <button class="button button_off" onclick="getdata('led1off')">OFF</button>
                    </div>
                    <div class="status">State LED2: <b><span id="StateLED2"></span></b></div>
                    <div class="button-container">
                        <button class="button button_on" onclick="getdata('led2on')">ON</button>
                        <button class="button button_off" onclick="getdata('led2off')">OFF</button>
                    </div>
                    <div class="status">State LED3: <b><span id="StateLED3"></span></b></div>
                    <div class="button-container">
                        <button class="button button_on" onclick="getdata('led3on')">ON</button>
                        <button class="button button_off" onclick="getdata('led3off')">OFF</button>
                    </div>
                </div>
                <div id="Sensor" class="tabcontents">
                    <h3>Sensor Data</h3>
                    <div class="sensor-data">
                        <p>Temperature: <b><span id="temperature"></span>°C</b></p>
                        <p>Humidity: <b><span id="humidity"></span>%</b></p>
                        <p>IR Sensor Detection: <b><span id="sensor"></span></b></p>
                        <p>Light Level: <b><span id="light"></span></b></p>
                    </div>
                </div>
                <div id="Camera" class="tabcontents">
                    <h3>Camera Control</h3>
                    <div class="camera-link">
                        <iframe src="http://192.168.1.120/" width="640" height="480"></iframe>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script>
        function openTab(evt, tabName){
            // khai báo các biến
            var i, tabcontents, tablinks;
            // Ẩn tất cả các nội dung
            tabcontents = document.getElementsByClassName("tabcontents");
            for(i = 0; i < tabcontents.length; i++){
                tabcontents[i].style.display = "none";
            }
            // xóa các class "active" tren tất cả các tab
            tablinks = document.getElementsByClassName("tablinks");
            for(i = 0; i < tablinks.length; i++){
                tablinks[i].className = tablinks[i].className.replace(" active", "");
            }
            // Hiển thị tab hiện tại, và thêm lớp "active" vào nút đã nhấp
            document.getElementById(tabName).style.display = "block";
            evt.currentTarget.className += " active";
        }
        document.getElementsByClassName("tablinks")[0].click();
    </script>
    <script>
        function create_obj() {
            var obj;
            if (navigator.appName == "Microsoft Internet Explorer") {
                obj = new ActiveXObject("Microsoft.XMLHTTP");
            } else {
                obj = new XMLHttpRequest();
            }
            return obj;
        }
        var xhttp = create_obj();
        
        function getdata(url) {
            xhttp.open("GET", "/" + url, true);
            xhttp.onreadystatechange = process;
            xhttp.send();
        }

        function process() {
            if (xhttp.readyState == 4 && xhttp.status == 200) {
                document.getElementById("container").innerHTML = xhttp.responseText;
            }
        }

        function getstatusL1L2L3() {
            xhttp.open("GET", "getstatusL1L2L3", true);
            xhttp.onreadystatechange = process_json;
            xhttp.send();
        }

        function process_json() {
            if (xhttp.readyState == 4 && xhttp.status == 200) {
                var Obj = JSON.parse(xhttp.responseText);
                document.getElementById("StateLED1").innerHTML = Obj.led1_pin;
                document.getElementById("StateLED2").innerHTML = Obj.led2_pin;
                document.getElementById("StateLED3").innerHTML = Obj.led3_pin;
            }
        }

        function fetchData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temperature').innerText = data.temperature;
                    document.getElementById('humidity').innerText = data.humidity;
                });
        }

        function fetchSensor() {
            fetch('/sensordata')
                .then(response => response.json())
                .then(sensordata => {
                    document.getElementById('sensor').innerText = sensordata.sensor;
                });
        }

        function fetchSensorLight() {
            fetch('readLight')
                .then(response => response.json())
                .then(readLight => {
                    document.getElementById('light').innerText = readLight.light;
                });
        }

        let intervalID1 = setInterval(function() {
            console.log('Task 1 running every 1 second');
            getstatusL1L2L3();
            fetchData();
            fetchSensorLight();
            fetchSensor();
        }, 1000);
    </script>
</body>
</html>
)rawliteral";
// sensor DHT11
#define DHTTYPE DHT11
#define DHTPIN 25
DHT dht(DHTPIN, DHTTYPE);
// khai bao chan led
uint8_t led1_pin = 4;
bool led1status = LOW;
uint8_t led2_pin = 0;
bool led2status = LOW;
uint8_t led3_pin = 2;
bool led3status = LOW;
// sensor light
#define analogPin 35
#define LED_SL 5
int Lux = 0;
// define sensor IR
#define IR_PIN 27
#define LED_IR 15
int sensorIR;
int count = 0;
// setup pins LED
void setupPins() {
    pinMode(led1_pin, OUTPUT);
    pinMode(led2_pin, OUTPUT);
    pinMode(led3_pin, OUTPUT);
    pinMode(LED_IR, OUTPUT);
    pinMode(IR_PIN, INPUT);
    pinMode(LED_SL, OUTPUT);
}
// khai báo chân đang tắt
void handleRoot() {
    led1status = LOW;
    led2status = LOW;
    led3status = LOW;
    server.send(200, "text/html", FPSTR(MainPage));
}
// Điều khiển LED ON-OFF
void handleLedOn(uint8_t ledPin, bool &ledStatus, const char* msg) {
    ledStatus = HIGH;
    digitalWrite(ledPin, HIGH);
    Serial.println(msg);
    server.send(200, "text/html", msg);
}
void handleLedOff(uint8_t ledPin, bool &ledStatus, const char* msg) {
    ledStatus = LOW;
    digitalWrite(ledPin, LOW);
    Serial.println(msg);
    server.send(200, "text/html", msg);
}
void handleNotFound() {
    server.send(404, "text/html", "NotFound");
}
void getStatusLeds() {
    String d1 = (digitalRead(led1_pin) == HIGH) ? "ON" : "OFF";
    String d2 = (digitalRead(led2_pin) == HIGH) ? "ON" : "OFF";
    String d3 = (digitalRead(led3_pin) == HIGH) ? "ON" : "OFF";
    String status = "{\"led1_pin\": \"" + d1 + "\",\"led2_pin\": \"" + d2 + "\",\"led3_pin\": \"" + d3 + "\"}";
    server.send(200, "application/json", status);
}
// Hàm đọc DHT11 cảm biến nhiệt độ, độ ẩm
void readDHT11() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor");
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println(F("DHT11 Sensor"));
    display.print(F("Temp: "));
    display.print(t);
    display.println(F(" C"));
    display.print(F("Humidity: "));
    display.print(h);
    display.println(F(" %"));
    display.display();
    String data = "{\"temperature\": " + String(t) + ",\"humidity\": " + String(h) + "}";
    server.send(200, "application/json", data);
}
// Hàm đọc cảm biến IR ứng dụng làm hệ thống phát hiện người tự động bật tắt đèn
void readSensorIR() {
    sensorIR = digitalRead(IR_PIN);
    String sensordata;
    if(sensorIR == 0){
        Serial.println("Cảm biến phát hiện người và bật đèn");
        digitalWrite(LED_IR, HIGH);
        sensordata = "{\"sensor\": \"Có\"}";
    }
    else
    {
        digitalWrite(LED_IR, LOW);
        sensordata = "{\"sensor\": \"Không\"}";
    }
    server.send(200, "application/json", sensordata);
}

// Hàm đọc cảm biến ánh sáng
void readSENSOR_LIGHT() {
  Lux = analogRead(analogPin); // doc gia tri analog 
  String readLight;
  
  if (Lux > 2000) {
    digitalWrite(LED_SL, HIGH);
    readLight = "{\"light\": \"Yếu\"}";
  } else {
    digitalWrite(LED_SL, LOW);
    readLight = "{\"light\": \"Mạnh\"}";
  }
  
  server.send(200, "application/json", readLight);

}
// Hàm đọc hình ảnh từ ESP32 cam lên Webserver cho esp32

// Hàm tạo kết nối STA và AP mode cho ESP32 webserver
void setupWifi_STAmode() {
  delay(10);
  Serial.println();
  // Thiết lập IP tĩnh cho ESP32
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Cấu hình IP tĩnh thất bại.");
  }
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void setupWifi_APmode() {
    WiFi.mode(WIFI_AP);
 // Khởi tạo chế độ Access Point với địa chỉ IP tĩnh
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Cấu hình AP thất bại!");
  }

  // Bắt đầu chế độ Access Point
  if (!WiFi.softAP(ssidAP, passAP)) {
    Serial.println("Khởi động AP thất bại!");
  } else {
    Serial.println("AP đã được khởi động");
  }

  // Hiển thị địa chỉ IP của Access Point
  Serial.print("AP MODE IP address: ");
  Serial.println(WiFi.softAPIP());
}
void setup() {
    Serial.begin(115200);
    dht.begin();
    //setupWifi_STAmode();
    setupWifi_APmode();
    setupPins(); // khai báo chân
    // Khởi tạo OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
    }
    display.clearDisplay();
    // Giao tiếp server
    server.on("/", handleRoot);
    server.on("/led1on", [](){ handleLedOn(led1_pin, led1status, "LED1: ON"); }); // Trạng thái LED1
    server.on("/led1off", [](){ handleLedOff(led1_pin, led1status, "LED1: OFF"); });
    server.on("/led2on", [](){ handleLedOn(led2_pin, led2status, "LED2: ON"); });// Trạng thái LED2
    server.on("/led2off", [](){ handleLedOff(led2_pin, led2status, "LED2: OFF"); });
    server.on("/led3on", [](){ handleLedOn(led3_pin, led3status, "LED3: ON"); });// Trạng thái LED3
    server.on("/led3off", [](){ handleLedOff(led3_pin, led3status, "LED3: OFF"); });
    server.on("/getstatusL1L2L3", getStatusLeds);// Điều khiển LED
    server.on("/data", readDHT11);// Đọc DHT11
    server.on("/sensordata", readSensorIR);// Đọc Cảm biến hồng ngoại
    server.on("/readLight", readSENSOR_LIGHT);// Đọc cảm biến ánh sáng
    server.onNotFound(handleNotFound);
    server.begin();
    
    Serial.println("Server is starting...");
}
void loop() {
    server.handleClient();
}

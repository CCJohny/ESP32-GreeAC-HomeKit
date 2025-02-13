#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Gree.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <DHT.h>
#include <WiFi.h>

AsyncWebServer server(80);

const char* ssid = "CCJ Home";
const char* password = "asdfghjkl";

#define kIrLed 4
IRGreeAC ac(kIrLed);

#define DHTPIN 16
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(DHTPIN, INPUT);
  dht.begin();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("连接到WiFi...");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleACControl);
  server.on("/api", HTTP_GET, handleGetTempHumidity);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/cool", HTTP_GET, handleCool);
  server.on("/heat", HTTP_GET, handleHeat);
  server.on("/dry", HTTP_GET, handleDry);
  server.on("/fan", HTTP_GET, handleFan);
  server.on("/auto", HTTP_GET, handleAuto);
  server.on("/speed/0", HTTP_GET, handleSpeed0);
  server.on("/speed/1", HTTP_GET, handleSpeed1);
  server.on("/speed/2", HTTP_GET, handleSpeed2);
  server.on("/speed/3", HTTP_GET, handleSpeed3);
  server.on("/temp", HTTP_GET, handleTemp);
  server.on("/swing/on", HTTP_GET, handleSwingOn);
  server.on("/swing/off", HTTP_GET, handleSwingOff);
  server.on("/sleep/on", HTTP_GET, handleSleepOn);
  server.on("/sleep/off", HTTP_GET, handleSleepOff);
  server.on("/light/on", HTTP_GET, handleLightOn);
  server.on("/light/off", HTTP_GET, handleLightOff);
  server.on("/turbo/on", HTTP_GET, handleTurboOn);
  server.on("/turbo/off", HTTP_GET, handleTurboOff);
  server.begin();
  
  ac.begin();
  ac.on();
  ac.setFan(1);
  ac.setMode(kGreeCool); // kGreeAuto, kGreeDry, kGreeCool, kGreeFan, kGreeHeat
  ac.setTemp(25);  // 16-30C
  ac.setSwingVertical(true, kGreeSwingAuto);
  ac.setXFan(false);
  ac.setLight(true);
  ac.setSleep(false);
  ac.setTurbo(false);

  printState();
}

void loop() {
}

void printState() {
  // Display the settings.
  Serial.printf("GREE A/C remote is in the following state: %s\n", ac.toString().c_str());
}

void handleACControl(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", R"rawliteral(
      <!DOCTYPE HTML>
      <html>
      <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1">
          <meta name="mobile-web-app-capable" content="yes">
          <title>格力空调控制</title>
          <style>
              body {font-family: Arial, sans-serif; margin: 0; padding: 0; box-sizing: border-box; max-width: 1500px; margin: auto;}
              .container {column-count: 1; column-gap: 20px; padding: 20px;}
              .card {background-color: #f7f7f7; border-radius: 15px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); margin: 10px 0; padding: 20px; display: inline-block; width: 100%; box-sizing: border-box; align-items: flex-start;}
              .capsule-button {border: none; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 5px 2px; cursor: pointer; border-radius: 50px;}
              .capsule-button.on-off {background-color: #4CAF50;}
              .capsule-button.speed {background-color: #FF5733;}
              .capsule-button.mode {background-color: #33C1FF;}
              .capsule-button.function {background-color: #FFC300;}
              .capsule-button.function.off {background-color: #FFF9E5; color: #333333;}  /* 使用浅黄潘通色 */
              .temp-slider {width: 100%; height: 40px;}
              @media screen and (max-width: 599px) {.container {column-count: 1; padding: 10px;} .card {margin: 5px 0; padding: 10px;} .capsule-button {padding: 8px 16px; font-size: 14px;} .temp-slider {height: 30px;}}
              @media screen and (min-width: 600px) and (max-width: 899px) {.container {column-count: 2;}}
              @media screen and (min-width: 900px) {.container {column-count: 3;}}
              html {font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center; color: #333333;}
              body {margin-top: 50px;}
              h1 {margin: 50px auto 30px;}
              .side-by-side {display: inline-block; vertical-align: middle; position: relative; white-space: nowrap;}
              .humidity-text, .temperature-text {font-weight: 400; font-size: 19px; width: 160px; text-align: center; white-space: nowrap;}
              .humidity, .temperature {font-weight: 300; font-size: 60px; text-align: center; white-space: nowrap;}
              .humidity {color: #3498db;}
              .temperature {color: #f39c12;}
              .data {padding: 10px;}
              .fixed-width {width: 100px; display: inline-block; text-align: center; white-space: nowrap;}
              .env-row {display: flex; justify-content: space-around; align-items: center;}
          </style>
          <script>
              function updateTemp(value) {
                  document.getElementById('tempValue').innerText = value + '°C';
                  fetch('/temp?value=' + value);
              }
              function updateData() {
                  fetch('/api')
                      .then(response => response.json())
                      .then(data => {
                          document.getElementById('temperature').innerText = data.Temperature.toFixed(1) + '°C';
                          document.getElementById('humidity').innerText = data.Humidity.toFixed(1) + '%';
                      }).catch(error => console.error('Error fetching data:', error));
              }
              function toggleFunction(button, functionName) {
                  var isActive = button.classList.contains('off');
                  button.classList.toggle('off', !isActive);
                  var url = isActive ? '/' + functionName + '/on' : '/' + functionName + '/off';
                  fetch(url);
              }
              setInterval(updateData, 2000);  // 每两秒刷新一次数据
          </script>
      </head>
      <body>
          <div class="container">
              <div class="card">
                  <h1>环境监测</h1>
                  <div class="env-row">
                      <div>
                          <div class="temperature-text">温度</div>
                          <div class="temperature" id="temperature">Null°C</div>
                      </div>
                      <div>
                          <div class="humidity-text">湿度</div>
                          <div class="humidity" id="humidity">Null%</div>
                      </div>
                  </div>
              </div>
              <div class="card">
                  <h1>格力空调控制</h1>
                  <button class="capsule-button on-off" onclick="fetch('/on')">开机</button>
                  <button class="capsule-button on-off" onclick="fetch('/off')">关机</button>
                  <h2>温度 <span id="tempValue">25°C</span></h2>
                  <input type="range" min="16" max="30" value="25" id="tempSlider" class="temp-slider" oninput="updateTemp(this.value)">
              </div>
              <div class="card">
                  <h2>风速</h2>
                  <button class="capsule-button speed" onclick="fetch('/speed/0')">自动</button>
                  <button class="capsule-button speed" onclick="fetch('/speed/1')">低</button>
                  <button class="capsule-button speed" onclick="fetch('/speed/2')">中</button>
                  <button class="capsule-button speed" onclick="fetch('/speed/3')">高</button>
              </div>
              <div class="card">
                  <h2>模式</h2>
                  <button class="capsule-button mode" onclick="fetch('/cool')">制冷</button>
                  <button class="capsule-button mode" onclick="fetch('/heat')">制热</button>
                  <button class="capsule-button mode" onclick="fetch('/dry')">除湿</button>
                  <button class="capsule-button mode" onclick="fetch('/fan')">风扇</button>
                  <button class="capsule-button mode" onclick="fetch('/auto')">自动</button>
              </div>
              <div class="card">
                  <h2>其他功能</h2>
                  <button class="capsule-button function off" onclick="toggleFunction(this, 'swing')">扫风</button>
                  <button class="capsule-button function off" onclick="toggleFunction(this, 'sleep')">睡眠</button>
                  <button class="capsule-button function off" onclick="toggleFunction(this, 'light')">数显</button>
                  <button class="capsule-button function off" onclick="toggleFunction(this, 'turbo')">强劲</button>
              </div>
          </div>
      </body>
      </html>
      )rawliteral");
}

void handleGetTempHumidity(AsyncWebServerRequest *request) {
    float Temperature = dht.readTemperature(); // 获取温度值
    float Humidity = dht.readHumidity(); // 获取湿度值
    String jsonResponse = "{\"Temperature\": " + String(Temperature) + ", \"Humidity\": " + String(Humidity) + "}";
    request->send(200, "application/json", jsonResponse);
}

void handleOn(AsyncWebServerRequest *request) {
    ac.on();
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "空调已开机");
}

void handleOff(AsyncWebServerRequest *request) {
    ac.off();
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "空调已关机");
}

void handleCool(AsyncWebServerRequest *request) {
    ac.on();
    ac.setMode(kGreeCool);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "已设置为制冷");
}

void handleHeat(AsyncWebServerRequest *request) {
    ac.on();
    ac.setMode(kGreeHeat);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "已设置为制热");
}

void handleDry(AsyncWebServerRequest *request) {
    ac.on();
    ac.setMode(kGreeDry);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "已设置为除湿");
}

void handleFan(AsyncWebServerRequest *request) {
    ac.on();
    ac.setMode(kGreeFan);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "已设置为风扇");
}

void handleAuto(AsyncWebServerRequest *request) {
    ac.on();
    ac.setMode(kGreeAuto);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "已设置为自动");
}

void handleSpeed0(AsyncWebServerRequest *request) {
    ac.setFan(0);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "风速已设置为自动");
}

void handleSpeed1(AsyncWebServerRequest *request) {
    ac.setFan(1);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "风速已设置为低");
}

void handleSpeed2(AsyncWebServerRequest *request) {
    ac.setFan(2);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "风速已设置为中");
}

void handleSpeed3(AsyncWebServerRequest *request) {
    ac.setFan(3);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "风速已设置为高");
}

void handleTemp(AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
        String tempValue = request->getParam("value")->value();
        int temp = tempValue.toInt();
        ac.setTemp(temp);
        ac.send();
        printState();
        request->send(200, "text/plain; charset=utf-8", "温度已设置为 " + String(temp) + "°C");
    } else {
        request->send(400, "text/plain; charset=utf-8", "缺少温度参数");
    }
}

void handleSwingOn(AsyncWebServerRequest *request) {
    ac.setSwingVertical(true, kGreeSwingAuto);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "扫风已开启");
}

void handleSwingOff(AsyncWebServerRequest *request) {
    ac.setSwingVertical(false, kGreeSwingAuto);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "扫风已关闭");
}

void handleSleepOn(AsyncWebServerRequest *request) {
    ac.setSleep(true);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "睡眠模式已开启");
}

void handleSleepOff(AsyncWebServerRequest *request) {
    ac.setSleep(false);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "睡眠模式已关闭");
}

void handleLightOn(AsyncWebServerRequest *request) {
    ac.setLight(true);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "数显已开启");
}

void handleLightOff(AsyncWebServerRequest *request) {
    ac.setLight(false);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "数显已关闭");
}

void handleTurboOn(AsyncWebServerRequest *request) {
    ac.setTurbo(true);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "强劲模式已开启");
}

void handleTurboOff(AsyncWebServerRequest *request) {
    ac.setTurbo(false);
    ac.send();
    printState();
    request->send(200, "text/plain; charset=utf-8", "强劲模式已关闭");
}

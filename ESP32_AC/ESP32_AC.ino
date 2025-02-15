#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Gree.h>
#include <WebServer.h>
#include <Wire.h>
#include <DHT.h>
#include <HomeSpan.h>
#include "AC.h"

WebServer server(80);

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

  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPortNum(6666);
  //homeSpan.enableOTA(false, true);
  homeSpan.setPairingCode("23336666");
  homeSpan.setWifiCallback(setupWeb);
  homeSpan.begin(Category::AirConditioners,"ESP32 HomeSpan GreeAC IR by CCJ");
  homeSpan.autoPoll();

  printState();

  //setupWeb();

  new SpanAccessory();

    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("CCJ");
      new Characteristic::SerialNumber("Gree");
      new Characteristic::Model("ESP32");
      new Characteristic::FirmwareRevision("Homespan");

    new AC_TEM();
    new AC_FAN();
    new AC_HUM();

}

void loop() {
  //homeSpan.poll();
  server.handleClient();
  delay(10);
}

void printState() {
  Serial.printf("GREE A/C remote is in the following state: %s\n", ac.toString().c_str());
}

void setupWeb() {
  server.begin();
  server.on("/", handleACControl);
  server.on("/api", handleGetTempHumidity);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/cool", handleCool);
  server.on("/heat", handleHeat);
  server.on("/dry", handleDry);
  server.on("/fan", handleFan);
  server.on("/auto", handleAuto);
  server.on("/speed/0", handleSpeed0);
  server.on("/speed/1", handleSpeed1);
  server.on("/speed/2", handleSpeed2);
  server.on("/speed/3", handleSpeed3);
  server.on("/temp", handleTemp);
  server.on("/swing/on", handleSwingOn);
  server.on("/swing/off", handleSwingOff);
  server.on("/sleep/on", handleSleepOn);
  server.on("/sleep/off", handleSleepOff);
  server.on("/light/on", handleLightOn);
  server.on("/light/off", handleLightOff);
  server.on("/turbo/on", handleTurboOn);
  server.on("/turbo/off", handleTurboOff);
}

void handleACControl() {
  server.send(200, "text/html", SendHTML());
}

void handleGetTempHumidity() {
  float Temperature = dht.readTemperature(); // 获取温度值
  float Humidity = dht.readHumidity(); // 获取湿度值
  String jsonResponse = "{\"Temperature\": " + String(Temperature) + ", \"Humidity\": " + String(Humidity) + "}";
  server.send(200, "application/json", jsonResponse);
}

void handleOn() {
  ac.on();
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "空调已开机");
}

void handleOff() {
  ac.off();
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "空调已关机");
}

void handleCool() {
  ac.on();
  ac.setMode(kGreeCool);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "已设置为制冷");
}

void handleHeat() {
  ac.on();
  ac.setMode(kGreeHeat);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "已设置为制热");
}

void handleDry() {
  ac.on();
  ac.setMode(kGreeDry);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "已设置为除湿");
}

void handleFan() {
  ac.on();
  ac.setMode(kGreeFan);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "已设置为风扇");
}

void handleAuto() {
  ac.on();
  ac.setMode(kGreeAuto);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "已设置为自动");
}

void handleSpeed0() {
  ac.setFan(0);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "风速已设置为自动");
}

void handleSpeed1() {
  ac.setFan(1);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "风速已设置为低");
}

void handleSpeed2() {
  ac.setFan(2);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "风速已设置为中");
}

void handleSpeed3() {
  ac.setFan(3);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "风速已设置为高");
}

void handleTemp() {
  if (server.hasArg("value")) {
    String tempValue = server.arg("value");
    int temp = tempValue.toInt();
    ac.setTemp(temp);
    ac.send();
    printState();
    server.send(200, "text/plain; charset=utf-8", "温度已设置为 " + String(temp) + "°C");
  } else {
    server.send(400, "text/plain; charset=utf-8", "缺少温度参数");
  }
}

void handleSwingOn() {
  ac.setSwingVertical(true, kGreeSwingAuto);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "扫风已开启");
}

void handleSwingOff() {
  ac.setSwingVertical(false, kGreeSwingAuto);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "扫风已关闭");
}

void handleSleepOn() {
  ac.setSleep(true);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "睡眠模式已开启");
}

void handleSleepOff() {
  ac.setSleep(false);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "睡眠模式已关闭");
}

void handleLightOn() {
  ac.setLight(true);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "数显已开启");
}

void handleLightOff() {
  ac.setLight(false);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "数显已关闭");
}

void handleTurboOn() {
  ac.setTurbo(true);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "强劲模式已开启");
}

void handleTurboOff() {
  ac.setTurbo(false);
  ac.send();
  printState();
  server.send(200, "text/plain; charset=utf-8", "强劲模式已关闭");
}

String SendHTML() {
    String ptr = "<!DOCTYPE HTML>";
    ptr += "<html>";
    ptr += "<head>";
    ptr += "<meta charset=\"UTF-8\">";
    ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    ptr += "<meta name=\"mobile-web-app-capable\" content=\"yes\">";
    ptr += "<title>格力空调控制</title>";
    ptr += "<style>";
    ptr += "body {font-family: Arial, sans-serif; margin: 0; padding: 0; box-sizing: border-box; max-width: 1500px; margin: auto;}";
    ptr += ".container {column-count: 1; column-gap: 20px; padding: 20px;}";
    ptr += ".card {background-color: #f7f7f7; border-radius: 15px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); margin: 10px 0; padding: 20px; display: inline-block; width: 100%; box-sizing: border-box; align-items: flex-start;}";
    ptr += ".capsule-button {border: none; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 5px 2px; cursor: pointer; border-radius: 50px;}";
    ptr += ".capsule-button.on-off {background-color: #4CAF50;}";
    ptr += ".capsule-button.speed {background-color: #FF5733;}";
    ptr += ".capsule-button.mode {background-color: #33C1FF;}";
    ptr += ".capsule-button.function {background-color: #FFC300;}";
    ptr += ".capsule-button.function.off {background-color: #FFF9E5; color: #333333;}";
    ptr += ".temp-slider {width: 100%; height: 40px;}";
    ptr += "@media screen and (max-width: 599px) {.container {column-count: 1; padding: 10px;} .card {margin: 5px 0; padding: 10px;} .capsule-button {padding: 8px 16px; font-size: 14px;} .temp-slider {height: 30px;}}";
    ptr += "@media screen and (min-width: 600px) and (max-width: 899px) {.container {column-count: 2;}}";
    ptr += "@media screen and (min-width: 900px) {.container {column-count: 3;}}";
    ptr += "html {font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center; color: #333333;}";
    ptr += "body {margin-top: 50px;}";
    ptr += "h1 {margin: 50px auto 30px;}";
    ptr += ".side-by-side {display: inline-block; vertical-align: middle; position: relative; white-space: nowrap;}";
    ptr += ".humidity-text, .temperature-text {font-weight: 400; font-size: 19px; width: 160px; text-align: center; white-space: nowrap;}";
    ptr += ".humidity, .temperature {font-weight: 300; font-size: 60px; text-align: center; white-space: nowrap;}";
    ptr += ".humidity {color: #3498db;}";
    ptr += ".temperature {color: #f39c12;}";
    ptr += ".data {padding: 10px;}";
    ptr += ".fixed-width {width: 100px; display: inline-block; text-align: center; white-space: nowrap;}";
    ptr += ".env-row {display: flex; justify-content: space-around; align-items: center;}";
    ptr += "</style>";
    ptr += "<script>";
    ptr += "function updateTemp(value) {";
    ptr += "document.getElementById('tempValue').innerText = value + '°C';";
    ptr += "fetch('/temp?value=' + value);";
    ptr += "}";
    ptr += "function updateData() {";
    ptr += "fetch('/api').then(response => response.json()).then(data => {";
    ptr += "document.getElementById('temperature').innerText = data.Temperature.toFixed(1) + '°C';";
    ptr += "document.getElementById('humidity').innerText = data.Humidity.toFixed(1) + '%';";
    ptr += "}).catch(error => console.error('Error fetching data:', error));";
    ptr += "}";
    ptr += "function toggleFunction(button, functionName) {";
    ptr += "var isActive = button.classList.contains('off');";
    ptr += "button.classList.toggle('off', !isActive);";
    ptr += "var url = isActive ? '/' + functionName + '/on' : '/' + functionName + '/off';";
    ptr += "fetch(url);";
    ptr += "}";
    ptr += "setInterval(updateData, 2000);";
    ptr += "</script>";
    ptr += "</head>";
    ptr += "<body>";
    ptr += "<div class=\"container\">";
    ptr += "<div class=\"card\">";
    ptr += "<h1>环境监测</h1>";
    ptr += "<div class=\"env-row\">";
    ptr += "<div>";
    ptr += "<div class=\"temperature-text\">温度</div>";
    ptr += "<div class=\"temperature\" id=\"temperature\">Null°C</div>";
    ptr += "</div>";
    ptr += "<div>";
    ptr += "<div class=\"humidity-text\">湿度</div>";
    ptr += "<div class=\"humidity\" id=\"humidity\">Null%</div>";
    ptr += "</div>";
    ptr += "</div>";
    ptr += "</div>";
    ptr += "<div class=\"card\">";
    ptr += "<h1>格力空调控制</h1>";
    ptr += "<button class=\"capsule-button on-off\" onclick=\"fetch('/on')\">开机</button>";
    ptr += "<button class=\"capsule-button on-off\" onclick=\"fetch('/off')\">关机</button>";
    ptr += "<h2>温度 <span id=\"tempValue\">25°C</span></h2>";
    ptr += "<input type=\"range\" min=\"16\" max=\"30\" value=\"25\" id=\"tempSlider\" class=\"temp-slider\" oninput=\"updateTemp(this.value)\">";
    ptr += "</div>";
    ptr += "<div class=\"card\">";
    ptr += "<h2>风速</h2>";
    ptr += "<button class=\"capsule-button speed\" onclick=\"fetch('/speed/0')\">自动</button>";
    ptr += "<button class=\"capsule-button speed\" onclick=\"fetch('/speed/1')\">低</button>";
    ptr += "<button class=\"capsule-button speed\" onclick=\"fetch('/speed/2')\">中</button>";
    ptr += "<button class=\"capsule-button speed\" onclick=\"fetch('/speed/3')\">高</button>";
    ptr += "</div>";
    ptr += "<div class=\"card\">";
    ptr += "<h2>模式</h2>";
    ptr += "<button class=\"capsule-button mode\" onclick=\"fetch('/cool')\">制冷</button>";
    ptr += "<button class=\"capsule-button mode\" onclick=\"fetch('/heat')\">制热</button>";
    ptr += "<button class=\"capsule-button mode\" onclick=\"fetch('/dry')\">除湿</button>";
    ptr += "<button class=\"capsule-button mode\" onclick=\"fetch('/fan')\">风扇</button>";
    ptr += "<button class=\"capsule-button mode\" onclick=\"fetch('/auto')\">自动</button>";
    ptr += "</div>";
    ptr += "<div class=\"card\">";
    ptr += "<h2>其他功能</h2>";
    ptr += "<button class=\"capsule-button function off\" onclick=\"toggleFunction(this, 'swing')\">扫风</button>";
    ptr += "<button class=\"capsule-button function off\" onclick=\"toggleFunction(this, 'sleep')\">睡眠</button>";
    ptr += "<button class=\"capsule-button function off\" onclick=\"toggleFunction(this, 'light')\">数显</button>";
    ptr += "<button class=\"capsule-button function off\" onclick=\"toggleFunction(this, 'turbo')\">强劲</button>";
    ptr += "</div>";
    ptr += "</div>";
    ptr += "</body>";
    ptr += "</html>";
    return ptr;
}
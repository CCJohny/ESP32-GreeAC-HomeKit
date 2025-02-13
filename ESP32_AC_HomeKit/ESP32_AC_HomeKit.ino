#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Gree.h>
#include <Wire.h>
#include <DHT.h>
#include <HomeSpan.h>
#include "config.h"
#include "AC.h"

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
  homeSpan.begin(Category::AirConditioners,"ESP32 HomeSpan GreeAC IR by CCJ");

  printState();

  new SpanAccessory();

    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("CCJ");
      new Characteristic::SerialNumber("Gree");
      new Characteristic::Model("ESP32");
      new Characteristic::FirmwareRevision("Homespan");

/*  new Service::Thermostat();
      new Characteristic::CurrentHeatingCoolingState(0);
      new Characteristic::TargetHeatingCoolingState(0);
      new Characteristic::CurrentTemperature(25);
      (new Characteristic::TargetTemperature(25))->setRange(16,30,1);*/

/*  new Service::Fan();
      new Characteristic::Active(1);
      new Characteristic::CurrentFanState(2);
      new Characteristic::TargetFanState(0);
      new Characteristic::RotationDirection(0);
      (new Characteristic::RotationSpeed(0))->setRange(0,3,1);
      new Characteristic::SwingMode(0);
      new Characteristic::LockPhysicalControls(0);*/

    new AC_TEM();
    new AC_FAN();
    new AC_HUM();

}

void loop() {
  homeSpan.poll();
}

void printState() {
  Serial.printf("GREE A/C remote is in the following state: %s\n", ac.toString().c_str());
}

extern IRGreeAC ac;
extern DHT dht;
void printState();

struct AC_TEM : Service::Thermostat {

  SpanCharacteristic *CHCS;
  SpanCharacteristic *THCS;
  SpanCharacteristic *CTEM;
  SpanCharacteristic *TTEM;

  AC_TEM() : Service::Thermostat(){

    CHCS = new Characteristic::CurrentHeatingCoolingState(0);
    THCS = new Characteristic::TargetHeatingCoolingState(0);
    CTEM = new Characteristic::CurrentTemperature(dht.readTemperature());
    TTEM = (new Characteristic::TargetTemperature(25))->setRange(16,30,1);
    
  }

  boolean update(){
    
    switch (THCS->getNewVal()) {
        case 0:
            ac.off();
            break;
        case 1:
            ac.on();
            ac.setMode(kGreeDry);
            break;
        case 2:
            ac.on();
            ac.setMode(kGreeCool);
            break;
        case 3:
            ac.on();
            ac.setMode(kGreeFan);
            break;
        default:
            ac.off();
            Serial.println("未知模式值");
            break;
    }

    ac.setTemp(TTEM->getNewVal());  // 16-30C

    ac.send();
    printState();

    return(true);
  
  }

  void loop(){

    if(CTEM->timeVal()>5000){                               // 5秒检测一次
      CTEM->setVal(dht.readTemperature());
    }
    
  }

};

struct AC_FAN : Service::Fan {

  SpanCharacteristic *ACP;
  SpanCharacteristic *CFS;
  SpanCharacteristic *TFS;
  SpanCharacteristic *RTD; //映射为睡眠模式
  SpanCharacteristic *RTS;
  SpanCharacteristic *SWM;
  SpanCharacteristic *LPC; //映射为数显开关

  AC_FAN() : Service::Fan(){

    ACP = new Characteristic::Active(1);
    CFS = new Characteristic::CurrentFanState(2);
    TFS = new Characteristic::TargetFanState(0);
    RTD = new Characteristic::RotationDirection(0);
    RTS = (new Characteristic::RotationSpeed(0))->setRange(0,3,1);
    SWM = new Characteristic::SwingMode(0);
    LPC = new Characteristic::LockPhysicalControls(0);
    
  }

  boolean update(){

    switch (ACP->getNewVal()) {
        case 0:
            ac.off();
            Serial.println("空调已关闭");
            break;
        case 1:
            ac.on();
            Serial.println("空调已打开");
            break;
        default:
            Serial.println("未知模式值");
            break;
    }
    
    ac.setFan(RTS->getNewVal());
    ac.setSwingVertical(SWM->getNewVal(), kGreeSwingAuto);
    ac.setLight(LPC->getNewVal());
    ac.setSleep(RTD->getNewVal());

    ac.send();
    printState();

    return(true);
  
  }

};

struct AC_HUM : Service::HumiditySensor {

  SpanCharacteristic *HMDT;

  AC_HUM() : Service::HumiditySensor(){

    HMDT = new Characteristic::CurrentRelativeHumidity(dht.readHumidity());
    
  }

  void loop(){

    if(HMDT->timeVal()>5000){                               // 5秒检测一次
      HMDT->setVal(dht.readHumidity());
    }
    
  };

};
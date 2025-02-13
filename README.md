# ESP32-GreeAC-HomeKit
基于ESP32实现HomeKit控制传统红外空调（以格力为例）。  

HomeKit controls traditional infrared air conditioners based on ESP32 (take Gree as an example).

## 项目概述 / Project Overview

本项目通过ESP32模块和Arduino编程，实现对传统红外控制空调的HomeKit控制，并通过DHT11模块进行实时温湿度监测。使用3mm/5mm红外发射管与三极管扩大红外遥控范围，从而增强控制效果。对于非苹果用户，还提供了网页控制版本，方便使用网页进行控制。  

This project uses an ESP32 module and Arduino programming to control a traditional infrared air conditioner via HomeKit, and to monitor real-time temperature and humidity using a DHT11 module. It uses 3mm/5mm infrared emission tubes and transistors to extend the range of infrared remote control, enhancing control effects. For non-Apple users, a web control version is also provided for convenient control via a web interface.

## 所需材料 / Required Materials

1. ESP32开发板 / ESP32 dev board
2. 3mm或5mm红外发射管 / 3mm or 5mm infrared emission tube
3. 三极管（用于放大电流） / Transistor (for current amplification)
4. DHT11温湿度传感器  / DHT11 temperature and humidity sensor
5. Arduino IDE
6. [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266)
7. [HomeSpan](https://github.com/HomeSpan/HomeSpan)
8. [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
9. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

> **注意**：请确保在Arduino IDE中安装的ESP32开发板管理器版本为<=2.0.17，因为更高版本与IRremoteESP8266库不兼容。因此，HomeSpan库应使用<=1.9.1版本。  
> **Note**: Ensure that the ESP32 board manager version in Arduino IDE is <= 2.0.17, as higher versions are incompatible with the IRremoteESP8266 library. Therefore, the HomeSpan library should use version <= 1.9.1.

## 项目步骤 / Project Steps

### 1. 准备工作 / Preparation
确保你已经安装了Arduino IDE，并配置好了ESP32开发环境。可以参考官方的ESP32安装指南进行设置。

Make sure you have installed Arduino IDE and configured the ESP32 development environment. You can refer to the official ESP32 installation guide for setup.

### 2. 电路连接 / Circuit Connection
- 将红外发射管与三极管连接，电路图如下 / Connect the infrared emission tube to the transistor, as shown in the circuit diagram below:
    ```
    红外发射管正极 -> 3.3V
    红外发射管负极 -> 三极管集电极（C）
    三极管发射极（E） -> GND
    三极管基极（B） -> ESP32 引脚D4
    ```
    ```
    Infrared emission tube positive -> 3.3V
    Infrared emission tube negative -> Transistor collector (C)
    Transistor emitter (E) -> GND
    Transistor base (B) -> ESP32 pin D4
    ```

- 将DHT11模块连接到ESP32，连接示例如下 / Connect the DHT11 module to the ESP32, as shown below:
    ```
    VCC -> 3.3V
    GND -> GND
    数据引脚 -> ESP32 引脚D16
    ```
    ```
    VCC -> 3.3V
    GND -> GND
    Data pin -> ESP32 pin D16
    ```

### 3. 程序选择 / Program Selection
选择以下Arduino程序 / Choose the following Arduino programs:

- **ESP32_AC_HomeKit** —— HomeKit控制版本 / HomeKit control version
- **ESP32_AC_Web** —— 网页控制版本 / Web control version

### 4. 上传代码并测试 / Upload Code and Test

#### 修改信息 / Modify Information
1. 在代码中的这部分 / In this part of the code:

    ```cpp
    const char* ssid = "CCJ Home";
    const char* password = "asdfghjkl";
    ```

    替换为你自己的Wi-Fi信息。  
    Replace with your own Wi-Fi information.

2. HomeKit控制版本在代码中的这部分 / In the HomeKit control version code:

    ```cpp
    homeSpan.setPairingCode("23336666");
    ```

    可修改配对码（8位纯数字）。  
    You can modify the pairing code (8-digit numeric).

将代码上传到ESP32开发板 / Upload the code to the ESP32 development board:

- **HomeKit控制版本**：打开Home应用程序，添加新的配件。你应该能够看到并控制你的空调，并实时监测温湿度。  
- **HomeKit control version**: Open the Home app, add new accessories. You should be able to see and control your air conditioner, and monitor temperature and humidity in real time.
- **网页控制版本**：在Arduino IDE的串口监视器中可以看到ESP32的IP地址，打开浏览器访问，使用网页进行控制。  
- **Web control version**: You can see the ESP32's IP address in the Arduino IDE serial monitor. Open a browser to access and use the web interface for control.

---

希望这个介绍对你有帮助。如果你有任何问题或者需要进一步的帮助，随时告诉我！😄  
I hope this introduction is helpful to you. If you have any questions or need further assistance, feel free to let me know! 😄


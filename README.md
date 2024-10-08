# Turn on your PC with Telegram and ESP32

![image](https://github.com/user-attachments/assets/77362d3b-ad12-4962-9442-120ff87dfed6)

## Features

* Don't need a static IP
* No need for Raspberry Pi or similar
* Wireless
* Low power consumption (less than 1 watt)
* Ability to add new devices via bot

## Libraries used:
1. [FastBot](https://github.com/GyverLibs/FastBot)
2. [Pairs](https://github.com/GyverLibs/Pairs)
3. [GyverBlinker](https://github.com/GyverLibs/GyverBlinker)
4. [WakeOnLan](https://github.com/a7md0/WakeOnLan)

## Setup

1. Make sure your target device supports Wake On Lan. You may need to enable it in the BIOS and/or your Network Card's Device Manager. You may also need to disable Windows Fast Startup
2. Create a Telegram bot using BotFather and get the API token
3. In the main file, change the parameters of your Wi-Fi network and bot token
   ```C
   #define WIFI_SSID "Your WiFi name"
   #define WIFI_PASS "Your WiFI password"
   #define BOT_TOKEN "Your Telegram bot token"
   ```
5. Сompile the code and upload the firmware to the microcontroller

## Usage

### /wol_list 
List devices
### /wol_add 
Add device. Usage: _/wol_add [Name] [MAC]_
### /wol_del 
Delete device. Usage: _/wol_del [ID]_
### /wol_on 
Turn on the device. Usage: _/wol_on [ID]_

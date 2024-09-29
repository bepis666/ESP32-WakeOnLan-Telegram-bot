#define WIFI_SSID "Your WiFi name"
#define WIFI_PASS "Your WiFI password"
#define BOT_TOKEN "Your Telegram bot token"
#define FORMAT_SPIFFS_IF_FAILED true

#include "WiFi.h"
#include "WiFiUdp.h"
#include "WakeOnLan.h"
#include "SPIFFS.h"
#include "FastBot.h"
#include "PairsFile.h"
#include "Blinker.h"

FastBot bot(BOT_TOKEN);
PairsFile macPairs(&SPIFFS, "/macPairs.dat", 10000);
WiFiUDP UDP;
WakeOnLan WOL(UDP);
Blinker led(2);

void setup() {

    // Opening the serial port for debugging
    Serial.begin(9600);
    
    // First connecting to WiFi
    led.blinkForever(100, 100);
    WiFi.setHostname("ESP32-WoL");
    WiFi.mode(WIFI_STA);
    wifi_connect();
  
    // Starting the file system
    SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);
    
    // Unloading the list of pairs from the file
    macPairs.begin(); 
    
    // Starting the telegram bot, connecting the message handler
    bot.attach(newMsg);

}

// Connecting to WiFI
void wifi_connect() {
  
    led.blinkForever(100, 100);
    Serial.println("WiFi disconnected...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    // Check the status. If there is no connection then display a message about the connection
    while (WiFi.status() != WL_CONNECTED) {
       Serial.println("Connecting to Wi-Fi...");
       led.tick();
    }
    
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    led.blinkForever(1000, 1000);
     
}

// Message handler
void newMsg(FB_msg& msg) {
  
    // Output of information and message text to the serial port
    Serial.println(msg.toString());
  
    // Output a list of devices
    if (msg.text.startsWith("/wol_list")) bot.sendMessage(wol_list(), msg.chatID);
  
    // Adding a device
    else if (msg.text.startsWith("/wol_add")) bot.sendMessage(wol_add(msg.text), msg.chatID);
  
    // Deleting a device
    else if (msg.text.startsWith("/wol_del")) bot.sendMessage(wol_del(msg.text), msg.chatID);
  
    // Turning on a device
    else if (msg.text.startsWith("/wol_on")) bot.sendMessage(wol_on(msg.text), msg.chatID);

}

// Output a list of devices
String wol_list() {
  
    String device_list;
    String string_macPairs;
    
    device_list = device_list + "Amount of devices: " + macPairs.amount() + "\nDevice list: \n";
    
    string_macPairs = macPairs;
    string_macPairs.replace("\":", " MAC: ");
    string_macPairs.replace("\"", "@. Device name: ");
    
    for (int i = 0; i < macPairs.amount(); i++) {
  
        string_macPairs.remove(0, 1);
        device_list = device_list + String(i + 1) + string_macPairs.substring(0, string_macPairs.indexOf("@"));
        string_macPairs.remove(0, string_macPairs.indexOf("@"));
    }
  
    return device_list;
}

// Adding a device
String wol_add(String msg_text) {

    // Parsing device name
    String device_name = msg_text;
    device_name.remove(0, 9);
    device_name.remove(device_name.indexOf(" "));

    // Parsing device MAC
    String device_mac = msg_text;
    device_mac.remove(0, 9);
    device_mac.remove(0, (device_mac.lastIndexOf(" ") + 1));

    // If the device name is already taken
    if (macPairs.contains(device_name)) return ("Device name already exists!");

    // Checking the MAC address format
    if (device_mac.length() != 17) return ("Wrong MAC-address format!");

    // If the MAC is already taken
    for (int i = 0; i < macPairs.amount(); i++) {
        if (macPairs.get(i) == device_mac) {
            return("Device MAC already exists!");
        }
    }

    // If everything is ok
    macPairs.set(device_name, device_mac);
    return("Device successfully added!\nDevice name: " + device_name + "\n" + "Device MAC: " + device_mac + "\nAmount of devices: " + macPairs.amount());
}

// Deleting a device
String wol_del(String msg_text) {
    
    // Parsing device ID
    msg_text.remove(0, 8);

    // Checking the device ID format
    if (msg_text.toInt() > 0) {
        if (not (msg_text.toInt() > macPairs.amount())){
            macPairs.remove(msg_text.toInt() - 1);
            return "Device removed successfully";
        }
        else return "Device doesn't exist!";
        
    }
    else return "Incorrect ID format!";
    
}

// Turning on a device
String wol_on(String msg_text) {
  
    // Parsing device ID
    msg_text.remove(0, 7);

    // Checking the device ID format
    if (msg_text.toInt() > 0) {
        if (not (msg_text.toInt() > macPairs.amount())){
            WOL.sendMagicPacket(macPairs.get(msg_text.toInt() - 1));
            return "Launch message sent";
        }
        else return "Device doesn't exist!";
        
    }
    else return "Incorrect ID format!";
}

void loop() {

    if (WiFi.status() != WL_CONNECTED) wifi_connect();  // Monitoring connection status
    bot.tick();  // Bot message handler
    macPairs.tick();  // Pair file handler
    led.tick(); // LED blinking

}

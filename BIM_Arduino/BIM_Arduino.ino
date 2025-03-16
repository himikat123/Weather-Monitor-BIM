/**
 *  Weather Monitor BIM v5.8
 *  https://github.com/himikat123/Weather-Monitor-BIM
 *
 *  © himikat123@gmail.com, Nürnberg, Deutschland, 2016-2025
 *
 *  Generic ESP8266 Module
 *  Flash Size: "4MB (FS:2MB OTA:~1019KB)"
 *
 *  Arduino IDE v1.8.19
 *  ESP8266 board v3.1.2
 */

/* Arduino libraries */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
WiFiClient wifiClient;
#include <ESP8266WebServer.h>
ESP8266WebServer webServer(80);
File fsUploadFile;
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
#include <Ticker.h>

Ticker clockPoints;

/* External Libraries */
#include <ArduinoJson.h> // v6.19.3 https://github.com/bblanchon/ArduinoJson
#include <TimeLib.h> // v1.6.0 https://playground.arduino.cc/Code/Time/
#include <ESP8266TrueRandom.h>

/* Own classes */
#include "globals.hpp"
Config config;
#include "sensors.hpp"
Sensors sensors;
#include "languages.hpp"
Lang lang;
#include "weather.hpp"
Weather weather;
#include "timentp.hpp"
TimeNTP timentp;
#include "thingspeak.hpp"
Thingspeak thingspeak;
#include "narodmon.hpp"
Narodmon narodmon;
#include "display.hpp"
Display display;
#include "network.hpp"
Network network;


void setup() {
  Serial.begin(74880);
  Serial.println("");
  Serial.println(SEPARATOR);
  Serial.println(SEPARATOR);
  Serial.println("*   Weather Monitor BIM " + String(global.fw) + "   © himikat123@gmail.com   2016-2025    *");
  Serial.println(SEPARATOR);
  Serial.println();

  pinMode(SETTINGS_BUTTON_PIN, INPUT);

  if(!LittleFS.begin()) {
    Serial.println("Flash FS initialisation failed!");
    while(1) yield();
  }

  config.readConfig();

  display.init();
  display.refresh(get_brightness());
  clockPoints.attach(0.5, clockPointsToggle);

  sensors.init();

  network.connect();
  
  webInterface_init();
}

void loop() {
  /**
   * Check if a button is pressed
   */
  // Enter access point mode if button is pressed
  if(digitalRead(SETTINGS_BUTTON_PIN) == 0) network.runAccessPoint();

  /**
   * Network connection if not connected
   */
  if(!network.isConnected()) {
    global.net_connected = false;
    if(!global.apMode) network.connect();
  }
  else global.net_connected = true;

  if(!global.apMode) {
    /**
     * Time synchronization with NTP server
     */
    if(config.clock_ntp_period()) {
      if((millis() - global.ntp_update) > config.clock_ntp_period() * 60000 or !global.clockSynchronized) {
        Serial.println(SEPARATOR);
        Serial.print("NTP synchronization... ");
        global.ntp_update = millis();
        if(network.isConnected()) timentp.get_time();
        else {
          global.ntp_update = 0;
          Serial.println("No internet connection");
        }
      }
    }

    /**
     * Receive data from thingspeak
     */
    if(config.thingspeakReceive_turnOn()) {
      if((millis() - global.thingspeakReceive) > (config.thingspeakReceive_period() * 60000) or global.thingspeakReceive == 0) {
        Serial.println(SEPARATOR);
        Serial.println("Receive data from thingspeak.com... ");
        global.thingspeakReceive = millis();
        if(network.isConnected()) thingspeak.receive();
        else {
          global.thingspeakReceive = 0;
          Serial.println("No internet connection");
        }
      }
    }

    /**
     * Send data to thingspeak
     */
    if(config.thingspeakSend_turnOn()) {
      if((millis() - global.thingspeakSend) > (config.thingspeakSend_period() * 60000) or global.thingspeakSend == 0) {
        Serial.println(SEPARATOR);
        Serial.println("Send data to thingspeak.com... ");
        global.thingspeakSend = millis();
        if(network.isConnected()) thingspeak.send();
        else {
          global.thingspeakSend = 0;
          Serial.println("No internet connection");
        }
      }
    }

    /**
     * Send data to narodmon
     */
    if(config.narodmonSend_turnOn()) {
      if((millis() - global.narodmonSend) > (config.narodmonSend_period() * 60000) or global.narodmonSend == 0) {
        Serial.println(SEPARATOR);
        Serial.println("Send data to narodmon... ");
        global.narodmonSend = millis();
        if(network.isConnected()) narodmon.send();
        else {
          global.narodmonSend = 0;
          Serial.println("No internet connection");
        }
      }
    }
    
    /**
     * Weather update 
     * every 20 minutes for open-meteo 
     * or every 60 minutes for weatherbit. 
     * If the last update was with an error, then after 5 minutes
     */
    uint32_t weatherUpd = config.weather_provider() == 0 ? 1200 : 3600;
    if(
      (now() - weather.get_currentUpdated() > weatherUpd && weather.get_errorUpdate() == 0) ||
      (weather.get_errorUpdate() && (now() - weather.get_errorUpdate() > 300))
    ) {
      Serial.println(SEPARATOR);
      Serial.println("Current weather update... ");
      weather.reset_errorUpdate();
      if(network.isConnected()) weather.update();
      else Serial.println("No internet connection");
    }
  }

  /**
   * Wired sensors update
   * every 5 seconds
   */
  if(millis() - global.sensors_update > 5000) {
    global.sensors_update = millis();
    sensors.read(); 
    
    comfortCalculate();
  }

  /**
   * List of available networks update
   * every 30 seconds
   */
  if(millis() - global.networks_update > 30000) {
    network.scan();
  }

  display.refresh(get_brightness());
  delay(10);

  webServer.handleClient();
}

void clockPointsToggle() {
  global.clockPoints = !global.clockPoints;
}

/**
 * Calculate comfort level 
 */
void comfortCalculate() {
  float temp = 40400.0;
  float hum = 40400.0;
  int tempLevel = 1;
  int humLevel = 1;
  bool heater = false;
  bool cooler = false;
  bool humidifier = false;
  bool dehumidifier = false;
  
  switch(config.comfort_temp_source()) {
    case 1: temp = weather.get_currentTemp(config.weather_temp_corr()); break;  // temperature from weather forecast
    case 2: {                                                                   // temperature from thingspeak
      if(now() - thingspeak.get_updated() < config.thingspeakReceive_expire() * 60)
        temp = thingspeak.get_field(config.comfort_temp_thing());
    }; break;
    case 3: temp = sensors.get_bme280_temp(config.bme280_temp_corr()); break;   // temperature from BME280
    case 4: temp = sensors.get_bmp180_temp(config.bmp180_temp_corr()); break;   // temperature from BMP180
    case 5: temp = sensors.get_sht21_temp(config.sht21_temp_corr()); break;     // temperature from SHT21
    case 6: temp = sensors.get_dht22_temp(config.dht22_temp_corr()); break;     // temperature from DHT22
    case 7: temp = sensors.get_ds18b20_temp(config.ds18b20_temp_corr()); break; // temperature from DS18B20
    default: ; break;
  }
  
  switch(config.comfort_hum_source()) {
    case 1: hum = weather.get_currentHum(config.weather_hum_corr()); break;     // humidity from weather forecast
    case 2: {                                                                   // humidity from thingspeak
      if(now() - thingspeak.get_updated() < config.thingspeakReceive_expire() * 60)
        hum = thingspeak.get_field(config.comfort_hum_thing());
    }; break;
    case 3: hum = sensors.get_bme280_hum(config.bme280_hum_corr()); break;      // humidity from BME280
    case 4: hum = sensors.get_sht21_hum(config.sht21_hum_corr()); break;        // humidity from SHT21
    case 5: hum = sensors.get_dht22_hum(config.dht22_hum_corr()); break;        // humidity from DHT22
    default: ; break;
  }

  if(sensors.checkTemp(temp)) {
    if(temp < config.comfort_temp_min()) heater = true;
    if(temp > config.comfort_temp_min()) heater = false;
    if(temp > config.comfort_temp_max()) cooler = true;
    if(temp < config.comfort_temp_max()) cooler = false;
    if(!heater and !cooler) tempLevel = 0;
    if(heater and !cooler) tempLevel = 2;
    if(!heater and cooler) tempLevel = 1;
  }
  else tempLevel = -1;

  if(sensors.checkHum(hum)) {
    if(hum < config.comfort_hum_min()) humidifier = true;
    if(hum > config.comfort_hum_min()) humidifier = false;
    if(hum > config.comfort_hum_max()) dehumidifier = true;
    if(hum < config.comfort_hum_max()) dehumidifier = false;
    if(!humidifier and !dehumidifier) humLevel = 0;
    if(humidifier and !dehumidifier) humLevel = 2;
    if(!humidifier and dehumidifier) humLevel = 1;
  }
  else humLevel = -1;

  global.comfort = 1;
  if(tempLevel == -1 && humLevel == -1) global.comfort = 0;
  if(tempLevel == 1 && humLevel < 1) global.comfort = 2;
  if(tempLevel == 2 && humLevel < 1) global.comfort = 3;
  if(tempLevel < 1 && humLevel == 1) global.comfort = 5;
  if(tempLevel < 1 && humLevel == 2) global.comfort = 4;
  if(tempLevel == 1 && humLevel == 1) global.comfort = 6;
  if(tempLevel == 1 && humLevel == 2) global.comfort = 7;
  if(tempLevel == 2 && humLevel == 1) global.comfort = 8;
  if(tempLevel == 2 && humLevel == 2) global.comfort = 9;
}

/**
 * Display brightness calculation
 */
unsigned int get_brightness() {
  #define HOUR 0
  #define MINUTE 1

  switch(config.display_brightMethod()) {

    /* By sunrise and sunset */
    case 0:
      return(weather.get_isDay() ? config.display_brightness_day() : config.display_brightness_night());

    /* By ambeint light sensor */
    case 1: {
      float bright = 0.0;
      if(config.display_lightSensor() == 0) {
        bright = sensors.checkVolt(sensors.get_analog_voltage(config.analog_voltage_corr())) ? sensors.get_analog_voltage(config.analog_voltage_corr()) * 30 : 50;
      }
      if(config.display_lightSensor() == 1) {
        bright = sensors.checkLight(sensors.get_max44009_light(config.max44009_light_corr())) ? sensors.get_max44009_light(config.max44009_light_corr()) : 50;
      }
      if(config.display_lightSensor() == 2) {
        bright = sensors.checkLight(sensors.get_bh1750_light(config.bh1750_light_corr())) ? sensors.get_bh1750_light(config.bh1750_light_corr()) : 50;
      }
      bright *= (float)config.display_lightSensor_sensitivity() / 20.0;
      if(bright < 1.0) bright = 1.0;
      if(bright > 100.0) bright = 100.0;
      return(round(bright));
    };

    /* By user defined time */
    case 2: {
      unsigned int morning = config.display_dayTime(HOUR) * 60 + config.display_dayTime(MINUTE);
      unsigned int evening = config.display_nightTime(HOUR) * 60 + config.display_nightTime(MINUTE);
      unsigned int cur_time = int(hour()) * 60 + minute();
      unsigned int bright = (cur_time >= morning and cur_time < evening) ? 
        config.display_brightness_day() : config.display_brightness_night();
      return(bright);
    };

    /* Constant brightness */
    case 3:
      return(config.display_brightness_day());

    default:
      return 50; break;
  }
}

/**
 *  Weather Monitor BIM v5.0
 *  https://github.com/himikat123/Weather-Monitor-BIM

 *  © himikat123@gmail.com, Nürnberg, Deutschland, 2016-2023

 *  Generic ESP8266 Module
 *  Flash Size: "4MB (FS:2MB OTA:~1019KB)"

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
//#include <ESPAsyncTCP.h> // v1.2.4 https://github.com/dvarrel/ESPAsyncTCP
//#include <ESPAsyncWebServer.h> // v1.2.3 https://github.com/me-no-dev/ESPAsyncWebServer
//AsyncWebServer server(80);

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
  Serial.println("*   Weather Monitor BIM " + String(global.fw) + "   © himikat123@gmail.com   2016-2023    *");
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

  if(!global.apMode) {
    /**
     * Network connection if not connected
     */
    if(!network.isConnected()) {
      global.net_connected = false;
      network.connect();
    }
    else global.net_connected = true;
    
    /**
     * Time synchronization with NTP server
     */
    if(config.clock_ntp_period()) {
      if((millis() - global.ntp_update) > config.clock_ntp_period() * 60000 or !global.clockSynchronized) {
        Serial.println(SEPARATOR);
        Serial.print("NTP synchronization... ");
        global.ntp_update = millis();
        if(network.isConnected()) timentp.get_time();
        else Serial.println("No internet connection");
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
        else Serial.println("No internet connection");
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
        else Serial.println("No internet connection");
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
        else Serial.println("No internet connection");
      }
    }
    
    /**
     * Weather update every 20 minutes
     * if last update was with an error then after 5 minutes
     */
    if((now() - weather.get_currentUpdated() > 1200 and weather.get_errorUpdate() == 0) or (weather.get_errorUpdate() and (now() - weather.get_errorUpdate() > 300))) {
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
  int temp = 40400;
  int hum = 40400;
  
  switch(config.comfort_temp_source()) {
    case 0: temp = config.comfort_temp_min(); break;                                   // temperature sensor excluded
    case 1: temp = round(weather.get_currentTemp()); break;                            // temperature from weather forecast
    case 2: {                                                                          // temperature from thingspeak
      if(now() - thingspeak.get_updated() < config.thingspeakReceive_expire() * 60)
        temp = round(thingspeak.get_field(config.comfort_temp_thing()));
    }; break;
    case 3: temp = round(sensors.get_bme280_temp(config.bme280_temp_corr())); break;   // temperature from BME280
    case 4: temp = round(sensors.get_bmp180_temp(config.bmp180_temp_corr())); break;   // temperature from BMP180
    case 5: temp = round(sensors.get_sht21_temp(config.sht21_temp_corr())); break;     // temperature from SHT21
    case 6: temp = round(sensors.get_dht22_temp(config.dht22_temp_corr())); break;     // temperature from DHT22
    case 7: temp = round(sensors.get_ds18b20_temp(config.ds18b20_temp_corr())); break; // temperature from DS18B20
    default: temp = 40400; break;
  }
  
  switch(config.comfort_hum_source()) {
    case 0: hum = config.comfort_hum_min(); break;                                // humidity sensor excluded
    case 1: hum = round(weather.get_currentHum()); break;                         // humidity from weather forecast
    case 2: {                                                                     // humidity from thingspeak
      if(now() - thingspeak.get_updated() < config.thingspeakReceive_expire() * 60)
        hum = round(thingspeak.get_field(config.comfort_hum_thing()));
    }; break;
    case 3: hum = round(sensors.get_bme280_hum(config.bme280_hum_corr())); break; // humidity from BME280
    case 4: hum = round(sensors.get_sht21_hum(config.sht21_hum_corr())); break;   // humidity from SHT21
    case 5: hum = round(sensors.get_dht22_hum(config.dht22_hum_corr())); break;   // humidity from DHT22
    default: temp = 40400; break;
  }
  
  if(sensors.checkTemp(temp) and sensors.checkHum(hum)) {
    if(temp >= config.comfort_temp_min() and temp <= config.comfort_temp_max() and hum >= config.comfort_hum_min() and hum <= config.comfort_hum_max()) {
      global.comfort = 1;
    }
    if(temp > config.comfort_temp_max() and hum >= config.comfort_hum_min() and hum <= config.comfort_hum_max()) {
      global.comfort = 2;
    }
    if(temp < config.comfort_temp_min() and hum >= config.comfort_hum_min() and hum <= config.comfort_hum_max()) {
      global.comfort = 3;
    }
    if(temp >= config.comfort_temp_min() and temp <= config.comfort_temp_max() and hum < config.comfort_hum_min()) {
      global.comfort = 4;
    }
    if(temp >= config.comfort_temp_min() and temp <= config.comfort_temp_max() and hum > config.comfort_hum_max()) {
      global.comfort = 5;
    }
    if(temp > config.comfort_temp_max() and hum > config.comfort_hum_max()) {
      global.comfort = 6;
    }
    if(temp > config.comfort_temp_max() and hum < config.comfort_hum_min()) {
      global.comfort = 7;
    }
    if(temp < config.comfort_temp_min() and hum > config.comfort_hum_max()) {
      global.comfort = 8;
    }
    if(temp < config.comfort_temp_min() and hum < config.comfort_hum_min()) {
      global.comfort = 9;
    }
  }
  else {
    global.comfort = 0;
  }
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

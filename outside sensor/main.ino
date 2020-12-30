/* Wireless Sensor BIM (esp part) v3.0
   © himikat123@gmail.com, Nürnberg, Deutschland, 2020
*/
                               // Board: Generic ESP8266 Module 
                               // 1MB (256kB SPIFFS)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Hash.h>
#include <Arduino.h>
#include <pgmspace.h>
#include <ESP8266TrueRandom.h>
#include "ThingSpeak.h"
#include <PubSubClient.h>
#include <Ticker.h>
#include "BlueDot_BME280.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include "SHT21.h"
#include "DHTesp.h"
#include <BH1750.h>
#include <MAX44009.h>

extern "C"{
  #include "main.h"
  #include "languages.h"
}

#define BUTTON          0
#define R              12
#define G              13
#define B              14
#define ONE_WIRE_BUS    2
#define DHTPIN          0
                     
ESP8266WebServer webServer(80);
DNSServer dnsServer;
File fsUploadFile;
Ticker updater;
Ticker sender;
WiFiClient  client;
PubSubClient mqtt(client);
BlueDot_BME280 bme1;
BlueDot_BME280 bme2;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds(&oneWire);
DeviceAddress thermometer1, thermometer2, thermometer3;
Adafruit_BMP085 bmp;
SHT21 SHT21;
DHTesp dht;
BH1750 lightMeter(0x23);
MAX44009 max_light;

void setup(){
    //PINs
  pinMode(BUTTON, INPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  led(1, 1, 1);
    //Serial port
  Serial.begin(74880);
  while(!Serial);
    //SPIFS
  if(!SPIFFS.begin()) while(1) yield();
    //EEPROM
  read_eeprom();
    //sensors
  sensors_init();
    //WIFI MODE
  WiFi.mode(WIFI_STA);
    //WiFi
  connectToWiFi();
  web_settings();
    //mDNS
  config.mdns.toCharArray(text_buf, (config.mdns.length()) + 1);
  MDNS.begin(text_buf);
  MDNS.addService("http", "tcp", 80);
    //ThingSpeak
  ThingSpeak.begin(client);
    //MQTT
  mqtt.setServer(config.mqtt_server, config.mqtt_port);
  mqtt.setCallback(mqtt_incomming);
    // Ticker
  sender.attach(config.period * 60, snd);
}

void mqtt_incomming(char* topic, byte* payload, unsigned int length){}

void snd(){
  sending_flag = true;
}

void sensors_sleep(void){
  if(sensors.bmp180Detected) bmp.begin(BMP085_ULTRALOWPOWER);
  if(sensors.bme1Detected or sensors.bmp1Detected){
    bme1.parameter.sensorMode = 0b00;
    bme1.init();
  }
  if(sensors.bme2Detected or sensors.bmp2Detected){
    bme2.parameter.sensorMode = 0b00;
    bme2.init();
  }
}

void loop(){
  if(sending_flag){
    sending_flag = false;
    Serial.println("sending");
    sending();
    if(!config.ac){
      sensors_sleep();
      ESP.deepSleep(config.period * 60000000);
    }
  }
  is_settings();
  webServer.handleClient();
  yield();
}

void sending(){
  BatteryLevel();
  sensors.temp_out = get_temp(0);
  if(sensors.temp_out != 404.0){
    if(config.t_unt == 0) sensors.temp_out += config.t_cor;
    else sensors.temp_out = sensors.temp_out * 1.8 + 32 + config.t_cor;
  }
  sensors.temp_in = get_temp(1);
  if(sensors.temp_in != 404.0){
    if(config.ti_unt == 0) sensors.temp_in += config.ti_cor;
    else sensors.temp_in = sensors.temp_in * 1.8 + 32 + config.ti_cor;
  }
  sensors.temp_extra = get_temp(2);
  if(sensors.temp_extra != 404.0){
    if(config.te_unt == 0) sensors.temp_extra += config.te_cor;
    else sensors.temp_extra = sensors.temp_extra * 1.8 + 32 + config.te_cor;
  }
  sensors.pres_out = get_pres(0);
  if(sensors.pres_out != 4040.0){
    if(config.p_unt == 0) sensors.pres_out += config.p_cor;
    else sensors.pres_out = sensors.pres_out * 0.75 + config.p_cor;
  }
  sensors.pres_in = get_pres(1);
  if(sensors.pres_in != 4040.0){
    if(config.pi_unt == 0) sensors.pres_in += config.pi_cor;
    else sensors.pres_in = sensors.pres_in * 0.75 + config.pi_cor;
  }
  sensors.hum_out = get_hum(0);
  if(sensors.hum_out != 404.0){
    sensors.hum_out += config.h_cor;
  }
  sensors.hum_in = get_hum(1);
  if(sensors.hum_in != 404.0){
     sensors.hum_in += config.hi_cor;
  }
  sensors.light = get_light();
  if(sensors.light >= 0.0){
    sensors.light += config.l_cor;
  }
  if(WiFi.status() == WL_CONNECTED){
    led(0, 1, 1);
    if(config.narod) sendToNarodmon();
    if(config.thingspeak) sendToThingSpeak();
    if(config.mqtt){
      if(!mqtt.connected()){
        if(mqtt.connect("Sensor", config.mqtt_login, config.mqtt_pass)){
          mqtt.setCallback(mqtt_incomming);
        }
        else{
          Serial.println("Could not connect to MQTT server");   
        }
      }
      if(mqtt.connected()){
        mqtt.loop();
        sendToMQTT();
      }
    }
  }
  else{
    Serial.print("network "); Serial.println(" was not found");
    is_settings();
    Serial.println("going sleep");
    led(0, 0, 0);
    sensors_sleep();
    ESP.deepSleep(300000000);
  }
}

void sendToNarodmon(){
  WiFiClient client;
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String buf = "#BIM" + mac + "#BIM";
  buf += "#" + config.latitude;
  buf += "#" + config.longitude;
  if(config.temp[0] > 0 and sensors.temp_out > -55 and sensors.temp_out < 120){
    buf += "\n#T" + mac + "#";
    buf += String(sensors.temp_out);
  }
  if(config.hum[0] > 0 and sensors.hum_out > -1 and sensors.hum_out < 101){
    buf += "\n#H" + mac + "#";
    buf += String(sensors.hum_out);
  }
  if(config.pres[0] > 0 and sensors.pres_out > 500 and sensors.pres_out < 1500){
    buf += "\n#P" + mac + "#";
    buf += String(sensors.pres_out);
  }
  if(config.temp[1] > 0 and sensors.temp_in > -55 and sensors.temp_in < 120){
    buf += "\n#TI" + mac + "#";
    buf += String(sensors.temp_in);
  }
  if(config.hum[1] > 0 and sensors.hum_in > -1 and sensors.hum_in < 101){
    buf += "\n#HI" + mac + "#";
    buf += String(sensors.hum_in);
  }
  if(config.pres[1] > 0 and sensors.pres_in > 500 and sensors.pres_in < 1500){
    buf += "\n#PI" + mac + "#";
    buf += String(sensors.pres_in);
  }
  if(config.temp[2] > 0 and sensors.temp_extra > -55 and sensors.temp_extra < 120){
    buf += "\n#TE" + mac + "#";
    buf += String(sensors.temp_extra);
  }
  if(config.light > 0 and sensors.light >= 0.0 and sensors.light < 1000000){
    buf += "\n#L" + mac + "#";
    buf += String(sensors.light);
  }
  buf += "\n#B" + mac + "#";
  buf += String(sensors.uBat);
  buf += "\n#BL" + mac + "#";
  buf += String(sensors.bat_level);
  buf += "\n##\r\n";
  if(!client.connect("narodmon.ru", 8283)) Serial.println("send to narodmon failed");
  else client.print(buf);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println(buf);
  Serial.println();
}

void sendToThingSpeak(){
  float dat[11];
  dat[0] = 0.0;
  dat[1] = sensors.temp_out;
  dat[2] = sensors.temp_in;
  dat[3] = sensors.temp_extra;
  dat[4] = sensors.hum_out;
  dat[5] = sensors.hum_in;
  dat[6] = sensors.pres_out;
  dat[7] = sensors.pres_in;
  dat[8] = sensors.light;
  dat[9] = sensors.uBat;
  dat[10] = sensors.bat_level;
  if(config.f1) ThingSpeak.setField(1, dat[config.f1]);
  if(config.f2) ThingSpeak.setField(2, dat[config.f2]);
  if(config.f3) ThingSpeak.setField(3, dat[config.f3]);
  if(config.f4) ThingSpeak.setField(4, dat[config.f4]);
  if(config.f5) ThingSpeak.setField(5, dat[config.f5]);
  if(config.f6) ThingSpeak.setField(6, dat[config.f6]);
  if(config.f7) ThingSpeak.setField(7, dat[config.f7]);
  if(config.f8) ThingSpeak.setField(8, dat[config.f8]);
  ThingSpeak.setStatus("myStatus");
  config.wr_key.toCharArray(text_buf, (config.wr_key.length())+1);
  int x = ThingSpeak.writeFields(config.ch_id, text_buf);
  if(x == 200) Serial.println("Channel update successful.");
  else Serial.println("Problem updating channel. HTTP error code " + String(x));
}

void sendToMQTT(void){
  char d[50];
  String(sensors.temp_out).toCharArray(d, (String(sensors.temp_out).length()) + 1);
  mqtt.publish("sensor/temp_out", d);
  String(sensors.temp_in).toCharArray(d, (String(sensors.temp_in).length()) + 1);
  mqtt.publish("sensor/temp_in", d);
  String(sensors.temp_extra).toCharArray(d, (String(sensors.temp_extra).length()) + 1);
  mqtt.publish("sensor/temp_extra", d);
  String(sensors.hum_out).toCharArray(d, (String(sensors.hum_out).length()) + 1);
  mqtt.publish("sensor/hum_out", d);
  String(sensors.hum_in).toCharArray(d, (String(sensors.hum_in).length()) + 1);
  mqtt.publish("sensor/hum_in", d);
  String(sensors.pres_out).toCharArray(d, (String(sensors.pres_out).length()) + 1);
  mqtt.publish("sensor/pres_out", d);
  String(sensors.pres_in).toCharArray(d, (String(sensors.pres_in).length()) + 1);
  mqtt.publish("sensor/pres_in", d);
  String(sensors.light).toCharArray(d, (String(sensors.light).length()) + 1);
  mqtt.publish("sensor/light", d);
  String(sensors.uBat).toCharArray(d, (String(sensors.uBat).length()) + 1);
  mqtt.publish("sensor/u_bat", d);
  String(sensors.bat_level).toCharArray(d, (String(sensors.bat_level).length()) + 1);
  mqtt.publish("sensor/bat_level", d);
}

void connectToWiFi(void){
  if(!ssids.num){
    WiFi.softAP(config.ap_ssid, config.ap_pass, config.chnl, config.hide);
    WiFi.mode(WIFI_AP_STA);
    String IP = WiFi.localIP().toString();
    if(IP == "0.0.0.0") WiFi.disconnect();
    led(1, 0, 1);
    web_settings();
    while(1){
      webServer.handleClient();
      yield();
    }
  }
  else{
    if(WiFi.status() != WL_CONNECTED){
      uint8_t n = WiFi.scanNetworks();
      if(n != 0){
        for(uint8_t i=0; i<n; i++){
          for(uint8_t k=0; k<ssids.num; k++){
            delay(1);
            if(WiFi.SSID(i) == ssids.ssid[k]){
              rssi = WiFi.RSSI(i);
              ssids.ssid[k].toCharArray(ssid, (ssids.ssid[k].length()) + 1);
              ssids.pass[k].toCharArray(password, (ssids.pass[k].length()) + 1);
              WiFi.begin(ssid, password);
              break;
            }
          }
        }
      }
      uint8_t e = 0;
      while(WiFi.status() != WL_CONNECTED){
        if((e++) > 20){
          for(uint8_t k=0; k<ssids.num; k++){
            delay(1);
            ssids.ssid[k].toCharArray(ssid, (ssids.ssid[k].length()) + 1);
            ssids.pass[k].toCharArray(password, (ssids.pass[k].length()) + 1);
            WiFi.begin(ssid, password);
            if(WiFi.status() == WL_CONNECTED) goto connectedd;
          }
          if(e&1) led(1, 1, 0);
          else{
            if(sensors.bat_level == 1) led(1, 0, 0);
            else led(0, 1, 0);
          }
          Serial.print("Unable to connect to "); Serial.println(ssid);
          Serial.println("Going sleep");
          is_settings();
          led(0, 0, 0);
          sensors_sleep();
          ESP.deepSleep(120000000);
        }
        if(e&1)led(1, 1, 0);
        else{
          if(sensors.bat_level == 1) led(1, 0, 0);
          else led(0, 1, 0);
        }
        delay(500);
        is_settings();
      }
    }
connectedd:
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    if(config.typ == 1){
      IPAddress ip;
      IPAddress subnet;
      IPAddress gateway;
      IPAddress dns1;
      IPAddress dns2;
      if(ip.fromString(config.ip) and
         gateway.fromString(config.gateway) and
         subnet.fromString(config.mask) and
         dns1.fromString(config.dns1) and
         dns2.fromString(config.dns2)){
        WiFi.config(ip, gateway, subnet, dns1, dns2);
      }
    }
    rssi = viewRSSI(String(WiFi.SSID()));
  }
  WiFi.SSID().toCharArray(ssid, (WiFi.SSID().length()) + 1);
  config.mdns.toCharArray(text_buf, (config.mdns.length()) + 1);
  MDNS.begin(text_buf);
  MDNS.addService("http", "tcp", 80);
  Serial.print("connected to "); Serial.println(WiFi.SSID());
  Serial.print("IP="); Serial.println(WiFi.localIP().toString());
}

int viewRSSI(String ssid){
  uint8_t n = WiFi.scanNetworks();
  int rssi = 0;
  if(n != 0){
    for(uint8_t i=0; i<n; i++){
      if(WiFi.SSID(i) == ssid) rssi = WiFi.RSSI(i);
    }
  }
  return rssi;
}

void read_eeprom(void){
  Serial.println("read config");
  String fData, fileData;
  File file = SPIFFS.open("/save/save.json", "r");
  if(file){
    fileData = file.readString();
    file.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(fileData);
    if(root.success()){
      String ap_ssid    = root["APSSID"];
      String ap_pass    = root["APPASS"];
      String ap_ip      = root["APIP"];
      String ap_mask    = root["APMASK"];
      String wr_key     = root["WRITE"];
      String ip         = root["IP"];
      String mask       = root["MASK"];
      String gw         = root["GATEWAY"];
      String dns1       = root["DNS1"];
      String dns2       = root["DNS2"];
      String lat        = root["LAT"];
      String lon        = root["LON"];
      String mdns       = root["MDNS"];
      config.temp[0]    = root["TEMP"];
      config.temp[1]    = root["ITEMP"];
      config.temp[2]    = root["ETEMP"];
      config.pres[0]    = root["PRES"];
      config.pres[1]    = root["IPRES"];
      config.hum[0]     = root["HUM"];
      config.hum[1]     = root["IHUM"];
      config.light      = root["LIGHT"];
      config.period     = root["PERIOD"];
      config.chnl       = root["CHNL"];
      config.hide       = root["HIDE"];
      config.lang       = root["LANG"];
      config.typ        = root["TYPE"];
      config.narod      = root["NARODMON"];
      config.thingspeak = root["THINGSPEAK"];
      config.mqtt       = root["MQTT"];
      config.ch_id      = root["CH_ID"];
      config.mqtt_port  = root["MQTT_PORT"];
      String mqtt_server= root["MQTT_SERVER"];
      String mqtt_login = root["MQTT_LOGIN"];
      String mqtt_pass  = root["MQTT_PASS"];
      config.k          = root["K"];
      config.ac         = root["AC"];
      config.f1         = root["F1"];
      config.f2         = root["F2"];
      config.f3         = root["F3"];
      config.f4         = root["F4"];
      config.f5         = root["F5"];
      config.f6         = root["F6"];
      config.f7         = root["F7"];
      config.f8         = root["F8"];
      config.t_cor      = root["T_COR"];
      config.p_cor      = root["P_COR"];
      config.h_cor      = root["H_COR"];
      config.l_cor      = root["L_COR"];
      config.ti_cor     = root["TI_COR"];
      config.pi_cor     = root["PI_COR"];
      config.hi_cor     = root["HI_COR"];
      config.te_cor     = root["TE_COR"];
      config.t_unt      = root["T_UNT"];
      config.p_unt      = root["P_UNT"];
      config.ti_unt     = root["TI_UNT"];
      config.pi_unt     = root["PI_UNT"];
      config.te_unt     = root["TE_UNT"];
      config.ip         = ip;
      config.mask       = mask;
      config.gateway    = gw;
      config.dns1       = dns1;
      config.dns2       = dns2;
      config.latitude   = lat;
      config.longitude  = lon;
      config.mdns       = mdns;
      config.wr_key     = wr_key;
      if(ap_ssid != "") ap_ssid.toCharArray(config.ap_ssid, (ap_ssid.length()) + 1);
      if(ap_pass != "") ap_pass.toCharArray(config.ap_pass, (ap_pass.length()) + 1);
      if(mqtt_server != "") mqtt_server.toCharArray(config.mqtt_server, (mqtt_server.length()) + 1);
      if(mqtt_login != "") mqtt_login.toCharArray(config.mqtt_login, (mqtt_login.length()) + 1);
      if(mqtt_pass != "") mqtt_pass.toCharArray(config.mqtt_pass, (mqtt_pass.length()) + 1);
      if(ap_ip != "") config.ap_ip = ap_ip;
      if(ap_mask != "") config.ap_mask = ap_mask;
    }
  }
  File f = SPIFFS.open("/save/ssids.json", "r");
  if(f){
    fData = f.readString();
    f.close();
    DynamicJsonBuffer jsonBuf;
    JsonObject& json = jsonBuf.parseObject(fData);
    if(json.success()){
      ssids.num = json["num"];
      for(uint8_t i=0; i<ssids.num; i++){
        ssids.ssid[i] = json["nets"][i * 2].as<String>();
        ssids.pass[i] = json["nets"][i * 2 + 1].as<String>();
      }
    }
  }
}

void led(bool r, bool g, bool b){
  digitalWrite(R, !r);
  digitalWrite(G, !g);
  digitalWrite(B, !b);
}

void is_settings(void){
  if(!digitalRead(BUTTON)){
    Serial.println("*********************************");
    Serial.println("entering settings mode");
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    if(ip.fromString(config.ap_ip) and gateway.fromString(config.ap_ip) and subnet.fromString(config.ap_mask)){
      WiFi.softAPConfig(ip, gateway, subnet);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(config.ap_ssid, config.ap_pass, config.chnl, config.hide);
    Serial.printf("connect to %s, password is %s\r\n", config.ap_ssid, config.ap_pass);
    Serial.println("type " + config.ap_ip + " in address bar of your browser");
    String IP = WiFi.localIP().toString();
    Serial.print("alt IP is "); Serial.println(IP);
    if(IP == "0.0.0.0") WiFi.disconnect();
    led(1, 0, 1);
    web_settings();
    while(1){
      webServer.handleClient();
      yield();
    }
  }
}

void BatteryLevel(void){
  float adc = analogRead(A0);
  float cor = -config.k;
  cor = cor + 400;
  sensors.uBat = adc / cor;
  sensors.bat_level = 1;
  if(!config.ac and sensors.uBat < 3.3){
    led(0, 0, 0);
    sensors_sleep();
    ESP.deepSleep(999999999*999999999U,WAKE_NO_RFCAL);
  }
  if(sensors.uBat >= 3.3 and sensors.uBat < 3.5) sensors.bat_level = 1;
  if(sensors.uBat >= 3.5 and sensors.uBat < 3.7) sensors.bat_level = 2;
  if(sensors.uBat >= 3.7 and sensors.uBat < 3.8) sensors.bat_level = 3;
  if(sensors.uBat >= 3.8 and sensors.uBat < 3.9) sensors.bat_level = 4;
  if(sensors.uBat >= 3.9) sensors.bat_level = 5;
  Serial.print("ADC="); Serial.println(adc);
  Serial.print("U bat="); Serial.print(sensors.uBat); Serial.println("V");
  Serial.printf("bat level=%d\r\n", sensors.bat_level);
}

void sensors_init(void){
  Wire.begin();
    //BME280/BMP280
  bme1.parameter.communication = 0;
  bme2.parameter.communication = 0;
  bme1.parameter.I2CAddress = 0x77;
  bme2.parameter.I2CAddress = 0x76;
  bme1.parameter.sensorMode = 0b11;
  bme2.parameter.sensorMode = 0b11;
  bme1.parameter.IIRfilter = 0b100;
  bme2.parameter.IIRfilter = 0b100;
  bme1.parameter.humidOversampling = 0b101;
  bme2.parameter.humidOversampling = 0b101;
  bme1.parameter.tempOversampling = 0b101;
  bme2.parameter.tempOversampling = 0b101;
  bme1.parameter.pressOversampling = 0b101;
  bme2.parameter.pressOversampling = 0b101;
  bme1.parameter.pressureSeaLevel = 1013.25;
  bme2.parameter.pressureSeaLevel = 1013.25;
  bme1.parameter.tempOutsideCelsius = 15;
  bme2.parameter.tempOutsideCelsius = 15;  
  int b1 = bme1.init();
  int b2 = bme2.init();
  if(b1 == 0x58) sensors.bmp1Detected = true;
  if(b2 == 0x58) sensors.bmp2Detected = true;
  if(b1 == 0x60) sensors.bme1Detected = true;
  if(b2 == 0x60) sensors.bme2Detected = true;
    //DS18B20
  ds.begin();
  sensors.dsDetected = ds.getDeviceCount();
  if(sensors.dsDetected){
    ds.getAddress(thermometer1, 0);
    ds.getAddress(thermometer2, 1);
    ds.getAddress(thermometer3, 2);
    ds.setResolution(thermometer1, 12);
    ds.setResolution(thermometer2, 12);
    ds.setResolution(thermometer3, 12);
    ds.requestTemperatures();
  }
    //BMP180
  if(bmp.begin()) sensors.bmp180Detected = true;
    //SHT21;
  SHT21.begin();
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(0xE7);
  Wire.endTransmission();
  delay(100);
  Wire.requestFrom(SHT21_ADDRESS, 1);
  if(Wire.available() == 1){
    Wire.read();
    sensors.shtDetected = true;
  }
    //DHT22
  dht.setup(DHTPIN, DHTesp::DHT22);
  if(!isnan(dht.getHumidity()) and !isnan(dht.getTemperature())) sensors.dhtDetected = true;
    //MAX44009
  if(!max_light.begin()) sensors.max44Detected = true;
    //BH1750
  if(lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) sensors.bhDetected = true;
}

float get_temp(uint8_t s){
  float temp = 404.0;
  ds_upd = false;
  if(config.temp[s] == 1){
    if(sensors.bme1Detected or sensors.bmp1Detected){
      temp = bme1.readTempC();
    }
  }
  if(config.temp[s] == 2){
    if(sensors.bme2Detected or sensors.bmp2Detected){
      temp = bme2.readTempC();
    }
  }
  if(config.temp[s] == 3){
    if(sensors.bmp180Detected){
      temp = bmp.readTemperature();
    }
  }
  if(config.temp[s] == 4){
    if(sensors.dsDetected){
      temp = ds.getTempC(thermometer1);
      ds_upd = true;
    }
  }
  if(config.temp[s] == 5){
    if(sensors.dsDetected){
      temp = ds.getTempC(thermometer2);
      ds_upd = true;
    }
  }
  if(config.temp[s] == 6){
    if(sensors.dsDetected){
      temp = ds.getTempC(thermometer3);
      ds_upd = true;
    }
  }
  if(config.temp[s] == 7){
    if(sensors.dhtDetected){
      temp = dht.getTemperature();
    }
  }
  if(config.temp[s] == 8){
    if(sensors.shtDetected){
      temp = SHT21.getTemperature();
    }
  }
  if(ds_upd) ds.requestTemperatures();
  return temp;
}

float get_pres(uint8_t s){
  float pres = 4040;
  if(config.pres[s] == 1){
    if(sensors.bme1Detected or sensors.bmp1Detected){
      bme1.readTempC(); 
      pres = bme1.readPressure();
    }
  }
  if(config.pres[s] == 2){
    if(sensors.bme2Detected or sensors.bmp2Detected){
      bme2.readTempC(); 
      pres = bme2.readPressure();
    }
  }
  if(config.pres[s] == 3){
    if(sensors.bmp180Detected){
      bmp.readTemperature(); 
      pres = bmp.readPressure() / 100.0;
    }
  }
  return pres;
}

float get_hum(uint8_t s){
  float hum = 404;
  if(config.hum[s] == 1){
    if(sensors.bme1Detected){
      bme1.readTempC(); 
      hum = bme1.readHumidity();
    }
  }
  if(config.hum[s] == 2){
    if(sensors.bme2Detected){
      bme2.readTempC(); 
      hum = bme2.readHumidity();
    }
  }
  if(config.hum[s] == 3){
    if(sensors.dhtDetected){
      dht.getTemperature(); 
      hum = dht.getHumidity();
    }
  }
  if(config.hum[s] == 4){
    if(sensors.shtDetected){
      SHT21.getTemperature(); 
      hum = SHT21.getHumidity();
    }
  }
  return hum;
}

static signed long get_light(void){
  static signed long lx = -1.0;
  if(config.light == 1) if(sensors.max44Detected) lx = max_light.get_lux();
  if(config.light == 2) if(sensors.bhDetected) lx = lightMeter.readLightLevel();
  return lx;
}

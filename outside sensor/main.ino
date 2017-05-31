/* Outside sensor v1.0
   © Alexandr Piteli himikat123@gmail.com, Chisinau, Moldova, 2017
   http://esp8266.atwebpages.com
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <Arduino.h>
#include <pgmspace.h>
#include "SparkFunBME280.h"
#include "Wire.h"

extern "C" {
#include "main.h"
#include "languages.h"
}

#define site "http://esp8266.atwebpages.com/api/"
#define BUTTON         0
#define R              12
#define G              13
#define B              14

ESP8266WebServer webServer(80);
File fsUploadFile;
BME280 mySensor;

void setup() {
   //PINs
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
   //Serial port
  Serial.begin(115200);
  while (!Serial);
  //SPIFS
  if (!SPIFFS.begin()) while (1) yield();
  //EEPROM
  Serial.println("read settings");
  read_eeprom();
  Serial.println("----------------------------------");
  //PINs
  pinMode(BUTTON, INPUT);
  //WIFI MODE
  WiFi.mode(WIFI_STA);
   //BME280
  mySensor.settings.commInterface=I2C_MODE;
  mySensor.settings.I2CAddress=0x76;
  mySensor.settings.runMode=3;
  mySensor.settings.tStandby=0;
  mySensor.settings.filter=0;
  mySensor.settings.tempOverSample=1;
  mySensor.settings.pressOverSample=1;
  mySensor.settings.humidOverSample=1;
  delay(10);
  mySensor.begin();
  Serial.print("Temperature: "); Serial.print(mySensor.readTempC(), 2); Serial.println(" degrees C");
  Serial.print("Pressure: "); Serial.print(mySensor.readFloatPressure(), 2); Serial.println(" Pa");
  Serial.print("%RH: "); Serial.print(mySensor.readFloatHumidity(), 2); Serial.println(" %");
  Serial.println("-----------------------------------");
   //battery
  Serial.println("read battery level");
  bat_level=BatteryLevel();
  if(bat_level==1) led(1,0,0);
  else led(0,1,0);
  Serial.println("-----------------------------------");
   // WiFi
  html.ssid.toCharArray(ssid,(html.ssid.length())+1);
  html.pass.toCharArray(password,(html.pass.length())+1);
  if(html.ssid==""){
    Serial.print("Creating network \""); Serial.print(rtcData.AP_SSID);
    Serial.print("\" with password \""); Serial.print(rtcData.AP_PASS); Serial.println("\"");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(rtcData.AP_SSID,rtcData.AP_PASS);
    led(1,0,1);
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
  else{
    if(String(WiFi.SSID())!=String(ssid)) WiFi.begin(ssid,password);
    if(html.typ==1){
      IPAddress ip;
      IPAddress subnet;
      IPAddress gateway;
      if(ip.fromString(html.ip) and gateway.fromString(html.gateway) and subnet.fromString(html.mask)) {
        WiFi.config(ip,gateway,subnet);
      }
    }
    uint8_t e=0;
    Serial.print("connecting to "); Serial.println(ssid);
    while(WiFi.status()!=WL_CONNECTED){
      if(e&1) led(1,1,0);
      else{
        if(bat_level==1) led(1,0,0);
        else led(0,1,0);
      }
      if((e++)>50){
        Serial.print("unable to connect to "); Serial.println(ssid);
        is_settings();
        led(0,0,0);
        mySensor.settings.runMode=0;
        ESP.deepSleep(1200000000);
      }
      delay(500);
    }
  }
  Serial.print("connected to "); Serial.println(ssid);
  String IP=WiFi.localIP().toString();
  Serial.print("IP "); Serial.println(IP);
}

void loop(){
  is_settings();
  if(WiFi.status()==WL_CONNECTED){
    getCoordinates();
    Serial.print("network ");
    Serial.print(html.ssid);
    Serial.println(" is connected");
    Serial.println("---------------------------");
    Serial.print("Latitude "); Serial.println(latitude);
    Serial.print("Longitude "); Serial.println(longitude);
    Serial.print("Altitude "); Serial.println(altitude);
    String mac=WiFi.macAddress();
    float temp=mySensor.readTempC();
    url=site;
    url+="sensor.php?MAC=";
    url+=mac;
    url+="&LAT=";
    url+=latitude;
    url+="&LON=";
    url+=longitude;
    url+="&ALT=";
    url+=altitude;
    url+="&TEMP=";
    url+=temp;
    url+="&PRES=";
    url+= mySensor.readFloatPressure()/100;
    url+="&HUM=";
    url+=mySensor.readFloatHumidity();
    url+="&BAT=";
    url+=bat_level;
    url+="&FW=";
    url+=fw;
    url+="&KEY=";
    url+=sha1(mac);
    url+=temp;
    HTTPClient client;
    client.begin(url);
    Serial.println(url);
    int httpCode=client.GET();
    httpData=client.getString();
    Serial.print("Server returned: ");
    Serial.println(httpData);
    client.end();
    led(0,0,1);
    httpData="";
    is_settings();
    delay(1000);
    Serial.println("going sleep");
    led(0,0,0);
    mySensor.settings.runMode=0;
    ESP.deepSleep(1200000000);
  }
  else{
    Serial.print("network ");
    Serial.print(html.ssid);
    Serial.println(" was not found");
    is_settings();
    Serial.println("going sleep");
    led(0,0,0);
    mySensor.settings.runMode=0;
    ESP.deepSleep(120000000);
  }
}
void led(bool r,bool g,bool b){
  digitalWrite(R,!r);
  digitalWrite(G,!g);
  digitalWrite(B,!b);
}

void is_settings(void){
  if(!digitalRead(BUTTON)){
    Serial.println("*********************************");
    Serial.println("entering settings mode");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(rtcData.AP_SSID,rtcData.AP_PASS);
    Serial.printf("connect to %s, password is %s\r\n", rtcData.AP_SSID,rtcData.AP_PASS);
    Serial.println("type 192.168.4.1 in address bar of your browser");
    String IP=WiFi.localIP().toString();
    Serial.print("alt IP is "); Serial.println(IP);
    led(1,0,1);
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
}

int viewNetworks(void){
  uint8_t n=WiFi.scanNetworks();
  Serial.printf("found %d networks\r\n",n);
  int rssi=0;
  if(n!=0){
    for(uint8_t i=0;i<n;i++){
      delay(1);
      if(WiFi.SSID(i)==html.ssid) rssi=WiFi.RSSI(i);
      Serial.print(WiFi.SSID(i)); Serial.print(" ");
      Serial.println(WiFi.RSSI(i));
    }
  }
  return rssi;
}

void read_eeprom(void){
  ESP.rtcUserMemoryRead(0,(uint32_t*)&rtcData,sizeof(rtcData));
  /*if(rtcData.crc_ssid!=calculateCRC32(((uint8_t*) &rtcData.AP_SSID),sizeof(rtcData.AP_SSID))) */strcpy(rtcData.AP_SSID, DEFAULT_AP_SSID);
  /*if(rtcData.crc_pass!=calculateCRC32(((uint8_t*) &rtcData.AP_PASS),sizeof(rtcData.AP_PASS))) */strcpy(rtcData.AP_PASS, DEFAULT_AP_PASS);
  //Serial.printf("AP_SSID=%s AP_PASS=%s\r\n",rtcData.AP_SSID,rtcData.AP_PASS);

  String fData;
  File f=SPIFFS.open("/bat.json","r");
  if(f){
    fData=f.readString();
    f.close();
    DynamicJsonBuffer jsonBuf;
    JsonObject& json=jsonBuf.parseObject(fData);
    if(json.success()){
      battery.min=json["min"];
      battery.max=json["max"];
      Serial.printf("bat min=%d bat max=%d\r\n",battery.min,battery.max);
    }
  }

  String fileData;
  File file=SPIFFS.open("/settings.json","r");
  if(file){
    fileData=file.readString();
    file.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root=jsonBuffer.parseObject(fileData);
    if(root.success()){
      String ssid=root["ssid"];
      String pass=root["pass"];
      String ip=root["ip"];
      String mask=root["mask"];
      String gw=root["gw"];
      html.lang=root["lng"];
      html.typ=root["type"];
      html.ssid=ssid;
      html.pass=pass;
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
    }
  }
  if(html.lang>3) html.lang=0;
  Serial.print("SSID="); Serial.println(html.ssid);
  Serial.print("PASS="); Serial.println(html.pass);
  Serial.print("LANG="); Serial.println(html.lang);
  Serial.print("CONN TYPE="); Serial.println(html.typ);
  Serial.print("IP="); Serial.println(html.ip);
  Serial.print("MASK="); Serial.println(html.mask);
  Serial.print("GATEWAY="); Serial.println(html.gateway);
}

void save_eeprom(void){
  rtcData.crc_ssid=calculateCRC32(((uint8_t*) &rtcData.AP_SSID),sizeof(rtcData.AP_SSID));
  rtcData.crc_pass=calculateCRC32(((uint8_t*) &rtcData.AP_PASS),sizeof(rtcData.AP_PASS));
  ESP.rtcUserMemoryWrite(0,(uint32_t*) &rtcData,sizeof(rtcData));

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.createObject();
  root["ssid"]=html.ssid;
  root["pass"]=html.pass;
  root["ip"]=html.ip;
  root["mask"]=html.mask;
  root["gw"]=html.gateway;
  root["lng"]=html.lang;
  root["type"]=html.typ;
  File file=SPIFFS.open("/settings.json","w");
  if(file){
    root.printTo(file);
    file.close();
  }
}

uint32_t calculateCRC32(const uint8_t *data,size_t length){
  uint32_t crc=0xffffffff;
  while(length--){
    uint8_t c=*data++;
    for(uint32_t i=0x80;i>0;i>>=1){
      bool bit=crc&0x80000000;
      if(c&i) bit=!bit;
      crc<<=1;
      if(bit)crc^=0x04c11db7;
    }
  }
  return crc;
}

int BatteryLevel(void){
  int level_battery=analogRead(A0);
  if(level_battery>battery.max){
    battery.max=level_battery;
    save_bat();
  }
  if(level_battery<battery.min){
    battery.min=level_battery;
    save_bat();
  }

  int b1=(battery.max-battery.min);
  int b2=(battery.max+1-level_battery);
  int b3=round(b1/b2);
  if(b3>5) b3=5;
  Serial.printf("ADC=%d\r\n",level_battery);
  Serial.printf("level=%d\r\n",b3);
  return b3;
}

void save_bat(void){
  String save="{\"min\":";
  save+=battery.min;
  save+=",\"max\":";
  save+=battery.max;
  save+="}";
  File file=SPIFFS.open("/bat.json","w");
  if(file){
    file.print(save);
    file.close();
  }
}

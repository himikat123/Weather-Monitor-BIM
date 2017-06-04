/* Weather Station v2.1
 * © Alexandr Piteli himikat123@gmail.com, Chisinau, Moldova, 2016-2017 
 * http://esp8266.atwebpages.com
 */
                               // Board ESP-12E 
                               // 1MB (128kB SPIFFS) 
#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NtpClientLib.h>
#include <UTFT.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <JPEGDecoder.h>
#include <Arduino.h>
#include <Print.h>
#include <pgmspace.h>
#include <SPI.h>
//#include <Adafruit_Sensor.h>
//#include <DHT.h>
//#include <DHT_U.h>

extern "C"{
  #include "main.h"
  #include "languages.h"
}

#define site "http://esp8266.atwebpages.com/api/"
#define ONE_WIRE_BUS 2
#define BUTTON 0
#define BACKLIGHT 12
#define CS 15
#define RES 5
#define DC 4
//#define DHTPIN 2
//#define DHTTYPE DHT22

UTFT myGLCD(ILI9341_S5P,CS,RES,DC);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
ESP8266WebServer webServer(80);
File fsUploadFile;
//DHT_Unified dht(DHTPIN, DHTTYPE);

extern uint8_t SmallFontRu[];
extern uint8_t BigFontRu[];

void setup(){
   //Serial port
  Serial.begin(115200);
  while (!Serial);
   //SPIFS
  if(!SPIFFS.begin()) while(1) yield();
   //PINs
  pinMode(BUTTON,INPUT);
  pinMode(BACKLIGHT,OUTPUT);
   //LCD
  myGLCD.InitLCD();
  analogWrite(BACKLIGHT,10);
  drawFSJpeg("/logo.jpg",0,0);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRect(0,0,319,18);
   //EEPROM 
  read_eeprom();
  analogWrite(BACKLIGHT,html.bright*10);
   //WIFI MODE
  WiFi.mode(WIFI_STA);
   //NTP
  ntp=ntpClient::getInstance("time.windows.com",1);
  ntp->setInterval(15,1800);
  ntp->setTimeZone(html.zone);
  ntp->setDayLight(html.adj);
  ntp->begin();
   //DS18B20
  sensors.begin();
  sensors.getAddress(insideThermometer,0); 
  sensors.setResolution(insideThermometer,9);
  sensors.requestTemperatures();
  tempInside=sensors.getTempC(insideThermometer);
   //DHT22
  //dht.begin();
  //sensor_t sensor;
  //dht.temperature().getSensor(&sensor);
  //dht.humidity().getSensor(&sensor);
   //battery
  showBatteryLevel();
   // WiFi
  myGLCD.drawBitmap(273,2,16,16,nowifi,1);
  html.ssid.toCharArray(ssid,(html.ssid.length())+1);
  html.pass.toCharArray(password,(html.pass.length())+1);
  is_settings();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.setFont(SmallFontRu);
  if(html.ssid==""){
    showSettingsMode();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(rtcData.AP_SSID,rtcData.AP_PASS);
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
      if(ip.fromString(html.ip) and gateway.fromString(html.gateway) and subnet.fromString(html.mask)){
        WiFi.config(ip,gateway,subnet);
      }
    }
    uint8_t e=0;
    myGLCD.setFont(SmallFontRu);
    sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connecting_to),ssid);
    sprintf(text_buf,"%-30s",text_buf);
    text_buf[30]='\0';
    myGLCD.print(text_buf,2,2);
    while(WiFi.status()!=WL_CONNECTED){
      if((e++)>50){
        sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].unable_to_connect_to),ssid);
        sprintf(text_buf,"%-30s",text_buf);
        text_buf[30]='\0';
        myGLCD.print(text_buf,2,2);
        delay(5000);
        if(html.sleep==0) ESP.reset();
        else{
          analogWrite(BACKLIGHT,0);
          myGLCD.lcdOff();
          ESP.deepSleep(0);
        }
      }
      delay(500);
      is_settings();
    } 
  }
  sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connected_to),ssid);
  sprintf(text_buf,"%-30s",text_buf);
  text_buf[30]='\0';
  myGLCD.print(text_buf,2,2);
}

void loop(){
  if(WiFi.status()==WL_CONNECTED){
    showBatteryLevel();
    showWiFiLevel(rssi);
    getCoordinates();
    getWeatherNow();
    getWeatherDaily();  
    showInsideTemp();
    siteTime();
    database();
    out();
      //sensors_event_t event;
      //dht.humidity().getEvent(&event);
      //if(isnan(event.relative_humidity)) ;
      //else weather.humidity=event.relative_humidity;
    showWeatherNow();
    showCityName();
    showTime();
    showWeatherToday();
    showWeatherTomorrow();
    showWeatherAfterTomorrow();
  }
  else{
    myGLCD.setColor(VGA_WHITE);
    myGLCD.setBackColor(VGA_BLACK);
    myGLCD.setFont(SmallFontRu);
    sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].unable_to_connect_to),ssid);
    sprintf(text_buf,"%-30s",text_buf);
    text_buf[30]='\0';
    myGLCD.print(text_buf,2,2);
    delay(10000);
    analogWrite(BACKLIGHT,0);
    if(html.sleep==0) ESP.reset();
    else{
      myGLCD.lcdOff();
      ESP.deepSleep(0);
    }
  }

  uint16_t sleep;
  if(html.sleep==0) sleep=600;
  else sleep=27*html.sleep;
  for(uint16_t i=0;i<sleep;i++){
    rssi=viewNetworks();
    showBatteryLevel();
    showWiFiLevel(rssi);
    showTime();
    is_settings();
    if(html.sleep==0) showInsideTemp();
  }
  if(html.sleep!=0){
    analogWrite(BACKLIGHT,html.bright*3);
    delay(5000);
    analogWrite(BACKLIGHT,0);
    myGLCD.lcdOff();
    ESP.deepSleep(0);
  }
}

int viewNetworks(void){
  uint8_t n=WiFi.scanNetworks();
  int rssi=0;
  if(n!=0){
    for(uint8_t i=0;i<n;i++){
      delay(1);
      if(WiFi.SSID(i)==html.ssid) rssi=WiFi.RSSI(i);
    }  
  }
  return rssi;
}

void siteTime(){
  url=site; 
  url+="time.php";
  HTTPClient client;
  client.begin(url);
  int httpCode=client.GET();
  if(httpCode==HTTP_CODE_OK){
    httpData=client.getString();
    char stamp[12];
    httpData.toCharArray(stamp,12);
    int dayLight=0;
    if(ntp->getDayLight()) dayLight=3600;
    setTime(atol(stamp)+(html.zone*3600)+dayLight);
  }
}

void database(void){
  byte mac[6];
  WiFi.macAddress(mac);
  if((html.id[0]==0xFF)or(html.id[0]==0)){
    url=site;
    url+="get_id.php?MAC=";
    url+=String(mac[5],HEX)+"-";
    url+=String(mac[4],HEX)+"-";
    url+=String(mac[3],HEX)+"-";
    url+=String(mac[2],HEX)+"-";
    url+=String(mac[1],HEX)+"-";
    url+=String(mac[0],HEX);
    HTTPClient client;
    client.begin(url);
    int httpCode=client.GET();
    if(httpCode>0){
      if(httpCode==HTTP_CODE_OK){
        httpData=client.getString();
        char myID[32];
        httpData.toCharArray(myID,10);
        if(myID>0){
          EEPROM.begin(512);
          EEPROM.put(140,myID);
          EEPROM.commit();
          EEPROM.end();
        }
      }
    }
  }
  url=site;
  url+="database.php?ID=";
  url+=html.id;
  url+="&COUNTRY=";
  country.toUpperCase();
  url+=country;
  url+="&CITY=";
  url+=city;
  url+="&FW=";
  url+=fw;
  url+="&LANG=";
  url+=urlLang;
  url+="&MAC=";
  url+=String(mac[5],HEX)+"-";
  url+=String(mac[4],HEX)+"-";
  url+=String(mac[3],HEX)+"-";
  url+=String(mac[2],HEX)+"-";
  url+=String(mac[1],HEX)+"-";
  url+=String(mac[0],HEX);
  HTTPClient client;
  client.begin(url+"&KEY="+sha1(html.id+city)+html.id);
  client.GET();
  client.end();
}

void out(void){
  url=site;
  url+="outside.php?MAC=";
  url+=html.sensor;
  HTTPClient client;
  client.begin(url);
  int httpCode=client.GET();
  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root=jsonBuffer.parseObject(httpData);
      if(root.success()){
        outside.lat     =root["lat"];
        outside.lon     =root["lon"];
        outside.alt     =root["alt"];
        outside.temp    =root["temp"];
        outside.pres    =root["press"];
        outside.humidity=root["hum"];
        outside.bat     =root["bat"];
        outside.updated =root["updated"];
      }
      client.end();
    }
  }
  httpData="";
}

void is_settings(void){
  if(!digitalRead(BUTTON)){
    showSettingsMode();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(rtcData.AP_SSID,rtcData.AP_PASS);
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
}

void read_eeprom(void){
  EEPROM.begin(512);
  EEPROM.get(140,html.id);
  EEPROM.end();
  
  ESP.rtcUserMemoryRead(0,(uint32_t*) &rtcData,sizeof(rtcData));
  if(rtcData.crc_ssid!=calculateCRC32(((uint8_t*) &rtcData.AP_SSID),sizeof(rtcData.AP_SSID))) strcpy(rtcData.AP_SSID,DEFAULT_AP_SSID);
  if(rtcData.crc_pass!=calculateCRC32(((uint8_t*) &rtcData.AP_PASS),sizeof(rtcData.AP_PASS))) strcpy(rtcData.AP_PASS,DEFAULT_AP_PASS);

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
      String ssid =root["ssid"];  
      String pass =root["pass"];
      String city =root["city"];
      String appid=root["appid"];
      html.zone   =root["zone"];
      html.bright =root["bright"];
      html.adj    =root["daylight"];
      html.units  =root["units"];
      html.pres   =root["press"];
      html.timef  =root["time"];
      html.lang   =root["lng"];
      html.sleep  =root["sleep"];
      html.typ    =root["type"];
      String ip   =root["ip"];
      String mask =root["mask"];
      String gw   =root["gateway"];
      String mac  =root["mac_out"];
      html.sensor=mac;
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
      html.ssid=ssid;
      html.pass=pass;
      html.city=city;
      html.appid=appid;
    }
  }
  if(html.sleep>100) html.sleep=1;
  if(html.lang>3) html.lang=0;
  if(html.adj>1) html.adj=0;
  if((html.zone>13) and (html.zone<-13)) html.zone=0;
  if(html.units>1) html.units=0;
  if(html.pres>1) html.pres=0;
  if(html.timef>1) html.timef=0;
  if(html.bright>1024 or html.bright==0) html.bright=30;
  switch(html.lang){
    case 0:urlLang="en";break;
    case 1:urlLang="ru";break;
    case 2:urlLang="ro";break;
    case 3:urlLang="de";break;
    default:urlLang="en";break;
  }
}

void save_eeprom(void){
  rtcData.crc_ssid=calculateCRC32(((uint8_t*) &rtcData.AP_SSID),sizeof(rtcData.AP_SSID));
  rtcData.crc_pass=calculateCRC32(((uint8_t*) &rtcData.AP_PASS),sizeof(rtcData.AP_PASS));  
  ESP.rtcUserMemoryWrite(0,(uint32_t*) &rtcData,sizeof(rtcData));

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.createObject();
  root["ssid"]    =html.ssid;
  root["pass"]    =html.pass;
  root["city"]    =html.city;
  root["appid"]   =html.appid;
  root["zone"]    =html.zone;
  root["bright"]  =html.bright;
  root["daylight"]=html.adj;
  root["units"]   =html.units;
  root["press"]   =html.pres;
  root["time"]    =html.timef;
  root["lng"]     =html.lang;
  root["sleep"]   =html.sleep;
  root["type"]    =html.typ;
  root["ip"]      =html.ip;
  root["mask"]    =html.mask;
  root["gateway"] =html.gateway;
  root["mac_out"] =html.sensor;
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
      if(bit) crc^=0x04c11db7;
    }
  }
  return crc;
}


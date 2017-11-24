/* Weather Monitor BIM v2.7
 * © Alexandr Piteli himikat123@gmail.com, Chisinau, Moldova, 2016-2017 
 * http://esp8266.atwebpages.com
 */
                               // Board ESP-12E 
                               // 1MB (512kB SPIFFS) 
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
#include <UTFT_Geometry.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <JPEGDecoder.h>
#include <Arduino.h>
#include <Print.h>
#include <pgmspace.h>
#include <SPI.h>

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

UTFT myGLCD(ILI9341_S5P,CS,RES,DC);
UTFT_Geometry geo(&myGLCD);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
ESP8266WebServer webServer(80);
File fsUploadFile;

extern uint8_t SmallFontRu[];
extern uint8_t BigFontRu[];
extern uint8_t DotMatrix_M_Num[];

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
  drawFSJpeg("/pic/logo.jpg",0,0);
  myGLCD.setColor(back_color);
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
  sensors.setResolution(insideThermometer,10);
  sensors.requestTemperatures();
  tempInside=sensors.getTempC(insideThermometer);
    //battery
  showBatteryLevel();
    // WiFi
  connectToWiFi();
}

void loop(){
  if(WiFi.status()==WL_CONNECTED){ 
    showBatteryLevel();
    showWiFiLevel(rssi);
    update_weather();
  }
  else{
    myGLCD.setColor(VGA_WHITE);
    myGLCD.setBackColor(0xCE79);
    myGLCD.setFont(SmallFontRu);
    sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].unable_to_connect_to),ssid);
    sprintf(text_buf,"%-35.35s",text_buf);
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
  if(html.sleep==0) sleep=40;
  else sleep=27*html.sleep;
  for(uint16_t i=0;i<sleep;i++){
    if(i==135 and i==270 and i==450 and i==540 and i==675) update_weather();
    if(weather.isDay) analogWrite(BACKLIGHT,html.bright*10);
    else analogWrite(BACKLIGHT,html.bright_n*10);
    rssi=viewRSSI(String(WiFi.SSID()));
    showBatteryLevel();
    showWiFiLevel(rssi);
    showTime();
    is_settings();
    showInsideTemp();
    yield();
  }
  if(html.sleep!=0){
    analogWrite(BACKLIGHT,html.bright*3);
    delay(5000);
    analogWrite(BACKLIGHT,0);
    myGLCD.lcdOff();
    ESP.deepSleep(0);
  }
}

void update_weather(void){
  getCoordinates();
  siteTime();
  showTime();
  getWeatherNow();
  getWeatherDaily();
  showInsideTemp();
  out();
  showWeatherNow();
  showWeatherToday();
  showWeatherTomorrow();
  showWeatherAfterTomorrow();
  database();
  yield();
}

void connectToWiFi(void){
  myGLCD.drawBitmap(273,0,16,16,nowifi,1);
  is_settings();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(back_color);
  myGLCD.setFont(SmallFontRu);
  if(!ssids.num){
    showSettingsMode();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(html.ap_ssid,html.ap_pass);
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
  else{
    myGLCD.setFont(SmallFontRu);
    if(WiFi.status()!=WL_CONNECTED){
      uint8_t n=WiFi.scanNetworks();
      if(n!=0){
        for(uint8_t i=0;i<n;i++){
          for(uint8_t k=0;k<ssids.num;k++){
            delay(1);
            if(WiFi.SSID(i)==ssids.ssid[k]){
              rssi=WiFi.RSSI(i);
              connected_ssid=ssids.ssid[k];
              ssids.ssid[k].toCharArray(ssid,(ssids.ssid[k].length())+1);
              ssids.pass[k].toCharArray(password,(ssids.pass[k].length())+1);
              WiFi.begin(ssid,password);
              sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connecting_to),ssid);
              sprintf(text_buf,"%-30s",text_buf);
              text_buf[30]='\0';
              myGLCD.print(text_buf,2,2);
              break;
            }
          }
        }  
      }
      uint8_t e=0;
      while(WiFi.status()!=WL_CONNECTED){
        if((e++)>20){
          for(uint8_t k=0;k<ssids.num;k++){
            delay(1);
            connected_ssid=ssids.ssid[k];
            ssids.ssid[k].toCharArray(ssid,(ssids.ssid[k].length())+1);
            ssids.pass[k].toCharArray(password,(ssids.pass[k].length())+1);
            WiFi.begin(ssid,password);
            sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connecting_to),ssid);
            sprintf(text_buf,"%-30s",text_buf);
            text_buf[30]='\0';
            myGLCD.print(text_buf,2,2);
            if(WiFi.status()==WL_CONNECTED) goto connectedd;
          }    
          sprintf(text_buf,"%-30s",UTF8(status_lng[html.lang].unable_to_connect_to));
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
 connectedd:
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    if(html.typ==1){
      IPAddress ip;
      IPAddress subnet;
      IPAddress gateway;
      if(ip.fromString(html.ip) and gateway.fromString(html.gateway) and subnet.fromString(html.mask)){
        WiFi.config(ip,gateway,subnet);
      }
    }
    rssi=viewRSSI(String(WiFi.SSID())); 
  }
  WiFi.SSID().toCharArray(ssid,(WiFi.SSID().length())+1);
  sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connected_to),ssid);
  sprintf(text_buf,"%-30s",text_buf);
  text_buf[30]='\0';
  myGLCD.print(text_buf,2,2);
}

int viewRSSI(String ssid){
  uint8_t n=WiFi.scanNetworks();
  int rssi=0;
  if(n!=0){
    for(uint8_t i=0;i<n;i++){
      if(WiFi.SSID(i)==ssid) rssi=WiFi.RSSI(i);
    }  
  }
  return rssi;
}

boolean summertime(){
  if(month()<3 || month()>10) return false;
  if(month()>3 && month()<10) return true;
  if(month()==3 && (hour()+24*day()) >= (1+24*(31-(5*year()/4+4)%7)) || month()==10 && (hour()+24*day())<(1+24*(31-(5*year()/4+1)%7))) return true;
  else return false;
}


void siteTime(){
  String url=site; 
  url+="time.php";
  HTTPClient client;
  client.begin(url);
  int httpCode=client.GET();
  if(httpCode==HTTP_CODE_OK){
    httpData=client.getString();
    char stamp[12];
    httpData.toCharArray(stamp,12);
    int dayLight=0;
    setTime(atol(stamp)+(html.zone*3600));
    if(summertime()) dayLight=3600;
    setTime(atol(stamp)+(html.zone*3600)+dayLight);
  }
}

void database(void){
  byte mac[6];String url;
  WiFi.macAddress(mac);
  int id=atoi(html.id);
  if(id==0){
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
  String url=site;
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
    WiFi.softAP(html.ap_ssid,html.ap_pass);
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
  int id=atoi(html.id);
  String n="0";
  if(id==0) n.toCharArray(html.id,(n.length())+1); 
  
  String fileData;
  File file=SPIFFS.open("/save/save.json","r");
  if(file){
    fileData=file.readString();
    file.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root=jsonBuffer.parseObject(fileData);
    if(root.success()){
      String ap_ssid =root["AP_SSID"];  
      String ap_pass =root["AP_PASS"];
      String city    =root["CITY"];
      String appid   =root["APPID"];
      html.zone      =root["ZONE"];
      html.bright    =root["BRIGHT"];
      html.bright_n  =root["BRIGHT_N"];
      html.adj       =root["DAYLIGHT"];
      html.units     =root["UNITS"];
      html.pres      =root["PRES"];
      html.timef     =root["TIME"];
      html.lang      =root["LANG"];
      html.sleep     =root["SLEEP"];
      html.typ       =root["TYPE"];
      html.k         =root["K"];
      String ip      =root["IP"];
      String mask    =root["MASK"];
      String gw      =root["GATEWAY"];
      String mac     =root["MAC"];
      html.sensor=mac;
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
      ap_ssid.toCharArray(html.ap_ssid,(ap_ssid.length())+1);
      ap_pass.toCharArray(html.ap_pass,(ap_pass.length())+1);
      html.city=city;
      html.appid=appid;
    }
  }
  if(html.sleep>100) html.sleep=1;
  if(html.lang>6) html.lang=0;
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
    case 4:urlLang="lt";break;
    case 5:urlLang="et";break;
    case 6:urlLang="ua";break;
    default:urlLang="en";break;
  }
  File f=SPIFFS.open("/save/ssids.json","r");
  if(f){
    String fData=f.readString();
    f.close();
    DynamicJsonBuffer jsonBuf;
    JsonObject& json=jsonBuf.parseObject(fData);
    if(json.success()){
      ssids.num=json["num"];
      for(uint8_t i=0;i<ssids.num;i++){
        ssids.ssid[i]=json["nets"][i*2].as<String>();
        ssids.pass[i]=json["nets"][i*2+1].as<String>();
      }
    }
  } 
}


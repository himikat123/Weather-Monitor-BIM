/* Weather Monitor BIM v3.9.4
 * © himikat123@gmail.com, Nürnberg, Deutschland, 2016-2019 
 * http://esp8266.atwebpages.com
 */
                               // Board Generic ESP8266 Module 
                               // 4MB (1MB SPIFFS) (or 1MB (512kB SPIFFS) w/o OTA)

#include <Time.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "BlueDot_BME280.h"
#include "Wire.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <DHT_U.h>
#include "SHT21.h"
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
#include <ESP8266TrueRandom.h>
#include <SPI.h>
#include <Ticker.h>

extern "C"{
  #include "main.h"
  #include "languages.h"
}

#define site "http://esp8266.atwebpages.com/api/"
#define ONE_WIRE_BUS 2
#define DHTPIN       0
#define DHTTYPE      DHT22
#define BUTTON       0
#define BACKLIGHT    12
#define CS           15
#define RES          5
#define DC           4

UTFT myGLCD(ILI9341_S5P,CS,RES,DC);
UTFT_Geometry geo(&myGLCD);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer1;
DeviceAddress thermometer2;
BlueDot_BME280 bme1;
BlueDot_BME280 bme2;
DHT_Unified dht(DHTPIN, DHTTYPE);
SHT21 SHT21;
ESP8266WebServer webServer(80);
File fsUploadFile;
Ticker updater;
Ticker sleeper;
Ticker handler;
Ticker ap;
Ticker rn;

extern uint8_t SmallFontRu[];
extern uint8_t BigFontRu[];
extern uint8_t DotMatrix_M_Num[];
extern uint8_t Arial_round[];
extern uint8_t Symbols[];

void setup(){
    //Serial port
  Serial.begin(74880);
  while(!Serial);
    //SPIFS
  if(!SPIFFS.begin()) while(1) yield();
    //PINs
  pinMode(BUTTON,INPUT);
  pinMode(BACKLIGHT,OUTPUT);
    //LCD
  myGLCD.InitLCD();
  analogWrite(BACKLIGHT,10);
  is_settings();
    //EEPROM 
  read_eeprom();
    //i2c
  Wire.pins(ONE_WIRE_BUS,DHTPIN);
    //Sensors
  sensors_init();
    //Bat check
  if(!html.ac){
    float adc=analogRead(A0);
    float cor=-html.k;
    cor=cor+400;
    float Ubat=adc/cor;
    if(Ubat<3.36){
      myGLCD.setColor(VGA_RED);
      analogWrite(BACKLIGHT,300);
      myGLCD.fillRect(100,100,220,104);
      myGLCD.fillRect(100,136,220,140);
      myGLCD.fillRect(100,100,104,140);
      myGLCD.fillRect(216,100,220,140);
      myGLCD.fillRect(220,110,227,130);
      delay(3000);
      analogWrite(BACKLIGHT,0);
      myGLCD.lcdOff();
      ESP.deepSleep(999999999*999999999U,WAKE_NO_RFCAL);
    }
  }
    //checking if icons need update
  String fileData="",upd_icn="";
  File file=SPIFFS.open("/save/upd_icon.json","r");
  if(file){
    fileData=file.readString();
    file.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root=jsonBuffer.parseObject(fileData);
    if(root.success()){
      String upd=root["UPD"];
      upd_icn=upd;  
    }
  }
  if(upd_icn!=html.upd_icn) need_upd_icon=true;
    //old data
  if(ESP.rtcUserMemoryRead(0,(uint32_t*)&weather,8)){
    uint32_t crcOfData=calculateCRC32((uint8_t*)&weather.temp,sizeof(weather.temp));
    if(crcOfData!=weather.crc32){
      weather.temp=404.0;
        //Logo
      analogWrite(BACKLIGHT,10);
      drawFSJpeg("/pic/logo.jpg",0,0);
      myGLCD.setColor(back_color);
      myGLCD.fillRect(0,0,319,18);
      analogWrite(BACKLIGHT,html.bright);
      delay(1000);
      is_settings();
    }
    else ESP.rtcUserMemoryRead(0,(uint32_t*)&weather,sizeof(weather));
    analogWrite(BACKLIGHT,html.bright);
    showBatteryLevel();
    showTime();
    showInsideTemp();
    showWeatherNow(true);
    showWeatherToday();
    showWeatherTomorrow();
    showWeatherAfterTomorrow();
  }
    //WIFI MODE
  WiFi.mode(WIFI_STA);
    //NTP
  ntp=ntpClient::getInstance("time.windows.com",1);
  ntp->setInterval(15,1800);
  ntp->setTimeZone(html.zone);
  ntp->setDayLight(html.adj);
  ntp->begin();
    // Ticker
  updater.attach(1200,updateWeather);
  handler.attach(5,hndlr);
  if(html.sleep==0) sleeper.attach(1201,goSleep);
  else sleeper.attach(html.sleep*60,goSleep);
  ap.attach(60,apTime);
  rn.attach(1,rn_str_plus);
    //mDNS
  html.mdns.toCharArray(text_buf,(html.mdns.length())+1);
  MDNS.begin(text_buf);
  MDNS.addService("http","tcp",80);
    //if settings button is pressed
  is_settings();
  web_settings();
}

void loop(){
  if(update_flag){
    if(WiFi.status()==WL_CONNECTED){
      showBatteryLevel();
      update_weather();
      update_flag=false;
      if(weather.isDay) analogWrite(BACKLIGHT,html.bright);
      else analogWrite(BACKLIGHT,html.bright_n);
      showWiFiLevel(rssi);
    }
    else{
      printCent(UTF8(status_lng[html.lang].unable_to_connect_to),23,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
      connectToWiFi();
      is_settings();
      showInsideTemp();
      showBatteryLevel();
      showTime();
    }
  }
  else{
    if(html.dl!=0){
      tmElements_t from={0,html.fm,html.fh,weekday(),day(),month(),year()-1970};
      uint32_t fromT=makeTime(from);
      tmElements_t to={0,html.tm,html.th,weekday(),day(),month(),year()-1970};
      uint32_t toT=makeTime(to);
      if(fromT<=now() and now()<=toT) weather.isDay=true;
      else weather.isDay=false;
    }
    if(weather.isDay) analogWrite(BACKLIGHT,html.bright);
    else analogWrite(BACKLIGHT,html.bright_n);
    out_bat();
    is_settings();
    showInsideTemp();
    showWeatherNow(false);
    rssi=viewRSSI(String(WiFi.SSID()));
    showBatteryLevel();
    showTime();
    showWiFiLevel(rssi);
    if(r_str==0) printCent(updtd,6,211,156,text_color,back_color,SmallFontRu);
    if(r_str==1){
      String str=UTF8(WeatherNow[html.lang].srise); str+=" ";
      str+=hour(weather.sunrise); str+=":";
      if(minute(weather.sunrise)<10) str+="0";
      str+=minute(weather.sunrise);
      printCent(str,6,211,156,text_color,back_color,SmallFontRu);
    }
    if(r_str==2){
      String str=UTF8(WeatherNow[html.lang].sset); str+=" ";
      str+=hour(weather.sunset); str+=":";
      if(minute(weather.sunset)<10) str+="0";
      str+=minute(weather.sunset);
      printCent(str,6,211,156,text_color,back_color,SmallFontRu);
    }
    if(html.sleep and sleep_flag){
      sleep_flag=false;
      if(weather.isDay) analogWrite(BACKLIGHT,html.bright/3);
      else analogWrite(BACKLIGHT,html.bright_n/3);
      delay(5000);
      analogWrite(BACKLIGHT,0);
      myGLCD.lcdOff();
      ESP.deepSleep(0);
    }
    while(handle_flag){
      webServer.handleClient();
      delay(100);
    }
    handle_flag=true;
  }
  yield();
}

void updateWeather(){
  update_flag=true;
}

void goSleep(){
  sleep_flag=true;
}

void hndlr(){
  handle_flag=false;
}

void apTime(){
  ap_flag=false;
}

void rn_str_plus(){
  if(r_str++>2) r_str=0;
  if(e_str++>1) e_str=0;
}

void update_weather(void){
  String url;
  if(html.os==0){
    sensor();
    connectToWiFi();
  }
  if(html.os>0) out();
  siteTime();
  
  if(need_upd_icon){
    Dir dir=SPIFFS.openDir("/pic");
    myGLCD.setColor(back_color);
    myGLCD.fillRect(0,0,280,16);
    while(dir.next()){
      String fname=dir.fileName();
      if(fname!="logo.jpg"){
        String file_name=fname;
        String msg="updating file ";
        msg+=file_name;
        printCent(msg,0,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
        fname.replace("+","p");
        url=site;
        url+="icon.php?icon="; url+=fname;
        url+="&r="; url+=html.bgr;
        url+="&g="; url+=html.bgg;
        url+="&b="; url+=html.bgb;
        HTTPClient http;
        File f=SPIFFS.open(file_name,"w");
        if(f){
          http.begin(url);
          int httpCode=http.GET();
          if(httpCode>0){
            if(httpCode==HTTP_CODE_OK){
              http.writeToStream(&f);
            }
          } 
          f.close();
        }
        http.end();
      }
    }
    File flupd=SPIFFS.open("/save/upd_icon.json","w");
    if(flupd){
      String upd="{\"UPD\":\"";
      upd+=html.upd_icn;
      upd+="\"}";
      flupd.print(upd);
      flupd.close();
      need_upd_icon=false;
    }
  }
  
  getWeatherNow();
  getWeatherDaily();
  showTime();
  showInsideTemp();
  showWeatherNow(true);
  showWeatherToday();
  showWeatherTomorrow();
  showWeatherAfterTomorrow();
  database();
}

void connectToWiFi(void){
  myGLCD.setColor(text_color);
  sprintf(text_buf,"%c",ANT100);
  myGLCD.setFont(Symbols);
  sprintf(text_buf,"%c",ANT100);
  if(html.ac){
    myGLCD.print(text_buf,290,0);
    myGLCD.setColor(VGA_RED);
    myGLCD.drawLine(290,0,305,15);
    myGLCD.drawLine(290,15,305,0);
  }
  else{
    if(html.battery==0){
      myGLCD.print(text_buf,273,0);
      myGLCD.setColor(VGA_RED);
      myGLCD.drawLine(273,0,288,15);
      myGLCD.drawLine(273,15,288,0);
    }
    if(html.battery==1){
      myGLCD.print(text_buf,265,0);
      myGLCD.setColor(VGA_RED);
      myGLCD.drawLine(265,0,280,15);
      myGLCD.drawLine(265,15,280,0);
    }
  }
  is_settings();
  if(!ssids.num){
    showSettingsMode();
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    if(ip.fromString(html.ap_ip) and gateway.fromString(html.ap_ip) and subnet.fromString(html.ap_mask)){
      WiFi.softAPConfig(ip,gateway,subnet);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(html.ap_ssid,html.ap_pass,html.chnl,html.hide);
    String IP=WiFi.localIP().toString();
    if(IP=="0.0.0.0") WiFi.disconnect(); 
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
  else{
    if(WiFi.SSID()==html.sssid) WiFi.disconnect();
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
              text_buf[30]='\0';
              printCent(text_buf,0,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
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
            text_buf[30]='\0';
            printCent(text_buf,0,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
            if(WiFi.status()==WL_CONNECTED) goto connectedd;
          }
          printCent(UTF8(status_lng[html.lang].unable_to_connect_to),0,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
          showSettingsMode();
          WiFi.mode(WIFI_AP_STA);
          WiFi.softAP(html.ap_ssid,html.ap_pass);
          String IP=WiFi.localIP().toString();
          if(IP=="0.0.0.0") WiFi.disconnect();
          web_settings();
          while(ap_flag){
            webServer.handleClient();
          }
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
      IPAddress dns1;
      IPAddress dns2;
      if(ip.fromString(html.ip) and 
         gateway.fromString(html.gateway) and 
         subnet.fromString(html.mask) and
         dns1.fromString(html.dns1) and
         dns2.fromString(html.dns2)){
        WiFi.config(ip,gateway,subnet,dns1,dns2);
      }
    }
    rssi=viewRSSI(String(WiFi.SSID())); 
  }
  WiFi.SSID().toCharArray(ssid,(WiFi.SSID().length())+1);
  sprintf(text_buf,"%s %s",UTF8(status_lng[html.lang].connected_to),ssid);
  text_buf[30]='\0';
  printCent(text_buf,0,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
  
  html.mdns.toCharArray(text_buf,(html.mdns.length())+1);
  MDNS.begin(text_buf);
  MDNS.addService("http","tcp",80);
  web_settings();
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
  if(month()<3||month()>10) return false;
  if(month()>3&&month()<10) return true;
  if(month()==3&&(hour()+24*day())>=(1+24*(31-(5*year()/4+4)%7))||month()==10&&(hour()+24*day())<(1+24*(31-(5*year()/4+1)%7))) return true;
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
    if(summertime() and html.adj) dayLight=3600;
    setTime(atol(stamp)+(html.zone*3600)+dayLight);
  }
  httpData="";
}

void database(void){
  String url;
  int id=atoi(html.id);
  if(id==0){
    url=site;
    url+="get_id.php?MAC="; url+=WiFi.macAddress();
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
  String country=weather.country;
  String city=weather.city;
  url=site; url+="database.php";
  url+="?ID="; url+=html.id;
  url+="&COUNTRY="; country.replace(" ","_"); url+=country;
  url+="&CITY="; city.replace(" ","_"); url+=city;
  url+="&FW="; url+=fw;
  url+="&LANG="; url+=urlLang;
  url+="&MAC="; url+=WiFi.macAddress();
  url+="&KEY="+sha1(html.id+city)+html.id;
  url.replace(" ","%20");
  HTTPClient client;
  client.begin(url);
  int httpCode=client.GET();

  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
    }
  }
  client.end();
}

void sensor(void){
  myGLCD.setFont(SmallFontRu);
  if(WiFi.SSID()!=html.sssid){
    uint8_t n=WiFi.scanNetworks();
    bool o=false;
    for(uint8_t i=0;i<n;i++){
      if(WiFi.SSID(i)==html.sssid) o=true;
    }
    if(o){
      WiFi.disconnect();
      html.sssid.toCharArray(ssid,(html.sssid.length())+1);
      html.spass.toCharArray(password,(html.spass.length())+1);
      WiFi.begin(ssid,password);
      //myGLCD.print(UTF8(status_lng[html.lang].connecting_sensor),2,2);
      printCent(UTF8(status_lng[html.lang].connecting_sensor),23,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
      uint8_t e=0;
      while(WiFi.status()!=WL_CONNECTED){
        if((e++)>20){
          //myGLCD.print(UTF8(status_lng[html.lang].unable_connect_sensor),2,2);
          printCent(UTF8(status_lng[html.lang].unable_connect_sensor),23,html.ac==0?html.battery==0?272:html.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
          delay(5000);
          break;
        }
        delay(500);
      }
    }
  }  
  if(WiFi.status()==WL_CONNECTED){
    String url="http://";
    url+=html.sip;
    url+="/esp/local.php";
    HTTPClient client;
    client.begin(url);
    int httpCode=client.GET();
    if(httpCode>0){
      if(httpCode==HTTP_CODE_OK){
        httpData=client.getString();
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root=jsonBuffer.parseObject(httpData);
        if(root.success()){
          outside.temp    =root["C"];
          outside.pres    =root["HPA"];
          outside.humidity=root["HUM"];
          int dayLight=summertime()?3600:0;
          outside.updated=now()-(html.zone*3600)-dayLight;
        }
        client.end();
      }
    }
    httpData="";
  }
  WiFi.disconnect();
}

void out(void){
  if(html.os==1){
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
          outside.temp     =root["temp"];
          outside.pres     =root["press"];
          outside.humidity =root["hum"];
          outside.bat      =root["bat"];
          outside.updated  =root["updated"];
          outside.tempi    =root["tempi"];
          outside.presi    =root["pressi"];
          outside.humidityi=root["humi"];
          outside.tempe    =root["tempe"];
        }
        client.end();
      }
    }
  }
  if(html.os==2){
    String url="http://api.thingspeak.com/channels/";
    url+=html.chid;
    url+="/feeds.json?results=1";
    HTTPClient client;
    client.begin(url);
    int httpCode=client.GET();
    if(httpCode>0){
      if(httpCode==HTTP_CODE_OK){
        httpData=client.getString();
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root=jsonBuffer.parseObject(httpData);
        if(root.success()){
          outside.temp      = root["feeds"][0]["field1"];
          outside.pres      = root["feeds"][0]["field3"];
          outside.humidity  = root["feeds"][0]["field2"];
          outside.bat       = root["feeds"][0]["field5"];
          outside.tempi     = root["feeds"][0]["field6"];
          outside.presi     = root["feeds"][0]["field8"];
          outside.humidityi = root["feeds"][0]["field7"];
          String updated    = root["feeds"][0]["created_at"];
          char buf[21];
          updated.toCharArray(buf,21);
          int ye=atoi(strtok(buf,"-T:"));
          int mo=atoi(strtok(NULL,"-T:"));
          int da=atoi(strtok(NULL,"-T:"));
          int hr=atoi(strtok(NULL,"-T:"));
          int mn=atoi(strtok(NULL,"-T:"));
          int sc=atoi(strtok(NULL,"-T:"));
          int a=(14-mo)/12;
          int y=ye-a;
          int m=mo+12*a-2;
          int R=7000+(da+y+y/4-y/100+y/400+(31*m)/12);
          int wd=R%7;
          tmElements_t updTime={sc,mn,hr,wd,da,mo,ye-1970};
          outside.updated=makeTime(updTime);
        }
        client.end();
      }
    }
  }
  httpData="";
}

void is_settings(void){
  if(!digitalRead(BUTTON)){
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    if(ip.fromString(html.ap_ip) and gateway.fromString(html.ap_ip) and subnet.fromString(html.ap_mask)){
      WiFi.softAPConfig(ip,gateway,subnet);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(html.ap_ssid,html.ap_pass,html.chnl,html.hide);
    String IP=WiFi.localIP().toString();
    if(IP=="0.0.0.0") WiFi.disconnect();
    analogWrite(BACKLIGHT,512);
    showSettingsMode();
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
}

void sensors_init(void){
    //BME280
  bme1.parameter.communication=0;
  bme2.parameter.communication=0;
  bme1.parameter.I2CAddress=0x77;
  bme2.parameter.I2CAddress=0x76;
  bme1.parameter.sensorMode=0b11;
  bme2.parameter.sensorMode=0b11;
  bme1.parameter.IIRfilter=0b100;
  bme2.parameter.IIRfilter=0b100;
  bme1.parameter.humidOversampling=0b101;
  bme2.parameter.humidOversampling=0b101;
  bme1.parameter.tempOversampling=0b101;
  bme2.parameter.tempOversampling=0b101;
  bme1.parameter.pressOversampling=0b101;
  bme2.parameter.pressOversampling=0b101;
  bme1.parameter.pressureSeaLevel=1013.25;
  bme2.parameter.pressureSeaLevel=1013.25;
  bme1.parameter.tempOutsideCelsius=15;
  bme2.parameter.tempOutsideCelsius=15;
  bme1.parameter.tempOutsideFahrenheit=59;
  bme2.parameter.tempOutsideFahrenheit=59;
  if(bme1.init()==0x60) bme1Detected=true;
  if(bme2.init()==0x60) bme2Detected=true;
    //DHT22
  if((html.temp==3) or (html.hum==2)){
    dht.begin();
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    dht.humidity().getSensor(&sensor);
  }  
    //DS18B20
  sensors.begin();
  dsDetected=sensors.getDeviceCount();
  if(dsDetected){
    sensors.getAddress(thermometer1,0);
    sensors.getAddress(thermometer2,1);
    sensors.setResolution(thermometer1,10);
    sensors.setResolution(thermometer2,10);
    sensors.requestTemperatures();
  }
    //SHT21;
  SHT21.begin();
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(0xE7);
  Wire.endTransmission();
  delay(100);
  Wire.requestFrom(SHT21_ADDRESS,1);
  if(Wire.available()==1){
    Wire.read();
    shtDetected=true;
  }
}

float get_temp(bool units){
  float temp=404;
  if(html.temp==1) if(bme1Detected) temp=units?bme1.readTempC():bme1.readTempF();
  if(html.temp==2) if(bme2Detected) temp=units?bme2.readTempC():bme2.readTempF();  
  if(html.temp==3) if(dsDetected) temp=units?sensors.getTempC(thermometer1):sensors.getTempF(thermometer1);
  if(html.temp==4) if(dsDetected) temp=units?sensors.getTempC(thermometer2):sensors.getTempF(thermometer2);
  if(html.temp==5){
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature));
    else temp=units?event.temperature:event.temperature*1.8+32;
  }
  if(html.temp==6) if(shtDetected) temp=units?SHT21.getTemperature():SHT21.getTemperature()*1.8+32;
  if(html.temp==100) temp=units?outside.tempi:outside.tempi*1.8+32;
  if(html.temp==101) temp=units?outside.temp:outside.temp*1.8+32;
  if(html.temp==102) temp=units?outside.tempe:outside.tempe*1.8+32;
  if(html.temp==3 or html.temp==4 or html.t_out==6 or html.t_out==7) sensors.requestTemperatures();
  return temp+html.t_cor;
}

float get_humidity(void){
  float hum=404;
  if(html.hum==1) if(bme1Detected){bme1.readTempC(); hum=bme1.readHumidity();}
  if(html.hum==2) if(bme2Detected){bme2.readTempC(); hum=bme2.readHumidity();}
  if(html.hum==3){
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity));
    else hum=event.relative_humidity;
  }
  if(html.hum==4) if(shtDetected){SHT21.getTemperature(); hum=SHT21.getHumidity();}
  if(html.hum==100) hum=outside.humidityi;
  if(html.hum==101) hum=outside.humidity;
  return hum+html.h_cor;
}

float get_temp_out(void){
  float temp=404;
  if(html.t_out==0) temp=html.to_units?weather.temp*1.8+32:weather.temp;
  if(html.t_out==1) temp=html.to_units?outside.tempi*1.8+32:outside.tempi;
  if(html.t_out==2) temp=html.to_units?outside.temp*1.8+32:outside.temp;
  if(html.t_out==3) temp=html.to_units?outside.tempe*1.8+32:outside.tempe;
  if(html.t_out==4) if(bme1Detected) temp=html.to_units?bme1.readTempF():bme1.readTempC();
  if(html.t_out==5) if(bme2Detected) temp=html.to_units?bme2.readTempF():bme2.readTempC();  
  if(html.t_out==6) if(dsDetected) temp=html.to_units?sensors.getTempF(thermometer1):sensors.getTempC(thermometer1);
  if(html.t_out==7) if(dsDetected) temp=html.to_units?sensors.getTempF(thermometer2):sensors.getTempC(thermometer2);
  if(html.t_out==8){
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature));
    else temp=html.to_units?event.temperature*1.8+32:event.temperature;
  }
  if(html.t_out==9) if(shtDetected) temp=html.to_units?SHT21.getTemperature()*1.8+32:SHT21.getTemperature();
  return temp+html.to_cor;
}

float get_humidity_out(void){
  float hum=404;
  if(html.h_out==0) hum=weather.humidity;
  if(html.h_out==1) hum=outside.humidityi;
  if(html.h_out==2) hum=outside.humidity;
  if(html.h_out==3) if(bme1Detected){bme1.readTempC(); hum=bme1.readHumidity();}
  if(html.h_out==4) if(bme2Detected){bme2.readTempC(); hum=bme2.readHumidity();}
  if(html.h_out==5){
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity));
    else hum=event.relative_humidity;
  }
  if(html.h_out==6) if(shtDetected){SHT21.getTemperature(); hum=SHT21.getHumidity();}
  return hum+html.ho_cor;
}

float get_pres_out(void){
  float pres=4040;
  if(html.p_out==0) pres=weather.pressure;
  if(html.p_out==1) pres=outside.presi;
  if(html.p_out==2) pres=outside.pres;
  if(html.p_out==3) if(bme1Detected){bme1.readTempC(); pres=bme1.readPressure();}
  if(html.p_out==4) if(bme2Detected){bme2.readTempC(); pres=bme2.readPressure();}
  return pres+html.po_cor;
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
      String ap_ssid=root["APSSID"];  
      String ap_pass=root["APPASS"];
      String ap_ip  =root["APIP"];
      String ap_mask=root["APMASK"];
      html.chnl     =root["CHNL"];
      html.hide     =root["HIDE"];
      String city   =root["CITY"];
      html.city_id  =root["CITY_ID"];
      String cid    =root["CID"];
      String lat    =root["LAT"];
      String lon    =root["LON"];
      String appid  =root["APPID"];
      html.zone     =root["ZONE"];
      html.bright   =root["BRIGHT"];
      html.bright_n =root["BRIGHT_N"];
      html.adj      =root["DAYLIGHT"];
      html.timef    =root["TIME"];
      html.lang     =root["LANG"];
      html.sleep    =root["SLEEP"];
      html.typ      =root["TYPE"];
      html.k        =root["K"];
      html.temp     =root["TEMP"];
      html.hum      =root["HUM"];
      html.t_cor    =root["T_COR"];
      html.h_cor    =root["H_COR"];
      html.provider =root["PROVIDER"];
      html.battery  =root["BATTERY"];
      html.os       =root["OS"];
      html.ti_units =root["TI_UNITS"];
      html.ti_round =root["TI_ROUND"];
      html.hi_round =root["HI_ROUND"];
      html.t_out    =root["T_OUT"];
      html.to_units =root["TO_UNITS"];
      html.to_round =root["TO_ROUND"];
      html.h_out    =root["H_OUT"];
      html.p_out    =root["P_OUT"];
      html.po_units =root["PO_UNITS"];
      html.w_units  =root["W_UNITS"];
      html.w_round  =root["W_ROUND"];
      html.ac       =root["AC"];
      html.to_cor   =root["TO_COR"];
      html.ho_cor   =root["HO_COR"];
      html.po_cor   =root["PO_COR"];
      String ip     =root["IP"];
      String mask   =root["MASK"];
      String gw     =root["GATEWAY"];
      String dns1   =root["DNS1"];
      String dns2   =root["DNS2"];
      String mac    =root["MAC"];
      String sssid  =root["SSSID"];
      String spass  =root["SPASS"];
      String sip    =root["SIP"];
      String mdns   =root["MDNS"];
      String chid   =root["CHID"];
      html.bgr      =root["BGSR"];
      html.bgg      =root["BGSG"];
      html.bgb      =root["BGSB"];
      html.txr      =root["TXSR"];
      html.txg      =root["TXSG"];
      html.txb      =root["TXSB"];
      html.rmr      =root["RMSR"];
      html.rmg      =root["RMSG"];
      html.rmb      =root["RMSB"];
      html.snr      =root["SNSR"];
      html.sng      =root["SNSG"];
      html.snb      =root["SNSB"];
      String upd_icn=root["UPD_ICN"];
      html.dl       =root["DL"];
      html.fh       =root["FH"];
      html.fm       =root["FM"];
      html.th       =root["TH"];
      html.tm       =root["TM"];
      html.upd_icn  =upd_icn;
      html.sensor=mac;
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
      html.dns1=dns1;
      html.dns2=dns2;
      html.lat=lat;
      html.lon=lon;
      if(ap_ssid!="") ap_ssid.toCharArray(html.ap_ssid,(ap_ssid.length())+1);
      if(ap_pass!="") ap_pass.toCharArray(html.ap_pass,(ap_pass.length())+1);
      if(ap_ip!="") html.ap_ip=ap_ip;
      if(ap_mask!="") html.ap_mask=ap_mask;
      html.city=city;
      html.appid=appid;
      html.sssid=sssid;
      html.spass=spass;
      html.sip=sip;
      if(mdns!="") html.mdns=mdns;
      html.chid=chid;
      html.cid=cid;
    }
  }
  if(html.sleep>100) html.sleep=1;
  if(html.lang>9) html.lang=0;
  if(html.adj>1) html.adj=0;
  if((html.zone>13) and (html.zone<-13)) html.zone=0;
  if(html.timef>1) html.timef=0;
  if(html.bright>1024 or html.bright==0) html.bright=500;
  switch(html.lang){
    case 0:urlLang="en";break;
    case 1:urlLang="ru";break;
    case 2:urlLang="ro";break;
    case 3:urlLang="de";break;
    case 4:urlLang="lt";break;
    case 5:urlLang="et";break;
    case 6:urlLang="ua";break;
    case 7:urlLang="az";break;
    case 8:urlLang="by";break;
    case 9:urlLang="bg";break;
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
  back_color=rgb565(html.bgr,html.bgg,html.bgb);
  text_color=rgb565(html.txr,html.txg,html.txb);
  out_color=rgb565(html.snr,html.sng,html.snb);
  rama_color=rgb565(html.rmr,html.rmg,html.rmb); 
}

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue){
  uint16_t b=(blue>>3)&0x1f;
  uint16_t g=((green>>2)&0x3f)<<5;
  uint16_t r=((red>>3)&0x1f)<<11;
  return (uint16_t)(r|g|b);
}

uint32_t calculateCRC32(const uint8_t *data,size_t length){
  uint32_t crc=0xffffffff;
  while(length--){
    uint8_t c=*data++;
    for(uint32_t i=0x80;i>0;i>>=1){
      bool bit=crc&0x80000000;
      if(c&i){
        bit=!bit;
      }
      crc<<=1;
      if(bit){
        crc^=0x04c11db7;
      }
    }
  }
  return crc;
}

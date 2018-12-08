/* Weather Monitor BIM v3.8
 * © Alexandr Piteli himikat123@gmail.com, Chisinau, Moldova, 2016-2018 
 * http://esp8266.atwebpages.com
 */
                               // Board ESP-12E 
                               // 1MB (512kB SPIFFS) 
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
DeviceAddress insideThermometer;
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
    showWeatherNow();
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
    //i2c
  Wire.pins(ONE_WIRE_BUS,DHTPIN);
    //Sensors
  sensors_init();
    // Ticker
  updater.attach(1200,updateWeather);
  handler.attach(5,hndlr);
  if(html.sleep==0) sleeper.attach(1201,goSleep);
  else sleeper.attach(html.sleep*60,goSleep);
  ap.attach(60,apTime);
    //if settings button is pressed
  is_settings();
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
      myGLCD.setColor(VGA_WHITE);
      myGLCD.setBackColor(back_color);
      myGLCD.setFont(SmallFontRu);
      sprintf(text_buf,"%s",UTF8(status_lng[html.lang].unable_to_connect_to));
      text_buf[35]='\0';
      myGLCD.print(text_buf,2,2);
      connectToWiFi();
    }
  }
  else{
    out_bat();
    is_settings();
    showInsideTemp();
    rssi=viewRSSI(String(WiFi.SSID()));
    showBatteryLevel();
    showTime();
    showWiFiLevel(rssi);
    if(weather.isDay) analogWrite(BACKLIGHT,html.bright);
    else analogWrite(BACKLIGHT,html.bright_n);
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
    }
    handle_flag=true;
    webServer.handleClient();
    yield();
  }
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

void update_weather(void){
  if(html.os==0){
    sensor();
    connectToWiFi();
  }
  if(html.os>0) out();
  siteTime();
  getWeatherNow();
  getWeatherDaily();
  showTime();
  showInsideTemp();
  showWeatherNow();
  showWeatherToday();
  showWeatherTomorrow();
  showWeatherAfterTomorrow();
  database();  
  yield();
}

void connectToWiFi(void){
  if(html.battery==0) myGLCD.drawBitmap(273,0,16,16,nowifi,1);
  if(html.battery==1) myGLCD.drawBitmap(265,0,16,16,nowifi,1);
  if(html.battery==2) myGLCD.drawBitmap(290,0,16,16,nowifi,1);
  is_settings();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(back_color);
  myGLCD.setFont(SmallFontRu);
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
      yield();
    }
  }
  else{
    if(WiFi.SSID()==html.sssid) WiFi.disconnect();
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
              myGLCD.setColor(back_color);
              myGLCD.fillRect(0,0,319,3);
              myGLCD.setColor(VGA_WHITE);
              myGLCD.print(text_buf,2,4);
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
            myGLCD.setColor(back_color);
            myGLCD.fillRect(0,0,319,3);
            myGLCD.setColor(VGA_WHITE);
            myGLCD.print(text_buf,2,4);
            if(WiFi.status()==WL_CONNECTED) goto connectedd;
          }
          sprintf(text_buf,"%-30s",UTF8(status_lng[html.lang].unable_to_connect_to));
          text_buf[30]='\0';
          myGLCD.setColor(back_color);
          myGLCD.fillRect(0,0,319,3);
          myGLCD.setColor(VGA_WHITE);
          myGLCD.print(text_buf,2,4);

          showSettingsMode();
          WiFi.mode(WIFI_AP_STA);
          WiFi.softAP(html.ap_ssid,html.ap_pass);
          String IP=WiFi.localIP().toString();
          if(IP=="0.0.0.0") WiFi.disconnect();
          web_settings();
          while(ap_flag){
            webServer.handleClient();
            yield();
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
  sprintf(text_buf,"%-30s",text_buf);
  text_buf[30]='\0';
  myGLCD.setColor(back_color);
  myGLCD.fillRect(0,0,319,3);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.print(text_buf,2,4);
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
  url+="&COUNTRY="; country.replace(" ",""); url+=country;
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
      myGLCD.print(UTF8(status_lng[html.lang].connecting_sensor),2,2);
      uint8_t e=0;
      while(WiFi.status()!=WL_CONNECTED){
        if((e++)>20){
          myGLCD.print(UTF8(status_lng[html.lang].unable_connect_sensor),2,2);
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
      yield();
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
    sensors.getAddress(insideThermometer,0);
    sensors.setResolution(insideThermometer,10);
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
  if(html.temp==1){
    if(bme1Detected) temp=units?bme1.readTempC():bme1.readTempF();
    if(bme2Detected) temp=units?bme2.readTempC():bme2.readTempF();  
  }
  if(html.temp==2){
    if(dsDetected) temp=units?sensors.getTempC(insideThermometer):sensors.getTempF(insideThermometer);
    sensors.requestTemperatures();
  }
  if(html.temp==3){
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature));
    else temp=units?event.temperature:event.temperature*1.8+32;
  }
  if(html.temp==4) if(shtDetected) temp=units?SHT21.getTemperature():SHT21.getTemperature()*1.8+32;
  if(html.temp==100) temp=units?outside.tempi:outside.tempi*1.8+32;
  if(html.temp==101) temp=units?outside.temp:outside.temp*1.8+32;
  if(html.temp==102) temp=units?outside.tempe:outside.tempe*1.8+32;
  return temp+html.t_cor;
}

float get_humidity(void){
  float hum=404;
  if(html.hum==1){
    if(bme1Detected){bme1.readTempC(); hum=bme1.readHumidity();}
    if(bme2Detected){bme2.readTempC(); hum=bme2.readHumidity();}
  }
  if(html.hum==2){
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity));
    else hum=event.relative_humidity;
  }
  if(html.hum==3) if(shtDetected){SHT21.getTemperature(); hum=SHT21.getHumidity();}
  if(html.hum==100) hum=outside.humidityi;
  if(html.hum==101) hum=outside.humidity;
  return hum+html.h_cor;
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
      html.sensor=mac;
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
      html.dns1=dns1;
      html.dns2=dns2;
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

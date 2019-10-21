/* Weather Monitor BIM v4.1
 * © himikat123@gmail.com, Nürnberg, Deutschland, 2016-2019 
 * https://github.com/himikat123/Weather-Monitor-BIM
 * http://b-i-m.online
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
  if(!config.ac){
    float adc=analogRead(A0);
    float cor=-config.k;
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
      analogWrite(BACKLIGHT,config.bright);
      delay(1000);
      is_settings();
    }
    else ESP.rtcUserMemoryRead(0,(uint32_t*)&weather,sizeof(weather));
    analogWrite(BACKLIGHT,config.bright);
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
  if(WiFi.status()!=WL_CONNECTED) connectToWiFi();
    //NTP
  ntp=ntpClient::getInstance("time.windows.com",1);
  ntp->setInterval(15,1800);
  ntp->setTimeZone(config.zone);
  ntp->setDayLight(config.adj);
  ntp->begin();
    // Ticker
  updater.attach(1200,updateWeather);
  handler.attach(5,hndlr);
  if(config.sleep==0) sleeper.attach(1201,goSleep);
  else sleeper.attach(config.sleep*60,goSleep);
  ap.attach(60,apTime);
  rn.attach(1,rn_str_plus);
    //mDNS
  config.mdns.toCharArray(text_buf,(config.mdns.length())+1);
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
      if(weather.isDay) analogWrite(BACKLIGHT,config.bright);
      else analogWrite(BACKLIGHT,config.bright_n);
      showWiFiLevel(rssi);
    }
    else{
      printCent(UTF8(status_lng[config.lang].unable_to_connect_to),23,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
      connectToWiFi();
      is_settings();
      showInsideTemp();
      showBatteryLevel();
      showTime();
    }
  }
  else{
    if(config.dl!=0){
      tmElements_t from={0,config.fm,config.fh,weekday(),day(),month(),year()-1970};
      uint32_t fromT=makeTime(from);
      tmElements_t to={0,config.tm,config.th,weekday(),day(),month(),year()-1970};
      uint32_t toT=makeTime(to);
      if(fromT<=now() and now()<=toT) weather.isDay=true;
      else weather.isDay=false;
    }
    if(weather.isDay) analogWrite(BACKLIGHT,config.bright);
    else analogWrite(BACKLIGHT,config.bright_n);
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
      String str=UTF8(WeatherNow[config.lang].srise); str+=" ";
      str+=hour(weather.sunrise); str+=":";
      if(minute(weather.sunrise)<10) str+="0";
      str+=minute(weather.sunrise);
      printCent(str,6,211,156,text_color,back_color,SmallFontRu);
    }
    if(r_str==2){
      String str=UTF8(WeatherNow[config.lang].sset); str+=" ";
      str+=hour(weather.sunset); str+=":";
      if(minute(weather.sunset)<10) str+="0";
      str+=minute(weather.sunset);
      printCent(str,6,211,156,text_color,back_color,SmallFontRu);
    }
    if(config.sleep and sleep_flag){
      sleep_flag=false;
      if(weather.isDay) analogWrite(BACKLIGHT,config.bright/3);
      else analogWrite(BACKLIGHT,config.bright_n/3);
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
  if(config.os==0){
    sensor();
    connectToWiFi();
  }
  if(config.os>0) out();
  getWeatherNow();
  getWeatherDaily();
  showTime();
  showInsideTemp();
  showWeatherNow(true);
  showWeatherToday();
  showWeatherTomorrow();
  showWeatherAfterTomorrow();
}

void connectToWiFi(void){
  myGLCD.setColor(text_color);
  sprintf(text_buf,"%c",ANT100);
  myGLCD.setFont(Symbols);
  sprintf(text_buf,"%c",ANT100);
  if(config.ac){
    myGLCD.print(text_buf,290,0);
    myGLCD.setColor(VGA_RED);
    myGLCD.drawLine(290,0,305,15);
    myGLCD.drawLine(290,15,305,0);
  }
  else{
    if(config.battery==0){
      myGLCD.print(text_buf,273,0);
      myGLCD.setColor(VGA_RED);
      myGLCD.drawLine(273,0,288,15);
      myGLCD.drawLine(273,15,288,0);
    }
    if(config.battery==1){
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
    if(ip.fromString(config.ap_ip) and gateway.fromString(config.ap_ip) and subnet.fromString(config.ap_mask)){
      WiFi.softAPConfig(ip,gateway,subnet);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(config.ap_ssid,config.ap_pass,config.chnl,config.hide);
    String IP=WiFi.localIP().toString();
    if(IP=="0.0.0.0") WiFi.disconnect(); 
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
  else{
    if(WiFi.SSID()==config.sssid) WiFi.disconnect();
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
              sprintf(text_buf,"%s %s",UTF8(status_lng[config.lang].connecting_to),ssid);
              text_buf[30]='\0';
              printCent(text_buf,0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
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
            sprintf(text_buf,"%s %s",UTF8(status_lng[config.lang].connecting_to),ssid);
            text_buf[30]='\0';
            printCent(text_buf,0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
            if(WiFi.status()==WL_CONNECTED) goto connectedd;
          }
          printCent(UTF8(status_lng[config.lang].unable_to_connect_to),0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
          showSettingsMode();
          WiFi.mode(WIFI_AP_STA);
          WiFi.softAP(config.ap_ssid,config.ap_pass);
          String IP=WiFi.localIP().toString();
          if(IP=="0.0.0.0") WiFi.disconnect();
          web_settings();
          while(ap_flag){
            webServer.handleClient();
          }
          if(config.sleep==0) ESP.reset();
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
    if(config.typ==1){
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
        WiFi.config(ip,gateway,subnet,dns1,dns2);
      }
    }
    rssi=viewRSSI(String(WiFi.SSID())); 
  }
  WiFi.SSID().toCharArray(ssid,(WiFi.SSID().length())+1);
  sprintf(text_buf,"%s %s",UTF8(status_lng[config.lang].connected_to),ssid);
  text_buf[30]='\0';
  printCent(text_buf,0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
  
  config.mdns.toCharArray(text_buf,(config.mdns.length())+1);
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

void sensor(void){
  myGLCD.setFont(SmallFontRu);
  if(WiFi.SSID()!=config.sssid){
    uint8_t n=WiFi.scanNetworks();
    bool o=false;
    for(uint8_t i=0;i<n;i++){
      if(WiFi.SSID(i)==config.sssid) o=true;
    }
    if(o){
      WiFi.disconnect();
      config.sssid.toCharArray(ssid,(config.sssid.length())+1);
      config.spass.toCharArray(password,(config.spass.length())+1);
      WiFi.begin(ssid,password);
      printCent(UTF8(status_lng[config.lang].connecting_sensor),23,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
      uint8_t e=0;
      while(WiFi.status()!=WL_CONNECTED){
        if((e++)>20){
          printCent(UTF8(status_lng[config.lang].unable_connect_sensor),23,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,2,text_color,back_color,SmallFontRu);
          delay(5000);
          break;
        }
        delay(500);
      }
    }
  }  
  if(WiFi.status()==WL_CONNECTED){
    String url="http://";
    url+=config.sip;
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
          outside.updated=now()-(config.zone*3600)-dayLight;
        }
        client.end();
      }
    }
    httpData="";
  }
  WiFi.disconnect();
}

void out(void){
  if(config.os==2){
    String url="http://api.thingspeak.com/channels/";
    url+=config.chid;
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
    if(ip.fromString(config.ap_ip) and gateway.fromString(config.ap_ip) and subnet.fromString(config.ap_mask)){
      WiFi.softAPConfig(ip,gateway,subnet);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(config.ap_ssid,config.ap_pass,config.chnl,config.hide);
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
  if((config.temp==5) or (config.hum==3)){
    float temp=0.0,hum=0.0;
    dht.begin();
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    dht.humidity().getSensor(&sensor);
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature));
    else temp=event.temperature;
    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity));
    else hum=event.relative_humidity;
    if(temp!=0.0 and hum>0.0) dhtDetected=true;
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
  if(config.temp==1) if(bme1Detected) temp=units?bme1.readTempC():bme1.readTempF();
  if(config.temp==2) if(bme2Detected) temp=units?bme2.readTempC():bme2.readTempF();  
  if(config.temp==3) if(dsDetected) temp=units?sensors.getTempC(thermometer1):sensors.getTempF(thermometer1);
  if(config.temp==4) if(dsDetected) temp=units?sensors.getTempC(thermometer2):sensors.getTempF(thermometer2);
  if(config.temp==5){
    if(dhtDetected){
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if(isnan(event.temperature));
      else temp=units?event.temperature:event.temperature*1.8+32;
    }
  }
  if(config.temp==6) if(shtDetected) temp=units?SHT21.getTemperature():SHT21.getTemperature()*1.8+32;
  if(config.temp==100) temp=units?outside.tempi:outside.tempi*1.8+32;
  if(config.temp==101) temp=units?outside.temp:outside.temp*1.8+32;
  if(config.temp==102) temp=units?outside.tempe:outside.tempe*1.8+32;
  if(config.temp==3 or config.temp==4 or config.t_out==6 or config.t_out==7) sensors.requestTemperatures();
  return temp+config.t_cor;
}

float get_humidity(void){
  float hum=404;
  if(config.hum==1) if(bme1Detected){bme1.readTempC(); hum=bme1.readHumidity();}
  if(config.hum==2) if(bme2Detected){bme2.readTempC(); hum=bme2.readHumidity();}
  if(config.hum==3){
    if(dhtDetected){
      sensors_event_t event;
      dht.humidity().getEvent(&event);
      if(isnan(event.relative_humidity));
      else hum=event.relative_humidity;
    }
  }
  if(config.hum==4) if(shtDetected){SHT21.getTemperature(); hum=SHT21.getHumidity();}
  if(config.hum==100) hum=outside.humidityi;
  if(config.hum==101) hum=outside.humidity;
  return hum+config.h_cor;
}

float get_temp_out(void){
  float temp=404;
  if(config.t_out==0) temp=config.to_units?weather.temp*1.8+32:weather.temp;
  if(config.t_out==1) temp=config.to_units?outside.tempi*1.8+32:outside.tempi;
  if(config.t_out==2) temp=config.to_units?outside.temp*1.8+32:outside.temp;
  if(config.t_out==3) temp=config.to_units?outside.tempe*1.8+32:outside.tempe;
  if(config.t_out==4) if(bme1Detected) temp=config.to_units?bme1.readTempF():bme1.readTempC();
  if(config.t_out==5) if(bme2Detected) temp=config.to_units?bme2.readTempF():bme2.readTempC();  
  if(config.t_out==6) if(dsDetected) temp=config.to_units?sensors.getTempF(thermometer1):sensors.getTempC(thermometer1);
  if(config.t_out==7) if(dsDetected) temp=config.to_units?sensors.getTempF(thermometer2):sensors.getTempC(thermometer2);
  if(config.t_out==8){
    if(dhtDetected){
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if(isnan(event.temperature));
      else temp=config.to_units?event.temperature*1.8+32:event.temperature;
    }
  }
  if(config.t_out==9) if(shtDetected) temp=config.to_units?SHT21.getTemperature()*1.8+32:SHT21.getTemperature();
  return temp+config.to_cor;
}

float get_humidity_out(void){
  float hum=404;
  if(config.h_out==0) hum=weather.humidity;
  if(config.h_out==1) hum=outside.humidityi;
  if(config.h_out==2) hum=outside.humidity;
  if(config.h_out==3) if(bme1Detected){bme1.readTempC(); hum=bme1.readHumidity();}
  if(config.h_out==4) if(bme2Detected){bme2.readTempC(); hum=bme2.readHumidity();}
  if(config.h_out==5){
    if(dhtDetected){
      sensors_event_t event;
      dht.humidity().getEvent(&event);
      if(isnan(event.relative_humidity));
      else hum=event.relative_humidity;
    }
  }
  if(config.h_out==6) if(shtDetected){SHT21.getTemperature(); hum=SHT21.getHumidity();}
  return hum+config.ho_cor;
}

float get_pres_out(void){
  float pres=4040;
  if(config.p_out==0) pres=weather.pressure;
  if(config.p_out==1) pres=outside.presi;
  if(config.p_out==2) pres=outside.pres;
  if(config.p_out==3) if(bme1Detected){bme1.readTempC(); pres=bme1.readPressure();}
  if(config.p_out==4) if(bme2Detected){bme2.readTempC(); pres=bme2.readPressure();}
  return pres+config.po_cor;
}

void read_eeprom(void){
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
      config.chnl     =root["CHNL"];
      config.hide     =root["HIDE"];
      String city   =root["CITY"];
      config.city_id  =root["CITY_ID"];
      String cid    =root["CID"];
      String lat    =root["LAT"];
      String lon    =root["LON"];
      String appid  =root["APPID"];
      config.zone     =root["ZONE"];
      config.bright   =root["BRIGHT"];
      config.bright_n =root["BRIGHT_N"];
      config.adj      =root["DAYLIGHT"];
      config.timef    =root["TIME"];
      config.lang     =root["LANG"];
      config.sleep    =root["SLEEP"];
      config.typ      =root["TYPE"];
      config.k        =root["K"];
      config.temp     =root["TEMP"];
      config.hum      =root["HUM"];
      config.t_cor    =root["T_COR"];
      config.h_cor    =root["H_COR"];
      config.provider =root["PROVIDER"];
      config.battery  =root["BATTERY"];
      config.os       =root["OS"];
      config.ti_units =root["TI_UNITS"];
      config.ti_round =root["TI_ROUND"];
      config.hi_round =root["HI_ROUND"];
      config.t_out    =root["T_OUT"];
      config.to_units =root["TO_UNITS"];
      config.to_round =root["TO_ROUND"];
      config.h_out    =root["H_OUT"];
      config.p_out    =root["P_OUT"];
      config.po_units =root["PO_UNITS"];
      config.w_units  =root["W_UNITS"];
      config.w_round  =root["W_ROUND"];
      config.ac       =root["AC"];
      config.to_cor   =root["TO_COR"];
      config.ho_cor   =root["HO_COR"];
      config.po_cor   =root["PO_COR"];
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
      config.bgr      =root["BGSR"];
      config.bgg      =root["BGSG"];
      config.bgb      =root["BGSB"];
      config.txr      =root["TXSR"];
      config.txg      =root["TXSG"];
      config.txb      =root["TXSB"];
      config.rmr      =root["RMSR"];
      config.rmg      =root["RMSG"];
      config.rmb      =root["RMSB"];
      config.snr      =root["SNSR"];
      config.sng      =root["SNSG"];
      config.snb      =root["SNSB"];
      config.dl       =root["DL"];
      config.fh       =root["FH"];
      config.fm       =root["FM"];
      config.th       =root["TH"];
      config.tm       =root["TM"];
      config.sensor=mac;
      config.ip=ip;
      config.mask=mask;
      config.gateway=gw;
      config.dns1=dns1;
      config.dns2=dns2;
      config.lat=lat;
      config.lon=lon;
      if(ap_ssid!="") ap_ssid.toCharArray(config.ap_ssid,(ap_ssid.length())+1);
      if(ap_pass!="") ap_pass.toCharArray(config.ap_pass,(ap_pass.length())+1);
      if(ap_ip!="") config.ap_ip=ap_ip;
      if(ap_mask!="") config.ap_mask=ap_mask;
      config.city=city;
      config.appid=appid;
      config.sssid=sssid;
      config.spass=spass;
      config.sip=sip;
      if(mdns!="") config.mdns=mdns;
      config.chid=chid;
      config.cid=cid;
    }
  }
  if(config.sleep>100) config.sleep=1;
  if(config.lang>9) config.lang=0;
  if(config.adj>1) config.adj=0;
  if((config.zone>13) and (config.zone<-13)) config.zone=0;
  if(config.timef>1) config.timef=0;
  if(config.bright>1024 or config.bright==0) config.bright=500;
  switch(config.lang){
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
  back_color=rgb565(config.bgr,config.bgg,config.bgb);
  text_color=rgb565(config.txr,config.txg,config.txb);
  out_color=rgb565(config.snr,config.sng,config.snb);
  rama_color=rgb565(config.rmr,config.rmg,config.rmb); 
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

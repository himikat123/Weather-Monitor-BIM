/* Outside sensor v1.1
   © Alexandr Piteli himikat123@gmail.com, Chisinau, Moldova, 2017
   http://esp8266.atwebpages.com
*/
//                                512kB SPIFFS 128kB
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
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Sodaq_BMP085.h>

extern "C" {
#include "main.h"
#include "languages.h"
}

#define site "http://esp8266.atwebpages.com/api/"
#define BUTTON         0
#define R              12
#define G              13
#define B              14
#define ONE_WIRE_BUS   2
#define DHTPIN         0
#define DHTTYPE        DHT22

ESP8266WebServer webServer(80);
File fsUploadFile;
BME280 mySensor;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress outsideThermometer;
DHT_Unified dht(DHTPIN, DHTTYPE);
Sodaq_BMP085 bmp;

void setup(){
    //PINs
  pinMode(BUTTON,INPUT);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);
  led(1,1,1);
    //Serial port
  Serial.begin(115200);
  while (!Serial);
    //mac address
  MacAddr=WiFi.macAddress();
  Serial.print("MAC address ");Serial.println(MacAddr);
    //SPIFS
  if (!SPIFFS.begin()) while (1) yield();
    //EEPROM
  Serial.println("read settings");
  read_eeprom();
  Serial.println("----------------------------------");
    //WIFI MODE
  WiFi.mode(WIFI_STA);
    //sensor
  sensors_init();
    //battery
  Serial.println("read battery level");
  bat_level=BatteryLevel();
  if(bat_level==1) led(1,0,0);
  else led(0,1,0);
  Serial.println("-----------------------------------");
    //WiFi
  connectToWiFi();
}

void loop(){
  is_settings();
  if(WiFi.status()==WL_CONNECTED){
    led(0,0,1);
    getCoordinates();
    Serial.print("network "); Serial.print(WiFi.SSID()); Serial.println(" is connected");
    Serial.println("---------------------------");
    Serial.print("Latitude "); Serial.println(latitude);
    Serial.print("Longitude "); Serial.println(longitude);
    Serial.print("Altitude "); Serial.println(altitude);
    float temp=get_temp(1);
    url=site;
    url+="sensor.php?MAC=";
    url+=MacAddr;
    url+="&LAT=";
    url+=latitude;
    url+="&LON=";
    url+=longitude;
    url+="&ALT=";
    url+=altitude;
    url+="&TEMP=";
    url+=temp;
    url+="&PRES=";
    url+= get_pres();
    url+="&HUM=";
    url+=get_humidity();
    url+="&BAT=";
    url+=bat_level;
    url+="&FW=";
    url+=fw;
    url+="&KEY=";
    url+=sha1(MacAddr);
    url+=temp;
    HTTPClient client;
    client.begin(url);
    Serial.println(url);
    int httpCode=client.GET();
    httpData=client.getString();
    Serial.print("Server returned: ");
    Serial.println(httpData);
    client.end();
    httpData="";
    is_settings();
    //delay(1000);
    Serial.println("going sleep");
    led(0,0,0);
    mySensor.settings.runMode=0;
    ESP.deepSleep(1200000000);
  }
  else{
    Serial.print("network "); Serial.println(" was not found");
    is_settings();
    Serial.println("going sleep");
    led(0,0,0);
    mySensor.settings.runMode=0;
    ESP.deepSleep(120000000);
  }
}

void connectToWiFi(void){
  is_settings();
  if(!ssids.num){
    Serial.print("Creating network \""); Serial.print(html.ap_ssid);
    Serial.print("\" with password \""); Serial.print(html.ap_pass); Serial.println("\"");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(html.ap_ssid,html.ap_pass);
    led(1,0,1);
    web_settings();
    while(1){
      webServer.handleClient();
    }
  }
  else{
    if(WiFi.status()!=WL_CONNECTED){
      uint8_t n=WiFi.scanNetworks();
      if(n!=0){
        for(uint8_t i=0;i<n;i++){
          for(uint8_t k=0;k<ssids.num;k++){
            delay(1);
            if(WiFi.SSID(i)==ssids.ssid[k]){
              rssi=WiFi.RSSI(i);
              ssids.ssid[k].toCharArray(ssid,(ssids.ssid[k].length())+1);
              ssids.pass[k].toCharArray(password,(ssids.pass[k].length())+1);
              WiFi.begin(ssid,password);
              Serial.print("Connecting to "); Serial.println(ssid);
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
            ssids.ssid[k].toCharArray(ssid,(ssids.ssid[k].length())+1);
            ssids.pass[k].toCharArray(password,(ssids.pass[k].length())+1);
            WiFi.begin(ssid,password);
            Serial.print("Connecting to "); Serial.println(ssid);
            if(WiFi.status()==WL_CONNECTED) goto connectedd;
          } 
          if(e&1) led(1,1,0);
          else{
            if(bat_level==1) led(1,0,0);
            else led(0,1,0);
          }
          Serial.print("Unable to connect to "); Serial.println(ssid);
          Serial.println("Going sleep");
          is_settings();
          led(0,0,0);
          //mySensor.settings.runMode=0;
          ESP.deepSleep(120000000);
        }
        if(e&1) led(1,1,0);
        else{
          if(bat_level==1) led(1,0,0);
          else led(0,1,0);
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
  Serial.print("Connected to "); Serial.println(ssid);
}

void sensors_init(void){
    //BMP180
  if((html.temp==2) or (html.pres==2)){
    Serial.println("Init BMP180");
    bmp.begin();
  }
    //BME280
  if((html.temp==1) or (html.pres==1) or (html.hum==1)){
    Serial.println("Init BME280");
    mySensor.settings.commInterface=I2C_MODE;
    mySensor.settings.I2CAddress=0x76;
    mySensor.settings.runMode=3;
    mySensor.settings.tStandby=0;
    mySensor.settings.filter=0;
    mySensor.settings.tempOverSample=1;
    mySensor.settings.pressOverSample=1;
    mySensor.settings.humidOverSample=1;
    mySensor.begin();
  }
    //DS18B20
  if(html.temp==3){
    Serial.println("Init DS18B20");
    sensors.begin();
    sensors.getAddress(outsideThermometer,0); 
    sensors.setResolution(outsideThermometer,12);
    sensors.requestTemperatures();
  }
    //DHT22
  if((html.temp==4) or (html.hum==2)){
    Serial.println("Init DHT22");
    dht.begin();
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    dht.humidity().getSensor(&sensor);
  }
}

float get_temp(bool units){
  float temp=0;
  if(units){
    if(html.temp==0) temp=404;
    if(html.temp==1) temp=mySensor.readTempC(),2;
    if(html.temp==2) temp=bmp.readTemperature();
    if(html.temp==3) temp=sensors.getTempC(outsideThermometer);
    if(html.temp==4){
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if(isnan(event.temperature));
      else temp=event.temperature;
    }  
  }
  else{
    if(html.temp==0) temp=404;
    if(html.temp==1) temp=mySensor.readTempF(),2;
    if(html.temp==2) temp=bmp.readTemperature()*1.8+32;
    if(html.temp==3) temp=sensors.getTempF(outsideThermometer);
    if(html.temp==4){
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if(isnan(event.temperature));
      else{temp=event.temperature; temp=temp*1.8+32;}
    }
  }
  Serial.print("Temperature: "); Serial.print(temp);
  if(units) Serial.println(" degrees C");
  else Serial.println(" degrees F");
  return temp;
}

int get_pres(void){
  int pres=0;
  if(html.pres==0) pres=4040;
  if(html.pres==1) pres=mySensor.readFloatPressure()/100;
  if(html.pres==2) pres=bmp.readPressure()/100;
  Serial.print("Pressure: "); Serial.print(pres); Serial.println(" Pa");
  Serial.print("Pressure: "); Serial.print(pres*0.75); Serial.println(" mmHg");
  return pres;
}

int get_humidity(void){
  int hum=0;
  if(html.hum==0) hum=404;
  if(html.hum==1) hum=mySensor.readFloatHumidity();
  if(html.hum==2){
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity));
    else hum=event.relative_humidity;
  }
  Serial.print("Humidity: "); Serial.print(hum); Serial.println("%");
  return hum;
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
//    WiFi.softAP(rtcData.AP_SSID,rtcData.AP_PASS);
//    Serial.printf("connect to %s, password is %s\r\n", rtcData.AP_SSID,rtcData.AP_PASS);
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

int viewRSSI(String ssid){
  uint8_t n=WiFi.scanNetworks();
  Serial.printf("found %d networks\r\n",n);
  int rssi=0;
  if(n!=0){
    for(uint8_t i=0;i<n;i++){
      if(WiFi.SSID(i)==ssid) rssi=WiFi.RSSI(i);
      Serial.print(WiFi.SSID(i)); Serial.print(" ");
      Serial.println(WiFi.RSSI(i));
    }  
  }
  return rssi;
}

void read_eeprom(void){
  String fData;
  File f=SPIFFS.open("/save/bat.json","r");
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
  Serial.printf("bat min=%d bat max=%d\r\n",battery.min,battery.max);  
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
      html.temp   =root["TEMP"];
      html.pres   =root["PRES"];
      html.hum    =root["HUM"];
      html.lang   =root["LANG"];
      html.typ    =root["TYPE"];
      String ip   =root["IP"];
      String mask =root["MASK"];
      String gw   =root["GATEWAY"];
      html.ip=ip;
      html.mask=mask;
      html.gateway=gw;
      ap_ssid.toCharArray(html.ap_ssid,(ap_ssid.length())+1);
      ap_pass.toCharArray(html.ap_pass,(ap_pass.length())+1);
    }
  }
  if(html.lang>3 or html.lang<0) html.lang=0;
  if(html.temp>3 or html.temp<0) html.temp=0;
  if(html.pres>1 or html.pres<0) html.pres=0;
  if(html.hum>1 or html.hum<0) html.hum=0;
  f=SPIFFS.open("/save/ssids.json","r");
  if(f){
    fData=f.readString();
    f.close();
    DynamicJsonBuffer jsonBuf;
    JsonObject& json=jsonBuf.parseObject(fData);
    if(json.success()){
      ssids.num=json["num"];
      Serial.printf("Saved %d networks\r\n",ssids.num);
      for(uint8_t i=0;i<ssids.num;i++){
        ssids.ssid[i]=json["nets"][i*2].as<String>();
        Serial.print("ssid "); Serial.print(ssids.ssid[i]);
        ssids.pass[i]=json["nets"][i*2+1].as<String>();
        Serial.print(" pass "); Serial.println(ssids.pass[i]);
      }
    }
  } 
  Serial.print("LANG="); Serial.println(html.lang);
  Serial.print("CONN TYPE="); Serial.println(html.typ);
  Serial.print("IP="); Serial.println(html.ip);
  Serial.print("MASK="); Serial.println(html.mask);
  Serial.print("GATEWAY="); Serial.println(html.gateway);
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

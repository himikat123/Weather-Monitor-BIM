extern unsigned int n[0xE1];
extern unsigned int s[0xE1];
extern unsigned int e[0xE1];
extern unsigned int w[0xE1];
extern unsigned int n_e[0xE1];
extern unsigned int n_w[0xE1];
extern unsigned int s_e[0xE1];
extern unsigned int s_w[0xE1];
extern unsigned int nowifi[0x0100];

String city;
String country;
String lang;
String fw="1.2";
String vers="Weather monitor v"+fw;
char descript[30];
uint8_t icon=1;
char cityName[32];
char text_buf[255];
float tempInside;
char ssid[32]="";
char password[32]="";
String servak;
String url;
int rssi;
ntpClient *ntp;
String httpData;
char* urlLang="en";
const char* host="settings";
const char* copy="© himikat123@gmail.com";
char DEFAULT_AP_SSID[15]="WeatherMonitor";
char DEFAULT_AP_PASS[11]="1234567890";

struct{
  uint32_t crc_ssid;
  uint32_t crc_pass;
  char AP_SSID[32];
  char AP_PASS[32];
} rtcData;

struct{
  int min=600;
  int max=620;
} battery;

struct{
  float temp;
  int bat;
  const char* mac;
  const char* updated;
} outside;

struct html_structure{
  String      ssid;
  String      pass;
  String      city;
  int8_t      zone=0;
  String      appid;
  uint8_t     adj=false;
  uint8_t     lang=0;
  char        id[32];
  uint8_t     units=0;
  uint8_t     pres=0;
  uint8_t     timef=0;
  int         bright=0;
  int         sleep=1;
};
html_structure html;

struct weather_structure{
  const char* latitude;
  const char* longitude;
  const char* city;
  const char* main;
  const char* icon;
  const char* descript;
  float temp;
  float pressure;
  byte  humidity;
  float speed;
  float deg;
  const char* country;

  const char* main1;
  const char* icon1;
  const char* descript1;
  float day1;
  float night1;
  float pressure1;
  byte  humidity1;
  float speed1;
  float deg1;

  const char* main2;
  const char* icon2;
  const char* descript2;
  float day2;
  float night2;
  float pressure2;
  byte  humidity2;
  float speed2;
  float deg2;

  const char* main3;
  const char* icon3;
  const char* descript3;
  float day3;
  float night3;
  float pressure3;
  byte  humidity3;
  float speed3;
  float deg3;
};
weather_structure weather;

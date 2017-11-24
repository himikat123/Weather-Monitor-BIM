#define ANT20 0xB6
#define ANT40 0xB7
#define ANT60 0xB8
#define ANT80 0xB9
#define ANT100 0xBA
#define BAT0 0xBB
#define BAT25 0xBC
#define BAT50 0xBD
#define BAT75 0xBE
#define BAT100 0xBF

extern unsigned int nowifi[0x0100];

String city;
String country;
String lang;
String fw="2.7";
String vers="BIM v"+fw;
char descript[30];
uint8_t icon=1;
uint8_t icon1;
uint8_t icon2;
uint8_t icon3;
char cityName[32];
char text_buf[255];
float tempInside;
char ssid[32]="";
char password[32]="";
String connected_ssid;
int rssi;
ntpClient *ntp;
String httpData;
char* urlLang="en";
int back=VGA_BLACK;
float temp_draw=0;
bool clock_draw=false;
int clock_upd=61;
const char* host="settings";
const char* copy="© himikat123@gmail.com";
char DEFAULT_AP_SSID[15]="WeatherMonitor";
char DEFAULT_AP_PASS[11]="1234567890";
int text_color=VGA_WHITE;
int out_color=0xFFF6;//0xEF7F;
int back_color=0x02F3;
time_t startTime=0;

struct{
  uint8_t num;
  String ssid[10];
  String pass[10];
} ssids;

struct{
  const char* lat;
  const char* lon;
  const char* alt;
  float       temp=200;
  int         pres=2000;
  int         humidity=200;
  int         bat=9;
  uint32_t    updated=0;
} outside;

struct html_structure{
  char        ap_ssid[32];
  char        ap_pass[32];
  String      city;
  int8_t      zone=0;
  String      appid;
  uint8_t     adj=0;
  uint8_t     lang=0;
  char        id[32];
  uint8_t     units=0;
  uint8_t     pres=0;
  uint8_t     timef=0;
  int         bright=0;
  int         bright_n=0;
  int         sleep=1;
  int         typ;
  int         k;
  String      ip;
  String      mask;
  String      gateway;
  String      sensor;
};
html_structure html;

struct weather_structure{
  const char* latitude;
  const char* longitude;
  const char* city;
  int         id;
  const char* icon;
  const char* descript;
  float       temp;
  float       pressure;
  byte        humidity;
  float       speed;
  float       deg;
  const char* country;
  time_t      sunrise;
  time_t      sunset;
  bool        isDay=true;

  const char* icon1;
  float day1;
  float night1;
  float speed1;
  
  const char* icon2;
  float day2;
  float night2;
  float speed2;
  
  const char* icon3;
  float day3;
  float night3;
  float speed3;
};
weather_structure weather;

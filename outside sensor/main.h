String lang;
String fw="1.0_BME280";
String vers="Outside sensor v1.0";
float tempOutside;
int bat_level=0;
char ssid[32]="";
char password[32]="";
String servak;
String url;
int rssi;
char text_buf[255];
String httpData;
const char* host="settings";
const char* copy="© himikat123@gmail.com";
char DEFAULT_AP_SSID[15]="OutsideSensor";
char DEFAULT_AP_PASS[11]="1234567890";
const char* latitude;
const char* longitude;
const char* altitude="";

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
  uint8_t     lang=0;
  int         typ;
  String      ip;
  String      mask;
  String      gateway;
};
html_structure html;


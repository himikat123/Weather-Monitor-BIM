String MacAddr="";
String lang;
String fw="1.2";
String vers="v"+fw;
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
const char* latitude;
const char* longitude;
const char* altitude="";

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
  char        ap_ssid[32];
  char        ap_pass[32];
  uint8_t     lang=0;
  int         typ;
  String      ip;
  String      mask;
  String      gateway;
  int         temp;
  int         pres;
  int         hum;
  bool        narod;
};
html_structure html;

struct{
  uint8_t num;
  String ssid[10];
  String pass[10];
} ssids;


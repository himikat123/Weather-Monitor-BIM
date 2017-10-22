String MacAddr="";
String lang;
String fw="1.3";
String vers="v"+fw;
float tempOutside;
char ssid[32]="";
char password[32]="";
String servak;
String url;
int rssi;
char text_buf[255];
String httpData;
const char* host="settings";
const char* copy="© himikat123@gmail.com";
float latitude=0;
float longitude=0;
float altitude=0;
float battery=0;
int bat_level;

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
  int         k;
  bool        thingspeak;
  String      wr_key;
};
html_structure html;

struct{
  uint8_t num;
  String ssid[10];
  String pass[10];
} ssids;


String lang;
String fw="2.0";
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
String latitude="0";
String longitude="0";
int altitude=0;
float battery=0;
int bat_level;
bool update_flag=false;

struct{
  float temp;
  int bat;
  const char* mac;
  const char* updated;
} outside;

struct html_structure{
  char        ap_ssid[32]="OutsideSensor";
  char        ap_pass[32]="1234567899";
  String      ap_ip="192.168.4.1";
  String      ap_mask="255.255.255.0";
  uint8_t     lang=0;
  int         typ;
  String      ip;
  String      mask;
  String      gateway;
  String      dns1;
  String      dns2;
  int         temp;
  int         pres;
  int         hum;
  bool        narod;
  int         k;
  bool        thingspeak;
  String      wr_key;
  bool        direct;
  int         every;
};
html_structure html;

struct{
  uint8_t num;
  String ssid[10];
  String pass[10];
} ssids;


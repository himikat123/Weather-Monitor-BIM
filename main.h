#define ANT20 0x00
#define ANT40 0x01
#define ANT60 0x02
#define ANT80 0x03
#define ANT100 0x04
#define BAT0 0x05
#define BAT25 0x06
#define BAT50 0x07
#define BAT75 0x08
#define BAT100 0x09

String lang="";
String fw="4.0";
String vers="BIM v"+fw;
char text_buf[255]="";
float tempInside=0;
float humInside=0;
char ssid[32]="";
char password[32]="";
String connected_ssid="";
int rssi=0;
ntpClient *ntp;
String httpData="";
char* urlLang="en";
int back=VGA_BLACK;
float temp_draw=0;
int hum_draw=0;
bool clock_draw=false;
int clock_upd=61;
char DEFAULT_AP_SSID[15]="WeatherMonitor";
char DEFAULT_AP_PASS[11]="1234567890";
int text_color=VGA_WHITE;
int out_color=0xFFF6;
int back_color=0x02F3;
int rama_color=VGA_WHITE;
time_t startTime=0;
String latitude="";
String longitude="";
bool update_flag=true;
bool sleep_flag=false;
bool handle_flag=true;
bool ap_flag=true;
bool bme1Detected=false;
bool bme2Detected=false;
bool shtDetected=false;
bool dhtDetected=false;
uint8_t dsDetected=0;
String updtd="";
uint8_t r_str=0;
uint8_t e_str=0;
uint8_t sm=12;
uint8_t sd=32;
char events[65]="";

struct{
  uint8_t num=0;
  String ssid[10];
  String pass[10];
} ssids;

struct{
  const char* lat="";
  const char* lon="";
  const char* alt="";
  float       temp=200;
  int         pres=2000;
  float       humidity=200;
  float       tempi=200;
  int         presi=2000;
  float       humidityi=200;
  float       tempe=200;
  int         bat=9;
  uint32_t    updated=0;
} outside;

struct html_structure{
  char        ap_ssid[32]="WeatherMonitor";
  char        ap_pass[32]="1234567899";
  String      ap_ip="192.168.4.1";
  String      ap_mask="255.255.255.0";
  int         chnl=1;
  int         hide=0;
  String      city="";
  int         city_id=0;
  String      cid="";
  String      lat="";
  String      lon="";
  int8_t      zone=0;
  String      appid="";
  uint8_t     adj=0;
  uint8_t     lang=0;
  uint8_t     timef=0;
  int         bright=0;
  int         bright_n=0;
  int         sleep=1;
  int         typ=0;
  int         k=0;
  String      ip="";
  String      mask="";
  String      gateway="";
  String      dns1="";
  String      dns2="";
  String      sensor="";
  int         temp=0;
  int         hum=0;
  int         provider=0;
  int         battery=0;
  int         os=0;
  String      sssid="";
  String      spass="";
  String      sip="";
  float       t_cor=0;
  float       h_cor=0;
  bool        ti_units=false;
  float       ti_round=0;
  float       hi_round=0;
  int         t_out=0;
  int         to_units=0;
  bool        to_round=false;
  int         h_out=0;
  int         p_out=0;
  int         po_units=0;
  int         w_units=0;
  bool        w_round=false;
  bool        ac=false;
  String      mdns="bim";
  String      chid="";
  float       to_cor=0;
  float       ho_cor=0;
  float       po_cor=0;
  uint8_t     bgr=128;
  uint8_t     bgg=128;
  uint8_t     bgb=128;
  uint8_t     txr=0;
  uint8_t     txg=128;
  uint8_t     txb=128;
  uint8_t     rmr=0;
  uint8_t     rmg=128;
  uint8_t     rmb=128;
  uint8_t     snr=128;
  uint8_t     sng=128;
  uint8_t     snb=0;
  bool        dl=false;
  int         fh=0;
  int         fm=0;
  int         th=0;
  int         tm=0;
};
html_structure html;

struct weather_structure{
  uint32_t    crc32=0;
  float       temp=404;
  float       pressure=4040;
  float       humidity=404;
  char        latitude[32]="";
  char        longitude[32]="";
  int         id=0;
  int         icon=0;
  char        descript[100]="";
  float       speed=404;
  float       deg=404;
  time_t      updated=0;
  time_t      sunrise=0;
  time_t      sunset=0;
  bool        isDay=false;

  int         icon1=0;
  float       day1=404;
  float       night1=404;
  float       speed1=404;
  
  int         icon2=0;
  float       day2=404;
  float       night2=404;
  float       speed2=404;
  
  int         icon3=0;
  float       day3=404;
  float       night3=404;
  float       speed3=404;
};
weather_structure weather;

/* Pin definitions */
#define SETTINGS_BUTTON_PIN    0 /* Settings button pin */
#define DHT22_PIN              0 /* DHT22 sensor pin*/
#define ONE_WIRE_BUS_PIN       2 /* DS18B20 one-wire bus pin */
#define TFT_BACKLIGHT         12
// TFT_DC                      4
// TFT_CS                     15
// TFT_RES                     5
// TFT_SCK                    14
// TFT_DATA                   13

#define DS18B20_RESOLUTION    12 /* DS18B20 resolution 9, 10, 11 or 12 bits */

#define SEPARATOR "**********************************************************************"

struct {
  char fw[7] = "v5.6"; // Firmware version
  bool clockSynchronized = false; // Is the time synchronized with the ntp server?
  bool clockSynchronize = false; // Should the display RTC be updated?
  bool fsInfoUpdate = true; // FS info update flag
  bool net_connected = false; // Is the device connected to the network?
  unsigned int disp_autoOff = 0; // Display auto off interval counter
  char ssids[30][33]; // List of available networks
  unsigned int rssis[30]; // List of signal strengths of available networks
  unsigned int nets = 0; // Number of available networks
  bool apMode = false; // Access point mode
  unsigned int comfort = 0; // Comfort level code
  bool clockPoints = false; // Clock points on/off

  /* Interval variables */
  unsigned int ntp_update = 100000;
  unsigned int networks_update = 0;
  unsigned int sensors_update = 0;
  unsigned int thingspeakReceive = 0;
  unsigned int thingspeakSend = 0;
  unsigned int narodmonSend = 0;
} global;

class Config {
  #define NETWORKS 3
  #define PROVIDERS 2
  #define THNG_FIELDS 8
  #define NAROD_FIELDS 12

  private:
  
  // Comfort
  unsigned int _comfort_temp_source = 0; // Comfort temperature source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-BMP180, 4-SHT21, 5-DHT22, 6-DS18B20
  unsigned int _comfort_temp_thing = 0; // Comfort temperature thingspeak field number: 0...7
  int _comfort_temp_min = 23; // Minimum comfort temperature: -50...100
  int _comfort_temp_max = 25; // Maximum comfort temperature: -50...100
  unsigned int _comfort_hum_source = 0; // Comfort humidity source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-SHT21, 4-DHT22
  unsigned int _comfort_hum_thing = 0; // Comfort humidity thingspeak field number: 0...7
  int _comfort_hum_min = 40; // Minimum comfort humidity: 0...100
  int _comfort_hum_max = 60; // Maximum comfort humidity: 0...100
    
  //WiFi network
  char _network_ssid[NETWORKS][33] = { "", "", "" }; // SSID list
  char _network_pass[NETWORKS][33] = { "", "", "" }; // Password list
  bool _network_type = false; // Connection type: false = dynamic IP, true = static IP
  char _network_ip[16] = ""; // Static IP address
  char _network_mask[16] = ""; // Static Subnet mask
  char _network_gw[16] = ""; // Static default gateway
  char _network_dns1[16] = ""; // Static DNS1 address
  char _network_dns2[16] = ""; // Static DNS2 address
  
  // Access point
  char _accessPoint_ssid[33] = "Weather monitor BIM"; // SSID
  char _accessPoint_pass[33] = "1234567890"; // Password
  unsigned int _accessPoint_chnl = 1; // WiFi channel number 1...13
  char _accessPoint_ip[33] = "192.168.4.1"; // IP address
  char _accessPoint_mask[33] = "255.255.255.0"; // Subnet mask
  
  // Weather
  char _weather_appid[PROVIDERS][33] = { "", "" }; // [0]->nothing, [1]->KEY weatherbit.io
  char _weather_city[41] = ""; // City name
  unsigned int _weather_cityid = 0; // City ID
  float _weather_lat = 0.0; // Latitude
  float _weather_lon = 0.0; // Longitude
  unsigned int _weather_provider = 0; // Weather forecast provider: 1-weatherbit.io, 2-open-meteo.com
  unsigned int _weather_citysearch = 0; // The way to recognize a city: 0-by name, 1-by ID, 2-by coordinates
  float _weather_temp_corr = 0; // Weather temperature correction
  float _weather_hum_corr = 0; // Weather humidity correction
  float _weather_pres_corr = 0; // Weather pressure correction

  // Language
  char _lang[3] = "en";

  // Clock
  unsigned int _clock_format = 0; // Clock format: 0-12 hour wo leading zero, 1-12 hour with leading zero, 2-24 hour wo leading zero, 3-24 hour with leading zero 
  char _clock_ntp[65] = "time.nist.gov"; // NTP server address
  int _clock_utc = 0; // Timezone: -12...13
  bool _clock_dlst = false; // Auto daylight saving time
  unsigned int _clock_ntp_period = 15; // NTP update period (minutes): 0...90000

  // Display
  char _display_dayTime[6] = "07:00"; // Time to switch to day mode
  char _display_nightTime[6] = "21:00"; // Time to switch to night mode
  unsigned int _display_brightMethod = 3; // Display brightness adjustment method: 0-Auto, 1-By light sensor, 2-By time, 3-Constant
  unsigned int _display_brightness_day = 50; // Day mode brightness: 1...100
  unsigned int _display_brightness_night = 50; // Night mode brightness: 1...100
  unsigned int _display_lightSensor = 1; // Sensor type for brightness adjust: 0-Analog input, 1-MAX44009, 2-BH1750
  unsigned int _display_lightSensor_sensitivity = 50; // Ambient light sensor sensibility: 1...100
  unsigned int _display_source_tempOut_sens = 0; // Outdoor temperature data source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-BMP180, 4-SHT21, 5-DHT22, 6-DS18B20
  unsigned int _display_source_tempOut_thing = 0; // Thingspeak field number for the outdoor temperature: 0...7
  unsigned int _display_source_humOut_sens = 0; // Outdoor humidity data source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-SHT21, 4-DHT22
  unsigned int _display_source_humOut_thing = 0; // Thingspeak field number for the outdoor humidity: 0...7
  unsigned int _display_source_presOut_sens = 0; // Outdoor pressure data source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-BMP180
  unsigned int _display_source_presOut_thing = 0; // Thingspeak field number for the outdoor pressure: 0...7
  unsigned int _display_source_tempIn_sens = 0; // Indoor temperature data source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-BMP180, 4-SHT21, 5-DHT22, 6-DS18B20
  unsigned int _display_source_tempIn_thing = 0; // Thingspeak field number for the intdoor temperature: 0...7
  unsigned int _display_source_humIn_sens = 0; // Indoor humidity data source: 0-Forecast, 1-Thingspeak, 2-BME280, 3-SHT21, 4-DHT22
  unsigned int _display_source_humIn_thing = 0; // Thingspeak field number for the outdoor humidity: 0...7
  unsigned int _display_source_volt_sens = 0; // Voltage data source: 0-Nothing, 1-Thingspeak
  unsigned int _display_source_volt_thing = 0; // Thingspeak field number for the voltage: 0...7
  unsigned int _display_source_volt_thingType = 0; // Sensor type for the voltage: 0-Battery voltage, 1-Battery percentage
  unsigned int _display_source_bat_sens = 0; // Battery level data source: 0-Nothing, 1-Thingspeak
  unsigned int _display_source_bat_thing = 0; // Thingspeak field number for the battery level: 0...7
  unsigned int _display_source_descr = 0; // Additional description data source: 0-Nothing, 1-Comfort level
  
  // Sensors
  float _bme280_temp_corr = 0.0; // BME280 temperature correction
  float _bme280_hum_corr = 0.0; // BME280 humidity correction
  float _bme280_pres_corr = 0.0; // BME280 pressure correction
  float _bmp180_temp_corr = 0.0; // BMP180 temperature correction
  float _bmp180_pres_corr = 0.0; // BMP180 pressure correction
  float _sht21_temp_corr = 0.0; // SHT21 temperature correction
  float _sht21_hum_corr = 0.0; // SHT21 humidity correction
  float _dht22_temp_corr = 0.0; // DHT22 temperature correction
  float _dht22_hum_corr = 0.0; // DHT22 humidity correction
  float _ds18b20_temp_corr = 0.0; // DS18B20 temperature correction
  float _max44009_light_corr = 0.0; // MAX44009 ambient light correction
  float _bh1750_light_corr = 0.0; // BH1750 ambient light correction
  float _analog_voltage_corr = 0.0; // Analog ambient light sensor voltage correction

  // Thingspeak send
  unsigned int _thingspeakSend_period = 5; // Period for sending data to Thingspeak (minutes): 0...999
  char _thingspeakSend_channelID[11] = ""; // Channel ID for sending data to Thingspeak
  char _thingspeakSend_wrkey[17] = ""; // Write API Key for sending data to Thingspeak
  char _thingspeakSend_rdkey[17] = ""; // Read API Key for sending data to Thingspeak
  unsigned int _thingspeakSend_fields[THNG_FIELDS] = {0, 0, 0, 0, 0, 0, 0, 0}; // Data sources to send to Thingspeak fields
  unsigned int _thingspeakSend_types[THNG_FIELDS] = {0, 0, 0, 0, 0, 0, 0, 0}; // Sensor data types to send to Thingspeak

  // Thingspeak receive
  unsigned int _thingspeakReceive_period = 5; // Period for receiving data from Thingspeak (minutes): 0...999
  char _thingspeakReceive_channelID[11] = ""; // Channel ID for receiving data from Thingspeak
  char _thingspeakReceive_rdkey[17] = ""; // Read API Key for receiving data from Thingspeak
  unsigned int _thingspeakReceive_expire = 10; // Thingspeak data expire (minutes): 1...999

  // Narodmon send
  unsigned int _narodmonSend_period = 5; // Period for sending data to Narodmon (minutes): 0...999
  char _narodmonSend_lon[11] = ""; // Longitude for Narodmon
  char _narodmonSend_lat[11] = ""; // Latitude for Narodmon
  char _narodmonSend_name[17] = "BIM"; // Sensor name
  unsigned int _narodmonSend_sensors[NAROD_FIELDS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Data sources to send to Narodmon fields
  char _narodmonSend_metrics[NAROD_FIELDS][17] {"T", "H", "P", "W", "F", "D", "S", "J", "F", "Y", "X", "C"}; // Sensor metrics to send to Narodmon fields
  unsigned int _narodmonSend_types[NAROD_FIELDS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Sensor data types to send to Narodmon

  // Account
  char _account_name[33] = ""; // Web interface username
  char _account_pass[65] = ""; // Web interface password
  bool _account_required = false; // Require username and password to access the web interface

  
  /* 
   * Division of time into hours and minutes 
   */
  unsigned int _get_time(bool level, String time) {
    unsigned int hour = time.substring(0, 2).toInt();
    unsigned int minute = time.substring(3, 5).toInt();
    if(hour > 23) hour = 0;
    if(minute > 59) minute = 0;
    return level ? minute : hour; 
  }


  public:

  void readConfig(void) {
    /* Read config file */
    Serial.println(SEPARATOR);
    Serial.print("Read config file... ");
    File file = LittleFS.open("/config.json", "r");
    if(file) {
      while(file.available()) {
        String json = file.readString();
        DynamicJsonDocument conf(12192);
        DeserializationError error = deserializeJson(conf, json);
        if(!error) {
          #define COPYSTR(from, to) strlcpy(to, from | to, sizeof(to))
          #define COPYNUM(from, to) to = from | to
          #define COPYBOOL(from, to) to = from.as<bool>() | to
          
          //WiFi network
          for(unsigned int i=0; i<NETWORKS; i++) {
            COPYSTR(conf["network"]["ssid"][i], _network_ssid[i]);
            COPYSTR(conf["network"]["pass"][i], _network_pass[i]);
          }
          COPYBOOL(conf["network"]["type"], _network_type);
          COPYSTR(conf["network"]["ip"], _network_ip);
          COPYSTR(conf["network"]["mask"], _network_mask);
          COPYSTR(conf["network"]["gw"], _network_gw);
          COPYSTR(conf["network"]["dns1"], _network_dns1);
          COPYSTR(conf["network"]["dns2"], _network_dns2);
          
          // Access point
          COPYSTR(conf["accessPoint"]["ssid"], _accessPoint_ssid);
          COPYSTR(conf["accessPoint"]["pass"], _accessPoint_pass);
          COPYNUM(conf["accessPoint"]["chnl"], _accessPoint_chnl);
          COPYSTR(conf["accessPoint"]["ip"], _accessPoint_ip);
          COPYSTR(conf["accessPoint"]["mask"], _accessPoint_mask);
          
          // Weather
          for(unsigned int i=0; i<PROVIDERS; i++) COPYSTR(conf["weather"]["appid"][i], _weather_appid[i]);
          COPYSTR(conf["weather"]["city"], _weather_city);
          COPYNUM(conf["weather"]["cityid"], _weather_cityid);
          COPYNUM(conf["weather"]["lon"], _weather_lon);
          COPYNUM(conf["weather"]["lat"], _weather_lat);
          COPYNUM(conf["weather"]["provider"], _weather_provider);
          COPYNUM(conf["weather"]["citysearch"], _weather_citysearch);
          COPYNUM(conf["weather"]["corr"]["t"], _weather_temp_corr);
          COPYNUM(conf["weather"]["corr"]["h"], _weather_hum_corr);
          COPYNUM(conf["weather"]["corr"]["p"], _weather_pres_corr);
        
          // Language
          COPYSTR(conf["lang"], _lang);

          // Clock
          COPYNUM(conf["clock"]["format"], _clock_format);
          COPYSTR(conf["clock"]["ntp"], _clock_ntp);
          COPYNUM(conf["clock"]["utc"], _clock_utc);
          COPYBOOL(conf["clock"]["dlst"], _clock_dlst);
          COPYNUM(conf["clock"]["ntp_period"], _clock_ntp_period);

          // Display
          COPYSTR(conf["display"]["dayTime"], _display_dayTime);
          COPYSTR(conf["display"]["nightTime"], _display_nightTime);
          COPYNUM(conf["display"]["brightMethod"], _display_brightMethod);
          COPYNUM(conf["display"]["brightness"]["day"], _display_brightness_day);
          COPYNUM(conf["display"]["brightness"]["night"], _display_brightness_night);
          COPYNUM(conf["display"]["lightSensor"], _display_lightSensor);
          COPYNUM(conf["display"]["sensitivity"], _display_lightSensor_sensitivity);
          COPYNUM(conf["display"]["source"]["tempOut"]["sens"], _display_source_tempOut_sens);
          COPYNUM(conf["display"]["source"]["tempOut"]["thing"], _display_source_tempOut_thing);
          COPYNUM(conf["display"]["source"]["humOut"]["sens"], _display_source_humOut_sens);
          COPYNUM(conf["display"]["source"]["humOut"]["thing"], _display_source_humOut_thing);
          COPYNUM(conf["display"]["source"]["presOut"]["sens"], _display_source_presOut_sens);
          COPYNUM(conf["display"]["source"]["presOut"]["thing"], _display_source_presOut_thing);
          COPYNUM(conf["display"]["source"]["tempIn"]["sens"], _display_source_tempIn_sens);
          COPYNUM(conf["display"]["source"]["tempIn"]["thing"], _display_source_tempIn_thing);
          COPYNUM(conf["display"]["source"]["humIn"]["sens"], _display_source_humIn_sens);
          COPYNUM(conf["display"]["source"]["humIn"]["thing"], _display_source_humIn_thing);
          COPYNUM(conf["display"]["source"]["volt"]["sens"], _display_source_volt_sens);
          COPYNUM(conf["display"]["source"]["volt"]["thing"], _display_source_volt_thing);
          COPYNUM(conf["display"]["source"]["volt"]["thingType"], _display_source_volt_thingType);
          COPYNUM(conf["display"]["source"]["bat"]["sens"], _display_source_bat_sens);
          COPYNUM(conf["display"]["source"]["bat"]["thing"], _display_source_bat_thing);
          COPYNUM(conf["display"]["source"]["descr"], _display_source_descr);

          // Sensors
          COPYNUM(conf["sensors"]["bme280"]["t"], _bme280_temp_corr);
          COPYNUM(conf["sensors"]["bme280"]["h"], _bme280_hum_corr);
          COPYNUM(conf["sensors"]["bme280"]["p"], _bme280_pres_corr);
          COPYNUM(conf["sensors"]["bmp180"]["t"], _bmp180_temp_corr);
          COPYNUM(conf["sensors"]["bmp180"]["p"], _bmp180_pres_corr);
          COPYNUM(conf["sensors"]["sht21"]["t"], _sht21_temp_corr);
          COPYNUM(conf["sensors"]["sht21"]["h"], _sht21_hum_corr);
          COPYNUM(conf["sensors"]["dht22"]["t"], _dht22_temp_corr);
          COPYNUM(conf["sensors"]["dht22"]["h"], _dht22_hum_corr);
          COPYNUM(conf["sensors"]["ds18b20"]["t"], _ds18b20_temp_corr);
          COPYNUM(conf["sensors"]["max44009"]["l"], _max44009_light_corr);
          COPYNUM(conf["sensors"]["bh1750"]["l"], _bh1750_light_corr);
          COPYNUM(conf["sensors"]["analog"]["v"], _analog_voltage_corr);

          // Thingspeak send
          COPYNUM(conf["thingspeakSend"]["period"], _thingspeakSend_period);
          COPYSTR(conf["thingspeakSend"]["channelID"], _thingspeakSend_channelID);
          COPYSTR(conf["thingspeakSend"]["wrkey"], _thingspeakSend_wrkey);
          COPYSTR(conf["thingspeakSend"]["rdkey"], _thingspeakSend_rdkey);
          for(unsigned int i=0; i<THNG_FIELDS; i++) {
            COPYNUM(conf["thingspeakSend"]["fields"][i], _thingspeakSend_fields[i]);
            COPYNUM(conf["thingspeakSend"]["types"][i], _thingspeakSend_types[i]);
          }

          // Thingspeak receive
          COPYNUM(conf["thingspeakReceive"]["period"], _thingspeakReceive_period);
          COPYSTR(conf["thingspeakReceive"]["channelID"], _thingspeakReceive_channelID);
          COPYSTR(conf["thingspeakReceive"]["rdkey"], _thingspeakReceive_rdkey);
          COPYNUM(conf["thingspeakReceive"]["expire"], _thingspeakReceive_expire);

          // Narodmon send
          COPYNUM(conf["narodmonSend"]["period"], _narodmonSend_period);
          COPYSTR(conf["narodmonSend"]["lon"], _narodmonSend_lon);
          COPYSTR(conf["narodmonSend"]["lat"], _narodmonSend_lat);
          COPYSTR(conf["narodmonSend"]["name"], _narodmonSend_name);
          for(unsigned int i=0; i<NAROD_FIELDS; i++) {
            COPYNUM(conf["narodmonSend"]["sensors"][i], _narodmonSend_sensors[i]);
            COPYSTR(conf["narodmonSend"]["metrics"][i], _narodmonSend_metrics[i]);
            COPYNUM(conf["narodmonSend"]["types"][i], _narodmonSend_types[i]);
          }

          // Comfort
          COPYNUM(conf["comfort"]["temp"]["source"], _comfort_temp_source);
          COPYNUM(conf["comfort"]["temp"]["thing"], _comfort_temp_thing);
          COPYNUM(conf["comfort"]["temp"]["min"], _comfort_temp_min);
          COPYNUM(conf["comfort"]["temp"]["max"], _comfort_temp_max);
          COPYNUM(conf["comfort"]["hum"]["source"], _comfort_hum_source);
          COPYNUM(conf["comfort"]["hum"]["thing"], _comfort_hum_thing);
          COPYNUM(conf["comfort"]["hum"]["min"], _comfort_hum_min);
          COPYNUM(conf["comfort"]["hum"]["max"], _comfort_hum_max);

          // Account
          COPYSTR(conf["account"]["name"], _account_name);
          COPYBOOL(conf["account"]["required"], _account_required);

          Serial.println("done");
        }
        else Serial.println(" Configuration file corrupted");
      }
    }
    else Serial.println(" No configuration file found");

    /* Read user file */
    Serial.println(SEPARATOR);
    Serial.print("Read user file... ");
    file = LittleFS.open("/user.us", "r");
    if(file) {
      while(file.available()) {
        String json = file.readString();
        DynamicJsonDocument conf(1024);
        DeserializationError error = deserializeJson(conf, json);
        if(!error) {
          strlcpy(_account_pass, conf["pass"] | _account_pass, sizeof(_account_pass));
          Serial.println("done");
        }
        else Serial.println(" User file corrupted");
      }
    }
    else Serial.println(" No user file found");
  }

  /**
   * Save json data to the config file
   * @param json data
   * @param filename
   * @return false on success 
   */
  bool save(String json, String filename="/config.json") {
    File file = LittleFS.open(filename, "w");
    if(file and json.length()) {
      file.print(json);
      file.close();
      Serial.println(json);
      readConfig();
      return false;
    }
    else return true;
  }

  /**
   * Save new password
   * @param new password
   * @return false on success
   */
  bool savePassword(String newPass) {
    String json = "{\"pass\":\"";
    json += newPass;
    json += "\"}";
    File file = LittleFS.open("/user.us", "w");
    if(file.print(json)) {
      file.close();
      readConfig();
      return false;
    }
    else return true;
  }

  /**
   * Getters
   */

  char* network_ssid(unsigned int num) {
    if(num >= NETWORKS) return (char*) "";
    return _network_ssid[num];
  }

  char* network_pass(unsigned int num) {
    if(num >= NETWORKS) return (char*) "";
    return _network_pass[num];
  }

  bool network_type() {
    return _network_type;
  }

  String network_ip() {
    return String(_network_ip);
  }

  String network_mask() {
    return String(_network_mask);
  }

  String network_gw() {
    return String(_network_gw);
  }

  String network_dns1() {
    return String(_network_dns1);
  }

  String network_dns2() {
    return String(_network_dns2);
  }

  char* accessPoint_ssid() {
    return _accessPoint_ssid;
  }

  char* accessPoint_pass() {
    return _accessPoint_pass;
  }

  unsigned int accessPoint_chnl() {
    if(_accessPoint_chnl < 1 or _accessPoint_chnl > 13) return 1;
    return _accessPoint_chnl;
  }

  String accessPoint_ip() {
    return String(_accessPoint_ip);
  }

  String accessPoint_mask() {
    return String(_accessPoint_mask);
  }

  String weather_appid(unsigned int num) {
    if(num >= PROVIDERS) return "";
    return String(_weather_appid[num]);
  }

  String weather_city() {
    return String(_weather_city);
  }

  String weather_cityid() {
    return String(_weather_cityid);
  }

  String weather_lon() {
    return String(_weather_lon);
  }

  String weather_lat() {
    return String(_weather_lat);
  }

  unsigned int weather_provider() {
    if(_weather_provider > 2) return 0;
    return _weather_provider;
  }

  unsigned int weather_citysearch() {
    if(_weather_citysearch > 2) return 0;
    return _weather_citysearch;
  }

  float weather_temp_corr() {
    return _weather_temp_corr;
  }

  float weather_hum_corr() {
    return _weather_hum_corr;
  }

  float weather_pres_corr() {
    return _weather_pres_corr;
  }

  String lang() {
    if(String(_lang) == "") return "en";
    return String(_lang);
  }

  unsigned int clock_format() {
    if(_clock_format > 3) return 0;
    return _clock_format;
  }

  char* clock_ntp() {
    if(String(_clock_ntp) == "") return (char*) "time.nist.gov";
    return _clock_ntp;
  }

  int clock_utc() {
    if(_clock_utc < -12 or _clock_utc > 13) return 0;
    return _clock_utc;
  }

  bool clock_dlst() {
    return _clock_dlst;
  }

  unsigned int clock_ntp_period(){
    if(_clock_ntp_period > 90000) return 15;
    return _clock_ntp_period;
  }

  unsigned int display_dayTime(bool level) {
    return _get_time(level, _display_dayTime); 
  }

  unsigned int display_nightTime(bool level) {
    return _get_time(level, _display_nightTime);
  }
    
  unsigned int display_brightMethod() {
    if(_display_brightMethod > 3) return 3;
    return _display_brightMethod;
  }

  unsigned int display_brightness_day() {
    if(_display_brightness_day < 1 or _display_brightness_day > 100) return 100;
    return _display_brightness_day;
  }

  unsigned int display_brightness_night() {
    if(_display_brightness_night < 1 or _display_brightness_night > 100) return 100;
    return _display_brightness_night;
  }

  unsigned int display_lightSensor() {
    if(_display_lightSensor > 2) return 0;
    return _display_lightSensor;
  }

  unsigned int display_lightSensor_sensitivity() {
    if(_display_lightSensor_sensitivity < 1 or _display_lightSensor_sensitivity > 100) return 50;
    return _display_lightSensor_sensitivity;
  }

  unsigned int display_source_tempOut_sens() {
    if(_display_source_tempOut_sens > 7) return 0;
    return _display_source_tempOut_sens;
  }

  unsigned int display_source_tempOut_thing() {
    if(_display_source_tempOut_thing >= THNG_FIELDS) return 0;
    return _display_source_tempOut_thing; 
  }

  unsigned int display_source_humOut_sens() {
    if(_display_source_humOut_sens > 5) return 0;
    return _display_source_humOut_sens; 
  }

  unsigned int display_source_humOut_thing() {
    if(_display_source_humOut_thing >= THNG_FIELDS) return 0;
    return _display_source_humOut_thing; 
  }

  unsigned int display_source_presOut_sens() {
    if(_display_source_presOut_sens > 4) return 0;
    return _display_source_presOut_sens;
  }

  unsigned int display_source_presOut_thing() {
    if(_display_source_presOut_thing >= THNG_FIELDS) return 0;
    return _display_source_presOut_thing; 
  }

  unsigned int display_source_tempIn_sens() {
    if(_display_source_tempIn_sens > 7) return 0;
    return _display_source_tempIn_sens; 
  }

  unsigned int display_source_tempIn_thing() {
    if(_display_source_tempIn_thing >= THNG_FIELDS) return 0;
    return _display_source_tempIn_thing;
  }

  unsigned int display_source_humIn_sens() {
    if(_display_source_humIn_sens > 5) return 0;
    return _display_source_humIn_sens; 
  }

  unsigned int display_source_humIn_thing() {
    if(_display_source_humIn_thing >= THNG_FIELDS) return 0;
    return _display_source_humIn_thing; 
  }

  unsigned int display_source_volt_sens() {
    if(_display_source_volt_sens > 1) return 0;
    return _display_source_volt_sens; 
  }

  unsigned int display_source_volt_thing() {
    if(_display_source_volt_thing >= THNG_FIELDS) return 0;
    return _display_source_volt_thing;
  }

  unsigned int display_source_volt_thingType() {
    if(_display_source_volt_thingType > 1) return 0;
    return _display_source_volt_thingType; 
  }

  unsigned int display_source_bat_sens() {
    if(_display_source_bat_sens > 1) return 0;
    return _display_source_bat_sens;
  }

  unsigned int display_source_bat_thing() {
    if(_display_source_bat_thing >= THNG_FIELDS) return 0;
    return _display_source_bat_thing;
  }

  unsigned int display_source_descr() {
    if(_display_source_descr > 1) return 0;
    return _display_source_descr; 
  }

  float bme280_temp_corr() {
    return _bme280_temp_corr;
  }

  float bme280_hum_corr() {
    return _bme280_hum_corr;
  }

  float bme280_pres_corr() {
    return _bme280_pres_corr;
  }

  float bmp180_temp_corr() {
    return _bmp180_temp_corr;
  }

  float bmp180_pres_corr() {
    return _bmp180_pres_corr;
  }

  float sht21_temp_corr() {
    return _sht21_temp_corr;
  }

  float sht21_hum_corr() {
    return _sht21_hum_corr;
  }

  float dht22_temp_corr() {
    return _dht22_temp_corr;
  }

  float dht22_hum_corr() {
    return _dht22_hum_corr;
  }

  float ds18b20_temp_corr() {
    return _ds18b20_temp_corr;
  }

  float max44009_light_corr() {
    return _max44009_light_corr;
  }

  float bh1750_light_corr() {
    return _bh1750_light_corr;
  }

  float analog_voltage_corr() {
    return _analog_voltage_corr;
  }

  bool thingspeakSend_turnOn() {
    return _thingspeakSend_period > 0;
  }

  unsigned int thingspeakSend_period() {
    if(_thingspeakSend_period < 1 or _thingspeakSend_period > 999) return 5;
    return _thingspeakSend_period;
  }

  String thingspeakSend_channelID() {
    return String(_thingspeakSend_channelID);
  }

  String thingspeakSend_wrkey() {
    return String(_thingspeakSend_wrkey);
  }

  String thingspeakSend_rdkey() {
    return String(_thingspeakSend_rdkey);
  }

  unsigned int thingspeakSend_fields(unsigned int num) {
    if(num >= THNG_FIELDS) return 0;
    if(_thingspeakSend_fields[num] > 11) return 0;
    return _thingspeakSend_fields[num];
  }

  unsigned int thingspeakSend_types(unsigned int num) {
    if(num >= THNG_FIELDS) return 0;
    if(_thingspeakSend_types[num] > 2) return 0;
    return _thingspeakSend_types[num];
  }

  bool thingspeakReceive_turnOn() {
    return _thingspeakReceive_period > 0;
  }

  unsigned int thingspeakReceive_period() {
    if(_thingspeakReceive_period < 1 or _thingspeakReceive_period > 999) return 5;
    return _thingspeakReceive_period;
  }

  String thingspeakReceive_channelID() {
    return String(_thingspeakReceive_channelID);
  }

  String thingspeakReceive_rdkey() {
    return String(_thingspeakReceive_rdkey);
  }

  unsigned int thingspeakReceive_expire() {
    if(_thingspeakReceive_expire < 1 or _thingspeakReceive_expire > 999) return 5;
    return _thingspeakReceive_expire;
  }

  bool narodmonSend_turnOn() {
    return _narodmonSend_period > 0;
  }

  unsigned int narodmonSend_period() {
    if(_narodmonSend_period < 1 or _narodmonSend_period > 999) return 5;
    return _narodmonSend_period;
  }

  String narodmonSend_lon() {
    return String(_narodmonSend_lon);
  }

  String narodmonSend_lat() {
    return String(_narodmonSend_lat);
  }

  String narodmonSend_name() {
    return String(_narodmonSend_name);
  }

  unsigned int narodmonSend_sensors(unsigned int num) {
    if(num >= NAROD_FIELDS) return 0;
    if(_narodmonSend_sensors[num] > 12) return 0;
    return _narodmonSend_sensors[num];
  }

  String narodmonSend_metrics(unsigned int num) {
    if(num >= NAROD_FIELDS) return "";
    return String(_narodmonSend_metrics[num]);
  }

  unsigned int narodmonSend_types(unsigned int num) {
    if(num >= NAROD_FIELDS) return 0;
    if(_narodmonSend_types[num] > 2) return 0;
    return _narodmonSend_types[num];
  }

  unsigned int comfort_temp_source() {
    if(_comfort_temp_source > 6) return 0;
    return _comfort_temp_source;
  }
  
  unsigned int comfort_temp_thing() {
    if(_comfort_temp_thing >= THNG_FIELDS) return 0;
    return _comfort_temp_thing;
  }
  
  int comfort_temp_min() {
    if(_comfort_temp_min < -50 or _comfort_temp_min > 100) return 23;
    return _comfort_temp_min;
  }
  
  int comfort_temp_max() {
    if(_comfort_temp_max < -50 or _comfort_temp_max > 100) return 25;
    return _comfort_temp_max;
  }
  
  unsigned int comfort_hum_source() {
    if(_comfort_hum_source > 4) return 0;
    return _comfort_hum_source;
  }
  
  unsigned int comfort_hum_thing() {
    if(_comfort_hum_thing >= THNG_FIELDS) return 0;
    return _comfort_hum_thing;
  }
  
  int comfort_hum_min() {
    if(_comfort_hum_min < 0 or _comfort_hum_min > 100) return 40;
    return _comfort_hum_min;
  }
  
  int comfort_hum_max() {
    if(_comfort_hum_max < 0 or _comfort_hum_max > 100) return 60;
    return _comfort_hum_max;
  }
  
  String account_name() {
    return String(_account_name);
  }

  String account_pass() {
    return String(_account_pass);
  }

  bool account_required() {
    return _account_required;
  }  

  /* 
   * Setters
   */
  void set_bright(int bright) {
    if(bright >= 1 and bright <= 100) {
      _display_brightness_day = bright;
      _display_brightness_night = bright;
    }
  }

  void set_sensitivity(int sensitivity) {
    if(sensitivity >= 1 and sensitivity <= 100) _display_lightSensor_sensitivity = sensitivity;
  }

  void set_lang(String lng) {
    lng.toCharArray(_lang, 3);
  }
};

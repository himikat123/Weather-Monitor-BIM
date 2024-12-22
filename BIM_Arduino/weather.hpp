class Weather {
  #define WEATHERBIT 1
  #define OPEN_METEO 2
  #define DAYS 3

  public:
    void update();
    bool checkWind(float wind);
    
    float get_currentTemp();
    float get_currentHum();
    float get_currentPres();
    float get_currentWindSpeed();
    int get_currentWindDir();
    unsigned int get_currentIcon();
    bool get_isDay();
    String get_description();
    unsigned int get_currentUpdated();
    unsigned int get_errorUpdate();
    unsigned int convertIcon(int code);
    void reset_errorUpdate();

    float get_dailyDayTemp(unsigned int num);
    float get_dailyNightTemp(unsigned int num);
    float get_dailyWindSpeed(unsigned int num);
    unsigned int get_dailyIcon(unsigned int num); 

  private:
    String _description = "----";
    float _currentTemp = 40400.0;
    float _currentHum = 40400.0;
    float _currentPres = 40400.0;
    float _currentWindSpeed = -1.0;
    int _currentWindDir = -1;
    unsigned int _currentIcon = 0;
    bool _is_day = false;
    unsigned int _currentUpdated = 0;
    unsigned int _errorUpdate = 0;
    String _country = "";
    String _city = "";

    float _dailyDayTemp[DAYS] = {40400.0, 40400.0, 40400.0};
    float _dailyNightTemp[DAYS] = {40400.0, 40400.0, 40400.0};
    float _dailyWindSpeed[DAYS] = {-1.0, -1.0, -1.0};
    unsigned int _dailyIcon[DAYS] = {0, 0, 0};
    unsigned int _dailyUpdated = 0;

    unsigned int _weatherbit_icon(int code);
    void _updateDaily(void);
    String _openMeteoCode2Description(uint8_t code);
    unsigned int _openMeteoIcon(int code);
};

/**
 * Check if wind speed is within the normal range
 */
bool Weather::checkWind(float wind) {
  return (wind >= 0.0 and wind <= 99.0);
}

/**
 * Open-meteo weather description
 */
String Weather::_openMeteoCode2Description(uint8_t code) {
    switch(code) {
        case 0: return lang.clearSky();
        case 1: return lang.mainlyClear();
        case 2: return lang.partlyCloudy();
        case 3:  return lang.overcast();
        case 45: return lang.fog();
        case 48: return lang.deposRimeFog();
        case 51: case 53: case 55: return lang.drizzle();
        case 56: case 57: return lang.freezingDrizzle();
        case 61: case 63: case 65: return lang.rain();
        case 66: case 67: return lang.freezingRain();
        case 71: case 73: case 75: return lang.snowFall();
        case 77: return lang.snowGrains();
        case 80: case 81: case 82: return lang.rainShowers();
        case 85: case 86: return lang.snowShowers();
        case 95: return lang.thunderstorm();
        case 96: case 99: return lang.thunderstormWithHail();
        default: return "---";
    }
}

/**
 * Open-meteo icon code conversion
 */
unsigned int Weather::_openMeteoIcon(int code) {
    switch(code) {
        case 0: case 1: return 1;
        case 2: return 2;
        case 3:  return 4;
        case 45: case 48: return 50;
        case 51: case 53: case 55: case 56: case 57: return 10;
        case 61: case 63: case 65: case 66: case 67: case 80: case 81: case 82: return 9;
        case 71: case 73: case 75: case 77: case 85: case 86: return 13;
        case 95: case 96: case 99: return 11;
        default: return 1;
    }
}

/**
 * Weather icon code conversion
 */
unsigned int Weather::convertIcon(int code) {
    switch(code) {
        case 1: return 1; break;
        case 2: return 2; break;
        case 3: return 2; break;
        case 4: return 3; break;
        case 9: return 4; break;
        case 10: return 5; break;
        case 11: return 6; break;
        case 13: return 7; break;
        case 50: return 8; break;
        default: return 0; break;
    }
}

/**
 * Weather update
 */
void Weather::update() {
  String url = "";
  if(config.weather_provider() == OPEN_METEO) {
    if(config.weather_lat() == "" || config.weather_lon() == "") {
      Serial.println("No Coordinates");
      return;
    }
    url = "http://api.open-meteo.com/v1/forecast";
    url += "?latitude=" + config.weather_lat();
    url += "&longitude=" + config.weather_lon();
    url += "&current=temperature_2m,relative_humidity_2m,is_day,weather_code,pressure_msl,wind_speed_10m,wind_direction_10m";
    url += "&wind_speed_unit=ms&timeformat=unixtime&timezone=auto";
  }
  else if(config.weather_provider() == WEATHERBIT) {
    if(config.weather_lat() == "" || config.weather_lon() == "") {
      Serial.println("No Coordinates");
      _errorUpdate = now();
      return;
    }
    url = "http://api.weatherbit.io/v2.0/current?key=" + config.weather_appid(WEATHERBIT);
    url += "&lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
    url += "&lang=" + config.lang();
  }
  else {
    Serial.println("Wrong weather provider");
    _errorUpdate = now();
    return;
  }
  Serial.println(url);
  HTTPClient clientWeather;
  clientWeather.begin(wifiClient, url);
  int httpCode = clientWeather.GET();
  if(httpCode == HTTP_CODE_OK) {
    String httpData = clientWeather.getString();
    Serial.println(httpData);
    StaticJsonDocument<2048> weather;
    DeserializationError errorWeather = deserializeJson(weather, httpData);
    if(errorWeather) {
      Serial.println("Current weather deserialization error");
      _errorUpdate = now();
      return;
    }
    if(config.weather_provider() == OPEN_METEO) {
      _description      = _openMeteoCode2Description(weather["current"]["weather_code"] | 0);
      _currentTemp      = weather["current"]["temperature_2m"] | 40400.0;
      _currentHum       = weather["current"]["relative_humidity_2m"] | 40400.0;
      _currentPres      = weather["current"]["pressure_msl"] | 40400.0;
      _currentWindSpeed = weather["current"]["wind_speed_10m"] | -1.0;
      _currentWindDir   = weather["current"]["wind_direction_10m"] | -1;
      _is_day           = weather["current"]["is_day"] == 0 ? false : true;
      _currentIcon      = _openMeteoIcon(weather["current"]["weather_code"] | 0);
    }
    if(config.weather_provider() == WEATHERBIT) {
      _description      = weather["data"][0]["weather"]["description"].as<String>();
      _currentTemp      = weather["data"][0]["temp"] | 40400.0;
      _currentHum       = weather["data"][0]["rh"] | 40400.0;
      _currentPres      = weather["data"][0]["pres"] | 40400.0;
      _currentWindSpeed = weather["data"][0]["wind_spd"] | -1.0;
      _currentWindDir   = weather["data"][0]["wind_dir"] | -1;
      _currentIcon      = _weatherbit_icon(weather["data"][0]["weather"]["code"].as<int>() | 0);
      const char* pod   = weather["data"][0]["pod"] | "";
      _is_day           = (String(pod) == String('d')) ? true : false;
      _country          = weather["data"][0]["country_code"].as<String>();
      _city             = weather["data"][0]["city_name"].as<String>();
    }
    httpData = "";
    _currentUpdated = now();
    Serial.print("Current weather updated successfully at: ");
    Serial.printf("%02d:%02d:%02d\r\n", hour(), minute(), second());
  }
  else {
    Serial.println("Current weather update error");
    _errorUpdate = now();
  }
  clientWeather.end();
  
  _updateDaily();
}

/**
 * Weatherbit icon code conversion
 */
unsigned int Weather::_weatherbit_icon(int code) {
  unsigned int icon = 4;
  if(code >= 200 and code < 300) icon = 11;
  if(code >= 300 and code < 400) icon = 9;
  if(code >= 500 and code < 600) icon = 10;
  if(code >= 600 and code < 700) icon = 13;
  if(code >= 700 and code < 800) icon = 50;
  if(code == 800) icon = 1;
  if(code == 801 or code == 802) icon = 2;
  if(code == 803) icon = 3;
  if(code >= 804) icon = 4;
  return icon;
}

/**
 * Update daily forecast
 */
void Weather::_updateDaily(void) {
  Serial.println(SEPARATOR);
  Serial.println("Daily forecast update... ");
  String url = "";

  if(config.weather_provider() == OPEN_METEO) {
    url = "http://api.open-meteo.com/v1/forecast";
    url += "?latitude=" + config.weather_lat();
    url += "&longitude=" + config.weather_lon();
    url += "&daily=weather_code,temperature_2m_max,temperature_2m_min,wind_speed_10m_max";
    url += "&wind_speed_unit=ms&timeformat=unixtime&timezone=auto&forecast_days=3";
  }
  
  if(config.weather_provider() == WEATHERBIT) {
    url = "http://api.weatherbit.io/v2.0/forecast/daily?days=3";
    url += "&lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
    url += "&key=" + config.weather_appid(WEATHERBIT);
  }
  
  HTTPClient clientDaily;
  Serial.println(url);
  clientDaily.begin(wifiClient, url);
  int httpCode = clientDaily.GET();
  if(httpCode == HTTP_CODE_OK) {
    String httpData = clientDaily.getString();
    Serial.println(httpData);
    DynamicJsonDocument forecast(8192);
    DeserializationError errorForecast = deserializeJson(forecast, httpData);
    if(errorForecast) {
      Serial.println("Daily forecast deserialization error");
      return;
    }
    if(config.weather_provider() == OPEN_METEO) {
      for(unsigned int i=0; i<DAYS; i++) {
        _dailyDayTemp[i]   = forecast["daily"]["temperature_2m_max"][i] | 40400.0;
        _dailyNightTemp[i] = forecast["daily"]["temperature_2m_min"][i] | 40400.0;
        _dailyWindSpeed[i] = forecast["daily"]["wind_speed_10m_max"][i] | -1.0;
        _dailyIcon[i]      = _openMeteoIcon(forecast["daily"]["weather_code"][i] | 0);
      }
    }
    if(config.weather_provider() == WEATHERBIT) {
      for(unsigned int i=0; i<DAYS; i++) {
        _dailyDayTemp[i]   = forecast["data"][i]["high_temp"] | 40400.0;
        _dailyNightTemp[i] = forecast["data"][i]["min_temp"] | 40400.0;
        _dailyIcon[i]      = _weatherbit_icon(forecast["data"][i]["weather"]["code"].as<int>() | 0);
        _dailyWindSpeed[i] = forecast["data"][i]["wind_spd"] | -1.0;
      }
    }
    httpData = "";
    _dailyUpdated = now();
    Serial.print("Daily forecast updated successfully at: ");
    Serial.printf("%02d:%02d:%02d\r\n", hour(), minute(), second());
  }
  else Serial.println("Daily forecast update error");
  clientDaily.end();
}

void Weather::reset_errorUpdate() {
  _errorUpdate = 0;
}

/**
 * Getters 
 */
float Weather::get_currentTemp() {
  return _currentTemp;
}

float Weather::get_currentHum() {
  return _currentHum;
}

float Weather::get_currentPres() {
  return _currentPres;
}

float Weather::get_currentWindSpeed() {
  return _currentWindSpeed;
}

int Weather::get_currentWindDir() {
  return _currentWindDir;
}

unsigned int Weather::get_currentIcon() {
  return _currentIcon;
}

bool Weather::get_isDay() {
  return _is_day;
}

String Weather::get_description() {
  return _description;
}

unsigned int Weather::get_currentUpdated() {
  return _currentUpdated;
}

unsigned int Weather::get_errorUpdate() {
  return _errorUpdate;
}

float Weather::get_dailyDayTemp(unsigned int num) {
  if(num >= DAYS) return 40400.0;
  return _dailyDayTemp[num];
}

float Weather::get_dailyNightTemp(unsigned int num) {
  if(num >= DAYS) return 40400.0;
  return _dailyNightTemp[num];
}

float Weather::get_dailyWindSpeed(unsigned int num) {
  if(num >= DAYS) return -1.0;
  return _dailyWindSpeed[num];
}

unsigned int Weather::get_dailyIcon(unsigned int num) {
  if(num >= DAYS) return 0;
  return _dailyIcon[num];
}

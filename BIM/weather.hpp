class Weather {
  #define OPENWEATHERMAP 0
  #define WEATHERBIT 1
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
    void reset_errorUpdate();

    float get_dailyDayTemp(unsigned int num);
    float get_dailyNightTemp(unsigned int num);
    float get_dailyWindSpeed(unsigned int num);
    unsigned int get_dailyIcon(unsigned int num); 

  private:
    String _description = "----";
    unsigned int _currentDate = 0;
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
};

/**
 * Check if wind speed is within the normal range
 */
bool Weather::checkWind(float wind) {
  return (wind >= 0.0 and wind <= 99.0);
}

/**
 * Weather update
 */
void Weather::update() {
  String url = "";
  if(config.weather_provider() == OPENWEATHERMAP) {
    if(config.weather_appid(OPENWEATHERMAP) == "") {
      Serial.println("No APPID");
      _errorUpdate = now();
      return;
    }
    url = "http://api.openweathermap.org/data/2.5/weather?appid=" + config.weather_appid(OPENWEATHERMAP);
    if(config.weather_citysearch() == 0) {
      if(config.weather_city() == "") {
        Serial.println("No Cityname");
        _errorUpdate = now();
        return;
      }
      url += "&q=" + config.weather_city();
    }
    if(config.weather_citysearch() == 1) {
      if(config.weather_cityid() == "") {
        Serial.println("No City ID");
        _errorUpdate = now();
        return;
      }
      url += "&id=" + config.weather_cityid();
    }
    if(config.weather_citysearch() == 2) {
      if(config.weather_lat() == "" || config.weather_lon() == "") {
        Serial.println("No Coordinates");
        _errorUpdate = now();
        return;
      }
      url += "&lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
    }
    url += "&units=metric&lang=" + config.lang();
  }
  else if(config.weather_provider() == WEATHERBIT) {
    url = "http://api.weatherbit.io/v2.0/current?key=" + config.weather_appid(WEATHERBIT);
    if(config.weather_citysearch() == 0) {
      if(config.weather_city() == "") {
        Serial.println("No Cityname");
        _errorUpdate = now();
        return;
      }
      url += "&city=" + config.weather_city();
    }
    if(config.weather_citysearch() == 1) {
      if(config.weather_cityid() == "") {
        Serial.println("No City ID");
        _errorUpdate = now();
        return;
      }
      url += "&city_id=" + config.weather_cityid();
    }
    if(config.weather_citysearch() == 2) {
      if(config.weather_lat() == "" || config.weather_lon() == "") {
        Serial.println("No Coordinates");
        _errorUpdate = now();
        return;
      }
      url += "&lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
    }
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
    if(config.weather_provider() == OPENWEATHERMAP) {
      _description      = weather["weather"][0]["description"].as<String>();
      _currentTemp      = weather["main"]["temp"] | 40400.0;
      _currentHum       = weather["main"]["humidity"] | 40400.0;
      _currentPres      = weather["main"]["pressure"] | 40400.0;
      _currentWindSpeed = weather["wind"]["speed"] | -1.0;
      _currentWindDir   = weather["wind"]["deg"] | -1;
      _currentIcon      = atoi(weather["weather"][0]["icon"]);
      String pod        = weather["weather"][0]["icon"] | "";
      _is_day           = (pod.substring(2) == "d") ? true : false;
      _country          = weather["sys"]["country"].as<String>();
      _city             = weather["name"].as<String>();
      _currentDate      = weather["dt"];
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

  if(config.weather_provider() == OPENWEATHERMAP) {
    String location = "";
    if(config.weather_citysearch() == 0) location = "?q=" + config.weather_city();
    if(config.weather_citysearch() == 1) location = "?id=" + config.weather_cityid();
    if(config.weather_citysearch() == 2) location = "?lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
    url = config.weather_parsingServer();
    url += location;
    url += "&provider=" + String(config.weather_provider());
    url += "&appid=" + config.weather_appid(config.weather_provider());
    url += "&mac=" + WiFi.macAddress();
  }
  
  if(config.weather_provider() == WEATHERBIT) {
    url = "http://api.weatherbit.io/v2.0/forecast/daily?days=4";
    if(config.weather_citysearch() == 0) url += "&city=" + config.weather_city();
    if(config.weather_citysearch() == 1) url += "&city_id=" + config.weather_cityid();
    if(config.weather_citysearch() == 2) url += "&lat=" + config.weather_lat() + "&lon=" + config.weather_lon();
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
    if(config.weather_provider() == OPENWEATHERMAP) {
      for(unsigned int i=0; i<DAYS; i++) {
        _dailyDayTemp[i]   = forecast["daily"]["dayTemp"][i] | 40400.0;
        _dailyNightTemp[i] = forecast["daily"]["nightTemp"][i] | 40400.0;
        _dailyIcon[i]      = forecast["daily"]["icon"][i] | 0;
        _dailyWindSpeed[i] = forecast["daily"]["windSpeed"][i] | -1.0;
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

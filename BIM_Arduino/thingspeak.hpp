class Thingspeak {
  public:
    void receive();
    void send();
    float get_field(unsigned int num);
    unsigned int get_updated();  

  private:
    String _fieldPrepare(unsigned int field);
    float _field[8] = {40400.0, 40400.0, 40400.0, 40400.0, 40400.0, 40400.0, 40400.0, 40400.0};
    unsigned int _updated = 0;
};

/**
 * Receive data from thingspeak
 */
void Thingspeak::receive() {
  if(config.thingspeakReceive_channelID() == "") {
    Serial.println("No Channel ID");
    return;
  }
  if(config.thingspeakReceive_rdkey() == "") {
    Serial.println("No Read API Key");
    return;
  }
  String url = "http://api.thingspeak.com/channels/" + config.thingspeakReceive_channelID();
  url += "/feeds.json?api_key=" + config.thingspeakReceive_rdkey() + "&results=1";
  String httpData = "";
  HTTPClient client;
  Serial.println(url);
  client.begin(wifiClient, url);
  int httpCode = client.GET();
  if(httpCode == HTTP_CODE_OK) {
    httpData = client.getString();
    Serial.println(httpData);
    StaticJsonDocument<2048> root;
    DeserializationError error = deserializeJson(root, httpData);
    if(error) {
      Serial.println("Deserialization error");
      return;
    }
    String thing_tm  = root["feeds"][0]["created_at"];
    _field[0] = root["feeds"][0]["field1"];
    _field[1] = root["feeds"][0]["field2"];
    _field[2] = root["feeds"][0]["field3"];
    _field[3] = root["feeds"][0]["field4"];    
    _field[4] = root["feeds"][0]["field5"];
    _field[5] = root["feeds"][0]["field6"];
    _field[6] = root["feeds"][0]["field7"];
    _field[7] = root["feeds"][0]["field8"];
    TimeElements tmth;
    char buf[22];
    thing_tm.toCharArray(buf, 22);
    tmth.Year = atoi(strtok(buf, "-")) - 1970;
    tmth.Month = atoi(strtok(NULL, "-"));
    tmth.Day = atoi(strtok(NULL, "T"));
    tmth.Hour = atoi(strtok(NULL, ":"));
    tmth.Minute = atoi(strtok(NULL, ":"));
    tmth.Second = atoi(strtok(NULL, ":"));
    _updated = makeTime(tmth);
    _updated += config.clock_utc() * 3600;
    _updated += config.clock_dlst() ? timentp.is_summertime() ? 3600 : 0 : 0;
    Serial.printf("successfully updated at %02d:%02d:%02d\r\n", hour(), minute(), second());
  }
  else Serial.println("error, code: " + String(httpCode));
  client.end();
}

/**
 * Send data to thingspeak
 */
void Thingspeak::send() {
  if(config.thingspeakSend_wrkey() == "") {
    Serial.println("No Write API Key");
    return;
  }
  
  String url = "http://api.thingspeak.com/update?api_key=" + config.thingspeakSend_wrkey();
  for(unsigned int i=0; i<8; i++) {
    url += _fieldPrepare(i);
  }
  
  String httpData = "";
  HTTPClient client;
  Serial.println(url);
  client.begin(wifiClient, url);
  int httpCode = client.GET();
  if(httpCode == HTTP_CODE_OK) {
    httpData = client.getString();
    Serial.println(httpData);
    Serial.println("successfull");
  }
  else Serial.println("error, code: " + String(httpCode));
  client.end();
  httpData = "";
}

/**
 * Get data from a field
 * @param field number
 * @return field data or obviously erroneous value
 */
float Thingspeak::get_field(unsigned int num) {
  if(num > 7) return -40400.0;
  return _field[num];
}

/**
 * Get timestamp of last update
 * @return timestamp
 */
unsigned int Thingspeak::get_updated() {
  return _updated;
}

/**
 * Prepare data for the desired field according to the settings
 * @param field number
 * @return a string with field name and data
 */
String Thingspeak::_fieldPrepare(unsigned int fieldNum) {
  String fields = "";
  String field = "&field" + String(fieldNum + 1) + "=";
  switch(config.thingspeakSend_fields(fieldNum)) {
    case 0: ; break; // --

    case 1: { // Weather
      // Temperature
      if(config.thingspeakSend_types(fieldNum) == 0 and sensors.checkTemp(weather.get_currentTemp(config.weather_temp_corr)))
        fields = field + String(weather.get_currentTemp(config.weather_temp_corr));
      // Humidity
      if(config.thingspeakSend_types(fieldNum) == 1 and sensors.checkHum(weather.get_currentHum(config.weather_hum_corr)))
        fields = field + String(weather.get_currentHum(config.weather_hum_corr));
      // Pressure
      if(config.thingspeakSend_types(fieldNum) == 2 and sensors.checkPres(weather.get_currentPres(config.weather_pres_corr))) 
        fields = field + String(weather.get_currentPres(config.weather_pres_corr));
    }; break;
    
    case 2: { // BME280
      // Temperature
      if(config.thingspeakSend_types(fieldNum) == 0 and 
        sensors.checkTemp(sensors.get_bme280_temp(config.bme280_temp_corr()))) 
          fields = field + String(sensors.get_bme280_temp(config.bme280_temp_corr()));
      // Humidity
      if(config.thingspeakSend_types(fieldNum) == 1 and 
        sensors.checkHum(sensors.get_bme280_hum(config.bme280_hum_corr())))
          fields = field + String(sensors.get_bme280_hum(config.bme280_hum_corr()));
      // Pressure
      if(config.thingspeakSend_types(fieldNum) == 2 and
        sensors.checkPres(sensors.get_bme280_pres(config.bme280_pres_corr())))
          fields = field + String(sensors.get_bme280_pres(config.bme280_pres_corr()));
    }; break;
    
    case 3: { // BMP180
      // Temperature
      if(config.thingspeakSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_bmp180_temp(config.bmp180_temp_corr()))) 
          fields = field + String(sensors.get_bmp180_temp(config.bmp180_temp_corr()));
      // Pressure
      if(config.thingspeakSend_types(fieldNum) == 1 and
        sensors.checkPres(sensors.get_bmp180_pres(config.bmp180_pres_corr()))) 
          fields = field + String(sensors.get_bmp180_pres(config.bmp180_pres_corr()));
    }; break;
    
    case 4: { // SHT21
      // Temperature
      if(config.thingspeakSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_sht21_temp(config.sht21_temp_corr()))) 
          fields = field + String(sensors.get_sht21_temp(config.sht21_temp_corr()));
      // Humidity
      if(config.thingspeakSend_types(fieldNum) == 1 and
        sensors.checkHum(sensors.get_sht21_hum(config.sht21_hum_corr())))
          fields = field + String(sensors.get_sht21_hum(config.sht21_hum_corr()));
    }; break;
    
    case 5: { // DHT22
      // Temperature
      if(config.thingspeakSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_dht22_temp(config.dht22_temp_corr()))) 
          fields = field + String(sensors.get_dht22_temp(config.dht22_temp_corr()));
      // Humidity
      if(config.thingspeakSend_types(fieldNum) == 1 and
        sensors.checkHum(sensors.get_dht22_hum(config.dht22_hum_corr()))) 
          fields = field + String(sensors.get_dht22_hum(config.dht22_hum_corr()));
    }; break;
    
    case 6: // DS18B20
      // Temperature
      if(sensors.checkTemp(sensors.get_ds18b20_temp(config.ds18b20_temp_corr())))
          fields = field + String(sensors.get_ds18b20_temp(config.ds18b20_temp_corr()));
      break;
    
    case 7: // MAX44009
      // Ambient light
      if(sensors.checkLight(sensors.get_max44009_light(config.max44009_light_corr())))
          fields = field + String(sensors.get_max44009_light(config.max44009_light_corr()));
      break;
    
    case 8: // BH1750
      // Ambient light
      if(sensors.checkLight(sensors.get_bh1750_light(config.bh1750_light_corr())))
          fields = field + String(sensors.get_bh1750_light(config.bh1750_light_corr()));
      break;

    case 9: // Runtime
      fields = field + String(millis());
      break;
    
    default: ; break; 
  }
  return fields;
}

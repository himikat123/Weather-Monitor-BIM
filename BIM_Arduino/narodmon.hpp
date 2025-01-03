class Narodmon {
  public: void send();
  private: String _fieldsPrepare(unsigned int fieldNum, String metrics, String mac);
};

/**
 * Send data to norodmon
 */
void Narodmon::send() {
  WiFiClient client;
  
  if(config.narodmonSend_lat() == "" or config.narodmonSend_lon() == "") {
    Serial.println("No coordinates");
    return;
  }

  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String buf = "#BIM" + mac;
  buf += "#" + config.narodmonSend_name();
  buf += "#" + config.narodmonSend_lat();
  buf += "#" + config.narodmonSend_lon();
  for(unsigned int i=0; i<12; i++) {
    buf += _fieldsPrepare(i, config.narodmonSend_metrics(i), mac); 
  }
  buf += "\n##\r\n";
  if(!client.connect("narodmon.ru", 8283)) Serial.println("failed");
  else {
    Serial.println(buf);
    client.print(buf);
    Serial.println("Done");
  }
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println("Successfull. Server returned " + line);
  }
  client.stop();
}

/**
 * Prepare data for the desired field according to the settings
 * @param field number
 * @return a string with field name and data
 */
String Narodmon::_fieldsPrepare(unsigned int fieldNum, String metrics, String mac) {
  String fields = "";
  String field = "\n#" + metrics + "_" + mac + "#";
  switch(config.narodmonSend_sensors(fieldNum)) {
    case 0: ; break; // --
    
    case 1: { // Weather
      // Temperature
      if(config.narodmonSend_types(fieldNum) == 0 and sensors.checkTemp(weather.get_currentTemp(config.weather_temp_corr)))
        fields = field + String(weather.get_currentTemp(config.weather_temp_corr));
      // Humidity
      if(config.narodmonSend_types(fieldNum) == 1 and sensors.checkHum(weather.get_currentHum(config.weather_hum_corr)))
        fields = field + String(weather.get_currentHum(config.weather_hum_corr));
      // Pressure
      if(config.narodmonSend_types(fieldNum) == 2 and sensors.checkPres(weather.get_currentPres(config.weather_pres_corr))) 
        fields = field + String(weather.get_currentPres(config.weather_pres_corr));
    }; break;
    
    case 2: { // BME280
      // Temperature
      if(config.narodmonSend_types(fieldNum) == 0 and 
        sensors.checkTemp(sensors.get_bme280_temp(config.bme280_temp_corr()))) 
          fields = field + String(sensors.get_bme280_temp(config.bme280_temp_corr()));
      // Humidity
      if(config.narodmonSend_types(fieldNum) == 1 and 
        sensors.checkHum(sensors.get_bme280_hum(config.bme280_hum_corr())))
          fields = field + String(sensors.get_bme280_hum(config.bme280_hum_corr()));
      // Pressure
      if(config.narodmonSend_types(fieldNum) == 2 and
        sensors.checkPres(sensors.get_bme280_pres(config.bme280_pres_corr())))
          fields = field + String(sensors.get_bme280_pres(config.bme280_pres_corr()));
    }; break;
    
    case 3: { // BMP180
      // Temperature
      if(config.narodmonSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_bmp180_temp(config.bmp180_temp_corr()))) 
          fields = field + String(sensors.get_bmp180_temp(config.bmp180_temp_corr()));
      // Pressure
      if(config.narodmonSend_types(fieldNum) == 1 and
        sensors.checkPres(sensors.get_bmp180_pres(config.bmp180_pres_corr()))) 
          fields = field + String(sensors.get_bmp180_pres(config.bmp180_pres_corr()));
    }; break;
    
    case 4: { // SHT21
      // Temperature
      if(config.narodmonSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_sht21_temp(config.sht21_temp_corr()))) 
          fields = field + String(sensors.get_sht21_temp(config.sht21_temp_corr()));
      // Humidity
      if(config.narodmonSend_types(fieldNum) == 1 and
        sensors.checkHum(sensors.get_sht21_hum(config.sht21_hum_corr())))
          fields = field + String(sensors.get_sht21_hum(config.sht21_hum_corr()));
    }; break;
    
    case 5: { // DHT22
      // Temperature
      if(config.narodmonSend_types(fieldNum) == 0 and
        sensors.checkTemp(sensors.get_dht22_temp(config.dht22_temp_corr()))) 
          fields = field + String(sensors.get_dht22_temp(config.dht22_temp_corr()));
      // Humidity
      if(config.narodmonSend_types(fieldNum) == 1 and
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

    case 9:{ // Runtime
      fields = field + String(millis());
    }; break;
    
    default: ; break; 
  }
  return fields;
}

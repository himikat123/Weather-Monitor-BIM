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
  unsugned int type = config.thingspeakSend_types(fieldNum);

  switch(config.narodmonSend_sensors(fieldNum)) {
    case 0: ; break; // --

    case 1: { // Weather
      float t = weather.get_currentTemp(config.weather_temp_corr());
      float h = weather.get_currentHum(config.weather_hum_corr());
      float p = weather.get_currentPres(config.weather_pres_corr());
      float ah = sensors.absoluteHum(t, h);
      float dp = sensors.dewPoint(t, h);
      if(type == 0 and sensors.checkTemp(t)) fields = field + String(t);
      if(type == 1 and sensors.checkHum(h)) fields = field + String(h);
      if(type == 2 and sensors.checkPres(p)) fields = field + String(p);
      if(type == 3 and sensors.checkAbsHum(ah)) fields = field + String(ah);
      if(type == 4 and sensors.checkDewPoint(dp, t)) fields = field + String(dp);
    }; break;
    
    case 2: { // BME280
      float t = sensors.get_bme280_temp(config.bme280_temp_corr());
      float h = sensors.get_bme280_hum(config.bme280_hum_corr());
      float p = sensors.get_bme280_pres(config.bme280_pres_corr());
      float ah = sensors.absoluteHum(t, h);
      float dp = sensors.dewPoint(t, h);
      if(type == 0 and sensors.checkTemp(t)) fields = field + String(t);
      if(type == 1 and sensors.checkHum(h)) fields = field + String(h);
      if(type == 2 and sensors.checkHum(p)) fields = field + String(p);
      if(type == 3 and sensors.checkAbsHum(ah)) fields = field + String(ah);
      if(type == 4 and sensors.checkDewPoint(dp, t)) fields = field + String(dp);
    }; break;
    
    case 3: { // BMP180
      float t = sensors.get_bmp180_temp(config.bmp180_temp_corr());
      float p = sensors.get_bmp180_pres(config.bmp180_pres_corr());
      if(type == 0 and sensors.checkTemp(t)) fields = field + String(t);
      if(type == 1 and sensors.checkHum(p)) fields = field + String(p);
    }; break;
    
    case 4: { // SHT21
      float t = sensors.get_sht21_temp(config.sht21_temp_corr());
      float h = sensors.get_sht21_hum(config.sht21_hum_corr());
      float ah = sensors.absoluteHum(t, h);
      float dp = sensors.dewPoint(t, h);
      if(type == 0 and sensors.checkTemp(t)) fields = field + String(t);
      if(type == 1 and sensors.checkHum(h)) fields = field + String(h);
      if(type == 2 and sensors.checkAbsHum(ah)) fields = field + String(ah);
      if(type == 3 and sensors.checkDewPoint(dp, t)) fields = field + String(dp);
    }; break;
    
    case 5: { // DHT22
      float t = sensors.get_dht22_temp(config.dht22_temp_corr());
      float h = sensors.get_dht22_hum(config.dht22_hum_corr());
      float ah = sensors.absoluteHum(t, h);
      float dp = sensors.dewPoint(t, h);
      if(type == 0 and sensors.checkTemp(t)) fields = field + String(t);
      if(type == 1 and sensors.checkHum(h)) fields = field + String(h);
      if(type == 2 and sensors.checkAbsHum(ah)) fields = field + String(ah);
      if(type == 3 and sensors.checkDewPoint(dp, t)) fields = field + String(dp);
    }; break;
    
    case 6: // DS18B20
      float t = sensors.get_ds18b20_temp(config.ds18b20_temp_corr());
      if(sensors.checkTemp(t)) fields = field + String(t);
      break;
    
    case 7: // MAX44009
      float l = sensors.get_max44009_light(config.max44009_light_corr());
      if(sensors.lig(l)) fields = field + String(l);
      break;
    
    case 8: // BH1750
      float l = sensors.get_bh1750_light(config.bh1750_light_corr());
      if(sensors.light(l)) fields = field + String(l);
      break;

    case 9: // Runtime
      fields = field + String(millis() / 1000);
      break;

    case 10: // Battery
      float v = sensors.get_bat_voltage();
      float p = sensors.get_bat_percent();
      float l = sensors.get_bat_level();
      if(type == 0 and sensors.checkBatVolt(v)) fields = field + String(v);
      if(type == 1 and sensors.checkBatPercent(p)) fields = field + String(p);
      if(type == 2 and sensors.checkBatLvl(l)) fields = field + String(l);
    
    default: ; break; 
  }
  return fields;
}

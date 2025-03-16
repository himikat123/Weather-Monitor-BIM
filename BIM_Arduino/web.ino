double web_code = 0;
String web_filelist = "";
size_t fsUsed = 0;
size_t fsTotal = 0;

/**
 * Get current date and time as a string
 */
String web_timeString(unsigned int tm) {
  char buf[20];
  sprintf(buf, "%02d:%02d:%02d %02d.%02d.%d", hour(tm), minute(tm), second(tm), day(tm), month(tm), year(tm));
  return String(buf);
}

/**
 * Prepare json string data
 */
String web_jsonStr(String key, String val, uint8_t prefix, bool comma) {
  String prefx = "";
  for(uint8_t i=0; i<prefix; i++) prefx += "\t";
  return prefx + "\"" + key + "\": \"" + val + "\"" + (comma ? "," : "") + "\n";
}

/**
 * Prepare json float data
 */
String web_jsonFloat(String key, float val, uint8_t prefix, bool comma) {
  String prefx = "";
  for(uint8_t i=0; i<prefix; i++) prefx += "\t";
  return prefx + "\"" + key + "\": " + String(val) + (comma ? "," : "") + "\n";
}

/**
 * Prepare json integer data
 */
String web_jsonInt(String key, int val, uint8_t prefix, bool comma) {
  String prefx = "";
  for(uint8_t i=0; i<prefix; i++) prefx += "\t";
  return prefx + "\"" + key + "\": " + String(val) + (comma ? "," : "") + "\n";
}

/**
 * List all files in directory
 */
void web_listAllFilesInDir(String dir_path) {
  Dir dir = LittleFS.openDir(dir_path);
  
  while(dir.next()) {
    if(dir.isFile()) {
      File file = dir.openFile("r");
      if(dir.fileName() != "user.us") {
        web_filelist += dir_path + dir.fileName() + ": ";
        web_filelist += String(file.size()) + ",";
      }
    }
    
    if(dir.isDirectory()) {
      web_listAllFilesInDir(dir_path + dir.fileName() + "/");
    }
  }
}

/**
 * Checks if the user is logged in
 */
bool web_isLogged(bool answer) {
  bool logged = false;
  if(!config.account_required()) logged = true;
  else {
    if(webServer.hasArg("code")) {
      String auth = webServer.arg("code");
      if(auth == String(web_code)) logged = true;
      else logged = false;
    }
    else logged = false;
  }

  if(!logged && answer) webServer.send(200, "text/plain", "NOT LOGGED IN");
  return logged;
}

String web_getContentType(String filename) {
  if(webServer.hasArg("download"))    return "application/octet-stream";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".json")) return "text/json";
  else if(filename.endsWith(".jpg"))  return "image/jpeg";
  else if(filename.endsWith(".css"))  return "text/css";
  else if(filename.endsWith(".js"))   return "application/javascript";
  else if(filename.endsWith(".gz"))   return "application/x-gzip";
  return "text/plain";
}

bool web_handleFileRead(String path) {
  if(!path.endsWith(".jpg") && !path.endsWith(".css") && !path.endsWith(".js")) {
    if(path != "/config.json" && path != "/defaultConfig.json") path = "/index.html";
  }
  
  String contentType = web_getContentType(path);
  String pathWithGz = path + ".gz";

  if(LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
    if(LittleFS.exists(pathWithGz)) path += ".gz";
    File file = LittleFS.open(path, "r");
    if(path.endsWith(".css.gz") || path.endsWith(".js.gz")) webServer.sendHeader("Cache-Control", "max-age=31536000");
    webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

/**
 * Upload a file
 */
void web_handleFileUpload() {
  HTTPUpload& upload = webServer.upload();
  if(upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/" + filename;
    fsUploadFile = LittleFS.open(filename, "w");
    filename = String();
  }
  else if(upload.status == UPLOAD_FILE_WRITE) {
    if(fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if(upload.status == UPLOAD_FILE_END) {
    if(fsUploadFile) fsUploadFile.close();
  }

  global.fsInfoUpdate = true;
}

/**
 * Initialise web interface
 */
void webInterface_init(void) {
  web_code = ESP8266TrueRandom.random();

  /**
   * Save config data to config file
   */
  webServer.on("/esp/saveConfig", HTTP_POST, []() {
    if(web_isLogged(true)) {
      if(webServer.hasArg("config")) {
        String conf = webServer.arg("config");
        if(conf.length())
          webServer.send(200, "text/plain", config.save(conf) ? "SAVE ERROR" : "OK");
        else webServer.send(200, "text/plain", "DATA ERROR");
      }
      else webServer.send(200, "text/plain", "ARG ERROR");
    }
  });

  /**
   * Send sensors data
   */
  webServer.on("/data.json", HTTP_GET, [] () {
    String data = "{\n";
    
    // System
    data += web_jsonStr("state", web_isLogged(false) ? "OK" : "LOGIN", 1, true);
    data += web_jsonStr("fw", global.fw, 1, true);
    data += web_jsonInt("runtime", round(millis() / 1000), 1, true);
    data += web_jsonInt("heap", ESP.getFreeHeap(), 1, true);
    data += web_jsonInt("time", now(), 1, true);
    
    // List of available networks  
    data += "\t\"ssids\": [\n";
    for(unsigned int i=0; i<global.nets; i++) {
      data += "\t\t[\""; data += global.ssids[i]; 
      data += "\", "; data += String(global.rssis[i]); 
      data += (i < global.nets - 1) ? "],\n" : "]\n";
    }
    data += "\t],\n";

    // Network
    data += "\t\"network\": {\n";
    data += web_jsonStr("ssid", global.apMode ? config.accessPoint_ssid() : WiFi.SSID(), 2, true);
    data += web_jsonInt("ch", WiFi.channel(), 2, true);
    data += web_jsonInt("sig", WiFi.RSSI(), 2, true);
    data += web_jsonStr("mac", WiFi.macAddress(), 2, true);
    data += web_jsonStr("ip", global.apMode ? config.accessPoint_ip() : WiFi.localIP().toString(), 2, true);
    data += web_jsonStr("mask", WiFi.subnetMask().toString(), 2, true);
    data += web_jsonStr("gw", WiFi.gatewayIP().toString(), 2, true);
    data += web_jsonStr("dns1", WiFi.dnsIP().toString(), 2, true);
    data += web_jsonStr("dns2", WiFi.dnsIP().toString(), 2, false);
    data += "\t},\n";

    // BME280
    data += "\t\"bme280\": {\n";
    data += web_jsonFloat("temp", round(sensors.get_bme280_temp(0) * 10) / 10, 2, true);
    data += web_jsonFloat("hum", round(sensors.get_bme280_hum(0) * 10) / 10, 2, true);
    data += web_jsonFloat("pres", round(sensors.get_bme280_pres(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // BMP180
    data += "\t\"bmp180\": {\n";
    data += web_jsonFloat("temp", round(sensors.get_bmp180_temp(0) * 10) / 10, 2, true);
    data += web_jsonFloat("pres", round(sensors.get_bmp180_pres(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // SHT21
    data += "\t\"sht21\": {\n";
    data += web_jsonFloat("temp", round(sensors.get_sht21_temp(0) * 10) / 10, 2, true);
    data += web_jsonFloat("hum", round(sensors.get_sht21_hum(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // DHT22
    data += "\t\"dht22\": {\n";
    data += web_jsonFloat("temp", round(sensors.get_dht22_temp(0) * 10) / 10, 2, true);
    data += web_jsonFloat("hum", round(sensors.get_dht22_hum(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // DS18B20
    data += "\t\"ds18b20\": {\n";
    data += web_jsonFloat("temp", round(sensors.get_ds18b20_temp(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // MAX44009
    data += "\t\"max44009\": {\n";
    data += web_jsonFloat("light", round(sensors.get_max44009_light(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // BH1750
    data += "\t\"bh1750\": {\n";
    data += web_jsonFloat("light", round(sensors.get_bh1750_light(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // Analog photoresistor
    data += "\t\"analog\": {\n";
    data += web_jsonFloat("volt", round(sensors.get_analog_voltage(0) * 10) / 10, 2, false);
    data += "\t},\n";
    
    // Weather
    data += "\t\"weather\": {\n";
    data += web_jsonFloat("temp", round(weather.get_currentTemp(0) * 10) / 10, 2, true);
    data += web_jsonFloat("hum", round(weather.get_currentHum(0) * 10) / 10, 2, true);
    data += web_jsonFloat("pres", round(weather.get_currentPres(0) * 10) / 10, 2, true);
    data += web_jsonInt("icon", weather.convertIcon(weather.get_currentIcon()), 2, true);
    data += web_jsonInt("isDay", weather.get_isDay(), 2, true);
    data += "\t\t\"wind\": {\n";
    data += web_jsonFloat("speed", round(weather.get_currentWindSpeed() * 10) / 10, 3, true);
    data += web_jsonFloat("dir", weather.get_currentWindDir(), 3, false);
    data += "\t\t},\n";
    data += web_jsonStr("descript", weather.get_description(), 2, true);
    data += web_jsonStr("time", String(weather.get_currentUpdated()), 2, true);
    data += "\t\t\"daily\": {\n";
    data += "\t\t\t\"tMax\": [";
    for(unsigned int i=0; i<3; i++) {
      data += String(weather.get_dailyDayTemp(i));
      data += (i < 2) ? ", " : "],\n";
    }
    data += "\t\t\t\"tMin\": [";
    for(unsigned int i=0; i<3; i++) {
      data += String(weather.get_dailyNightTemp(i));
      data += (i < 2) ? ", " : "],\n";
    }
    data += "\t\t\t\"wind\": [";
    for(unsigned int i=0; i<3; i++) {
      data += String(weather.get_dailyWindSpeed(i));
      data += (i < 2) ? ", " : "],\n";
    }
    data += "\t\t\t\"icon\": [";
    for(unsigned int i=0; i<3; i++) {
      data += String(weather.convertIcon(weather.get_dailyIcon(i)));
      data += (i < 2) ? ", " : "]";
    }
    data += "\n\t\t}\n\t},\n";
    
    // Thingspeak
    data += "\t\"thing\": {\n";
    data += "\t\t\"data\": [";
    for(unsigned int i=0; i<8; i++) {
      data += String(thingspeak.get_field(i));
      data += (i < 7) ? ", " : "],\n";
    }
    data += web_jsonInt("time", thingspeak.get_updated(), 2, false);
    data += "\t},\n";

    // File system
    if(global.fsInfoUpdate) {
      FSInfo fsInfo;
      LittleFS.info(fsInfo);
      web_filelist = String();
      web_listAllFilesInDir("/");
        fsTotal = fsInfo.totalBytes;
        fsUsed = fsTotal - fsInfo.usedBytes;
        global.fsInfoUpdate = false;
    }
    data += "\t\"fs\": {\n";
    data += "\t\t\"list\": \"";
    data += web_filelist;
    data += "\",\n";
    data += web_jsonFloat("total", fsTotal, 2, true);
    data += web_jsonFloat("free", fsUsed, 2, false);
    data += "\t}\n}";
    webServer.send(200, "text/json", data);
    data = String();
  });

  /**
   * Restore default config
   */
  webServer.on("/esp/defaultConfig", HTTP_POST, [] () {
    if(web_isLogged(true)) {
      if(webServer.hasArg("config") && webServer.arg("config") == "default") {
        char ibuffer[64];
        if(LittleFS.exists("/config.json") == true) LittleFS.remove("/config.json");
        File def = LittleFS.open("/defaultConfig.json", "r");
        File cfg = LittleFS.open("/config.json", "w");
        if(def and cfg) {
          while(def.available()) {
            byte i = def.readBytes(ibuffer, 64);
            cfg.write((uint8_t *)ibuffer, i);
          }
          webServer.send(200, "text/plain", "OK");
        }
        cfg.close();
        def.close();
        global.fsInfoUpdate = true;
        webServer.send(200, "text/plain", "error");
      }
      else webServer.send(200, "text/plain", "bad args");
    }
  });

  /**
   * Restart ESP8266
   */
  webServer.on("/esp/restart", HTTP_GET, [] () {
     if(web_isLogged(true)) {
      webServer.send(200, "text/plain", "OK");
      ESP.restart();
    }
  });

  /**
   * Change language
   */
  webServer.on("/esp/changelang", HTTP_GET, [] () {
    if(web_isLogged(true)) {
      if(webServer.hasArg("lang")) {
        config.set_lang(webServer.arg("lang"));
        webServer.send(200, "text/plain", "OK");
      }
      else webServer.send(200, "text/plain", "error");
    }
  });

  /**
   * Brightness adjust
   */
  webServer.on("/esp/bright", HTTP_GET, [] () {
    if(web_isLogged(true)) {
      if(webServer.hasArg("bright")) {
        config.set_bright((webServer.arg("bright")).toInt());
        webServer.send(200, "text/plain", "OK");
      }
      else webServer.send(200, "text/plain", "ERROR");
    }
  });

  /**
   * Ambient light sensor sensitivity adjust
   */
  webServer.on("/esp/sensitivity", HTTP_GET, [] () {
    if(web_isLogged(true)) {
      if(webServer.hasArg("bright")) {
        config.set_sensitivity((webServer.arg("bright")).toInt());
        webServer.send(200, "text/plain", "OK");
      }
      else webServer.send(200, "text/plain", "ERROR");
    }
  });

  /**
   * Initialize clock syncronization with NTP server
   */
  webServer.on("/esp/syncClock", HTTP_GET, [] () {
    if(web_isLogged(true)) {
      if(webServer.hasArg("ntp")) {
        global.clockSynchronized = false;
        webServer.send(200, "text/plain", "OK");
      }
      else {
        setTime((webServer.arg("h")).toInt(), 
                (webServer.arg("i")).toInt(),
                (webServer.arg("s")).toInt(),
                (webServer.arg("d")).toInt(),
                (webServer.arg("m")).toInt(),
                (webServer.arg("y")).toInt());
        webServer.send(200, "text/plain", web_timeString(now()));
      }
    }
  });

  /**
   * Send information about the clock synchronization process
   */
  webServer.on("/esp/syncdialog", HTTP_GET, [] () {
    if(web_isLogged(true)) {
      if(!global.clockSynchronized) webServer.send(200, "text/plain", ".");
      else webServer.send(200, "text/plain", web_timeString(now()));
    }
  });

  /**
   * Change password
   */
  webServer.on("/esp/changePass", HTTP_POST, [] () {
    if(web_isLogged(true)) {
      String oldPass = (webServer.hasArg("oldPass")) ? webServer.arg("oldPass") : "";
      String newPass = (webServer.hasArg("newPass")) ? webServer.arg("newPass") : "";
      String res = "-";
      res = (oldPass == config.account_pass()) ? config.savePassword(newPass) ? "ERROR Write file" : "OK" : "ERROR";
      webServer.send(200, "text/plain", res);
    }
  });

  /**
   * Delete a file
   */
  webServer.on("/esp/delete", HTTP_POST, [] () {
    if(web_isLogged(true)) {
      String path = "/" + webServer.arg("file");
      if(!LittleFS.exists(path)) webServer.send(404, "text/plain", "FileNotFound");
      else {
        LittleFS.remove(path);
        webServer.send(200, "text/plain", "OK");
        path = String();
        global.fsInfoUpdate = true;
      }
    }
  });

  /**
   * Rename a file
   */
  webServer.on("/esp/rename", HTTP_POST, [] () {
    if(web_isLogged(true)) {
      String alt = "/" + webServer.arg("old");
      String neu = "/" + webServer.arg("new");
      if(!LittleFS.exists(alt)) webServer.send(404, "text/plain", "FileNotFound");
      else {
        LittleFS.rename(alt, neu);
        webServer.send(200, "text/plain", "OK");
        alt = String();
        neu = String();
        global.fsInfoUpdate = true;
      }
    }
  });

  /**
   * Login
   * Username and password validation 
   */
  webServer.on("/esp/login", HTTP_POST, [] () {
    String login = webServer.hasArg("name") ? webServer.arg("name") : "";
    String pass = webServer.hasArg("pass") ? webServer.arg("pass") : "";
    bool loged = false;
    
    if(login == config.account_name() and pass == config.account_pass()) {
      loged = true;
      web_code = second() * 66000000 + millis() % 1000000;
    }

    webServer.send(200, "text/plain", loged ? ("OK:" + String(web_code)) : "error:1");
  });

  /**
   * Upload a file in FS
   */
  webServer.on("/esp/fileUpload", HTTP_POST, [] () {
    webServer.send(200, "text/plain", "");
  }, web_handleFileUpload);

  webServer.onNotFound([] () {
    if(!web_handleFileRead(webServer.uri())) webServer.send(404,"text/plain", "FileNotFound");
  });
  
  //const char * headerkeys[] = {"User-Agent", "Cookie"};
  //size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //webServer.collectHeaders(headerkeys, headerkeyssize);
  webServer.begin();
}

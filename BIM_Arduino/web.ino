double web_code = 0;
String web_filelist = "";

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
      web_filelist += "\t\t\t[\"";
      web_filelist += dir_path + dir.fileName() + "\", ";
      web_filelist += String(float(file.size()) / 1024.0) + "],\n";
    }
    
    if(dir.isDirectory()) {
      web_listAllFilesInDir(dir_path + dir.fileName() + "/");
    }
  }
}

/**
 * Checks if the user is logged in
 */
bool web_isLogged() {
  if(webServer.hasHeader("Cookie")) {
    String cookie = webServer.header("Cookie");
    if(cookie.substring(cookie.indexOf("auth") + 5) == String(web_code)) return true;
    else return false;
  }
  
  return false;
}

/**
 * Checks if the user is logged in and sends the requested file, otherwise sends the login page
 */
//bool web_fileRead(AsyncWebServerRequest *request) {
//  String path = request->url();
//  if(!path.endsWith(".ico") && !path.endsWith(".jpg")) {
//    if(path == "/config.json") {
//      if(!web_isLogged(request)) {
//        request->send(200, "text/plain", "{\"lang\": \"" + config.lang() + "\", \"state\": \"LOGIN\"}");
//        return true;
//      }
//    }
//    else path = "/index.html";
//  }
//  String pathWithGz = path + ".gz";
//  if(LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
//    if(LittleFS.exists(pathWithGz)) path += ".gz";
//    AsyncWebServerResponse *response = request->beginResponse(LittleFS, path);
//    if(request->hasArg("download")) response->addHeader("Content-Type", "application/octet-stream");
//    else if(path.endsWith(".json")) response->addHeader("Content-Type", "application/json");
//    else if(path.endsWith(".jpg")) response->addHeader("Content-Type", "image/jpeg");
//    else if(path.endsWith(".ico")) response->addHeader("Content-Type", "image/x-icon");
//    else if(path.endsWith(".gz")) {
//      response->addHeader("Content-Encoding", "gzip");
//      response->addHeader("Content-Type", "text/html");
//    }
//    else response->addHeader("Content-Type", "text/plain");
//    request->send(response);
//    return true;
//  }
//  return false;
//}















String web_getContentType(String filename) {
  if(webServer.hasArg("download"))    return "application/octet-stream";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".json")) return "text/json";
  else if(filename.endsWith(".jpg"))  return "image/jpeg";
  else if(filename.endsWith(".gz"))   return "application/x-gzip";
  return "text/plain";
}

bool web_handleFileRead(String path) {
  //if(webServer.hasHeader("Cookie")) {
    //String cookie = webServer.header("Cookie");
    //int8_t au = cookie.indexOf("auth");
    //uint8_t cook[10]; 
    //uint8_t coincid = 0;
    //uint8_t code_auth[10];
    //ESP.rtcUserMemoryRead(0, (uint32_t*)&code_auth, 10);
    //for(uint8_t i=0; i<10; i++) cook[i] = (uint8_t)cookie[au + 5 + i] - 48;
    //for(uint8_t i=0; i<10; i++) if(code_auth[i] == cook[i]) coincid++;
    //if(au != -1 and coincid == 10 or 
    //   path.endsWith("json") or 
    //   path == "/log-err.htm" or
    //   path == "/ok.htm" or
    /*   path == "/fail.htm")*/ return web_FileRead(path); 
    //else return FileRead("/login.htm"); 
  //}
  //else {
  //  if(path.endsWith("json")) return FileRead(path);
  //  else return FileRead("/login.htm");
  //}
}

bool web_FileRead(String path) {
  if(!path.endsWith(".jpg")) {
    if(path == "/config.json") {
//    if(!web_isLogged(request)) {
//      request->send(200, "text/plain", "{\"lang\": \"" + config.lang() + "\", \"state\": \"LOGIN\"}");
//      return true;
//    }
    }
    else path = "/index.html";
  }
  
  String contentType = web_getContentType(path);
  String pathWithGz = path + ".gz";

  if(LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
    if(LittleFS.exists(pathWithGz)) path += ".gz";
    File file = LittleFS.open(path, "r");
    size_t sent = webServer.streamFile(file, contentType);
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

  web_filelist = String();
  web_listAllFilesInDir("/");
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
    if(web_isLogged()) {
      if(webServer.hasArg("config")) {
        String conf = webServer.arg("config");
        if(conf.length())
          webServer.send(200, "text/plain", config.save(conf) ? "SAVE ERROR" : "OK");
        else webServer.send(200, "text/plain", "DATA ERROR");
      }
      else webServer.send(200, "text/plain", "ARG ERROR");
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Send sensors data
   */
  webServer.on("/data.json", HTTP_GET, [] () {
    String data = "{\n";
    
    // System
    data += web_jsonStr("state", web_isLogged() ? "OK" : "LOGIN", 1, true);
    data += web_jsonStr("fw", global.fw, 1, true);
    data += web_jsonStr("runtime", lang.runtime(millis() / 1000), 1, true);
    data += web_jsonInt("freeheap", ESP.getFreeHeap(), 1, true);
    data += web_jsonStr("time", web_timeString(now()), 1, true);
    
    // Weekday
    int wd = weekday();
    data += "\t\"wd\": [\""; data += String(wd); data += "\", \"";
    data += lang.weekdayShortName(wd); data += "\", \"";
    if(++wd > 7) wd = 1;
    data += lang.weekdayShortName(wd); data += "\", \"";
    if(++wd > 7) wd = 1;
    data += lang.weekdayShortName(wd); data += "\"],\n";
    
    // Units
    data += "\t\"units\": {\n";
    data += web_jsonStr("v", lang.v(), 2, true);
    data += web_jsonStr("mm", lang.mm(), 2, true);
    data += web_jsonStr("ms", lang.ms(), 2, false);
    data += "\t},\n";
    
    // Network
    data += "\t\"network\": {\n";
    data += web_jsonStr("ssid", global.apMode ? config.accessPoint_ssid() : WiFi.SSID(), 2, true);
    data += web_jsonInt("ch", WiFi.channel(), 2, true);
    data += web_jsonStr("sig", String(WiFi.RSSI()) + "dBm", 2, true);
    data += web_jsonStr("mac", WiFi.macAddress(), 2, true);
    data += web_jsonStr("ip", WiFi.localIP().toString(), 2, true);
    data += web_jsonStr("mask", WiFi.subnetMask().toString(), 2, true);
    data += web_jsonStr("gw", WiFi.gatewayIP().toString(), 2, true);
    data += web_jsonStr("dns1", WiFi.dnsIP().toString(), 2, true);
    data += web_jsonStr("dns2", WiFi.dnsIP().toString(), 2, false);
    data += "\t},\n";

    // Comfort
    data += web_jsonStr("comfort", lang.comfort(global.comfort), 1, true);
    
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
    data += web_jsonFloat("temp", round(weather.get_currentTemp() * 10) / 10, 2, true);
    data += web_jsonFloat("hum", round(weather.get_currentHum() * 10) / 10, 2, true);
    data += web_jsonFloat("pres", round(weather.get_currentPres() * 10) / 10, 2, true);
    data += web_jsonInt("icon", weather.get_currentIcon(), 2, true);
    data += web_jsonInt("isDay", weather.get_isDay(), 2, true);
    data += "\t\t\"wind\": {\n";
    data += web_jsonFloat("speed", round(weather.get_currentWindSpeed() * 10) / 10, 3, true);
    data += web_jsonFloat("dir", weather.get_currentWindDir(), 3, false);
    data += "\t\t},\n";
    data += web_jsonStr("descript", weather.get_description(), 2, true);
    data += web_jsonStr("upd", weather.get_currentUpdated() == 0 ? "" : web_timeString(weather.get_currentUpdated()), 2, true);
    data += "\t\t\"daily\": [\n";
    for(unsigned int i=0; i<3; i++) {
      data += "\t\t\t{\n";
      data += web_jsonFloat("tMax", weather.get_dailyDayTemp(i), 4, true);
      data += web_jsonFloat("tMin", weather.get_dailyNightTemp(i), 4, true);
      data += web_jsonFloat("wind", weather.get_dailyWindSpeed(i), 4, true);
      data += web_jsonInt("icon", weather.get_dailyIcon(i), 4, false);
      data += "\t\t\t},\n";
    }
    data.remove(data.length() - 2);
    data += "\n\t\t]\n\t},\n";
    
    // List of available networks  
    data += "\t\"ssids\": [\n";
    for(unsigned int i=0; i<global.nets; i++) {
      data += "\t\t[\""; data += global.ssids[i]; 
      data += "\", "; data += String(global.rssis[i]); 
      data += (i < global.nets - 1) ? "],\n" : "]\n";
    }
    data += "\t],\n";
    
    // Thingspeak
    data += "\t\"thing\": {\n";
    data += "\t\t\"fields\": [";
    for(unsigned int i=0; i<8; i++) {
      data += String(thingspeak.get_field(i));
      data += (i < 7) ? ", " : "],\n";
    }
    data += web_jsonInt("expired", (now() - thingspeak.get_updated() < config.thingspeakReceive_expire() * 60) ? -1 : 0, 2, false);
    data += "\t}\n}";
  
    webServer.send(200, "text/json", data);
    data = String();
  });

  /**
   * Send files list
   */
  webServer.on("/files.json", HTTP_GET, [] () {
    String data = "{\n";
    if(!web_isLogged()) data = "{}";
    else {
      data += "\t\"fs\": {\n";
      data += "\t\t\"list\": [\n";
      data += web_filelist;
      data.remove(data.length() - 2);
      data += "\n\t\t],\n";
  
      FSInfo fsInfo;
      LittleFS.info(fsInfo);
      data += web_jsonFloat("total", fsInfo.totalBytes / 1024, 2, true);
      data += web_jsonFloat("free", (fsInfo.totalBytes - fsInfo.usedBytes) / 1024, 2, false);
      data += "\t}\n}";
    }

    webServer.send(200, "text/json", data);
    data = String();
  });

  /**
   * Restart ESP8266
   */
   webServer.on("/esp/restart", HTTP_GET, [] () {
     if(web_isLogged()) {
      webServer.send(200, "text/plain", "OK");
      ESP.restart();
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Turn display on and off
   */
  webServer.on("/esp/dispToggle", HTTP_GET, [] () {
    if(web_isLogged()) {
    //global.display_but_pressed = true;
      webServer.send(200, "text/plain", "OK");
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Brightness adjust
   */
  webServer.on("/esp/bright", HTTP_GET, [] () {
    if(web_isLogged()) {
      if(webServer.hasArg("bright")) {
        config.set_bright((webServer.arg("bright")).toInt());
        webServer.send(200, "text/plain", "OK");
      }
      else webServer.send(200, "text/plain", "ERROR");
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Ambient light sensor sensitivity adjust
   */
  webServer.on("/esp/sensitivity", HTTP_GET, [] () {
    if(web_isLogged()) {
      if(webServer.hasArg("bright")) {
        config.set_sensitivity((webServer.arg("bright")).toInt());
        webServer.send(200, "text/plain", "OK");
      }
      else webServer.send(200, "text/plain", "ERROR");
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Initialize clock syncronization with NTP server
   */
  webServer.on("/esp/syncClock", HTTP_GET, [] () {
    if(web_isLogged()) {
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
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Send information about the clock synchronization process
   */
  webServer.on("/esp/syncdialog", HTTP_GET, [] () {
    if(web_isLogged()) {
      if(!global.clockSynchronized) webServer.send(200, "text/plain", ".");
      else webServer.send(200, "text/plain", web_timeString(now()));
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Change password
   */
  webServer.on("/esp/changePass", HTTP_POST, [] () {
    if(web_isLogged()) {
      String oldPass = (webServer.hasArg("oldPass")) ? webServer.arg("oldPass") : "";
      String newPass = (webServer.hasArg("newPass")) ? webServer.arg("newPass") : "";
      String res = "-";
      res = (oldPass == config.account_pass()) ? config.savePassword(newPass) ? "ERROR Write file" : "OK" : "ERROR";
      webServer.send(200, "text/plain", res);
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");
  });

  /**
   * Delete a file
   */
  webServer.on("/esp/delete", HTTP_POST, [] () {
    if(web_isLogged()) {
      String path = "/" + webServer.arg("file");
      if(!LittleFS.exists(path)) webServer.send(404, "text/plain", "FileNotFound");
      else {
        LittleFS.remove(path);
        webServer.send(200, "text/plain", "OK");
        path = String();
      }
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");

    web_filelist = String();
    web_listAllFilesInDir("/");
  });

  /**
   * Rename a file
   */
  webServer.on("/esp/rename", HTTP_POST, [] () {
    if(web_isLogged()) {
      String alt = "/" + webServer.arg("old");
      String neu = "/" + webServer.arg("new");
      if(!LittleFS.exists(alt)) webServer.send(404, "text/plain", "FileNotFound");
      else {
        LittleFS.rename(alt, neu);
        webServer.send(200, "text/plain", "OK");
        alt = String();
        neu = String();
      }
    }
    else webServer.send(200, "text/plain", "NOT LOGGED");

    web_filelist = String();
    web_listAllFilesInDir("/");
  });

  /**
   * Login
   * Username and password validation 
   */
  webServer.on("/esp/login", HTTP_GET, [] () {
    String login = webServer.hasArg("name") ? webServer.arg("name") : "";
    String pass = webServer.hasArg("pass") ? webServer.arg("pass") : "";
    bool loged = false;
    
    if(login == config.account_name() and pass == config.account_pass()) {
      loged = true;
      web_code = second() * 66000000 + millis() % 1000000;
    }
    
    webServer.sendHeader("Cache-Control", "no-cache");
    webServer.sendHeader("Set-Cookie", "auth=" + String(web_code) + "; Path=/; Max-Age=7200");
    webServer.send(200, "text/plain", loged ? "OK" : "error");
  });

  /**
   * Upload a file in FS
   */
  webServer.on("/esp/fileUpload", HTTP_POST, [] () {
    webServer.send(200, "text/plain", "");
  }, web_handleFileUpload);

  /**
   * Update firmware
   */
  webServer.on("/esp/fwUpdate", HTTP_POST, [] () {
    if(Update.hasError()) Serial.println("Firmware update failed!");
    else Serial.println("Firmware update successful!");
    delay(1000);
    ESP.reset();
  }, [] () {
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000)&0xFFFFF000;
      if(!Update.begin(maxSketchSpace)) Update.printError(Serial);
    }
    else if(upload.status == UPLOAD_FILE_WRITE) {
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) Update.printError(Serial);
    } 
    else if(upload.status == UPLOAD_FILE_END) {
      if(Update.end(true)) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      else Update.printError(Serial);
      Serial.setDebugOutput(false);
    }
    yield();
  });
  
  webServer.onNotFound([] () {
    if(!web_handleFileRead(webServer.uri())) webServer.send(404,"text/plain", "FileNotFound");
  });
  
  const char * headerkeys[] = {"User-Agent", "Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  webServer.collectHeaders(headerkeys, headerkeyssize);
  webServer.begin();
  web_listAllFilesInDir("/");
}

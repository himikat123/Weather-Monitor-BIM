String getContentType(String filename){
  if(webServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".json")) return "text/json";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  if(webServer.hasHeader("Cookie")){
    String cookie = webServer.header("Cookie");
    int8_t au = cookie.indexOf("auth");
    uint8_t cook[10]; 
    uint8_t coincid = 0;
    uint8_t code_auth[10];
    ESP.rtcUserMemoryRead(0, (uint32_t*) & code_auth, 10);
    for(uint8_t i=0; i<10; i++) cook[i] = (uint8_t)cookie[au + 5 + i] - 48;
    for(uint8_t i=0; i<10; i++) if(code_auth[i] == cook[i]) coincid++;
    if(au != -1 and coincid == 10 or
       path.endsWith("js") or 
       path.endsWith("json") or 
       path == "/log-err.htm" or 
       path == "/ok.htm" or 
       path == "/fail.htm") return FileRead(path); 
    else return FileRead("/login.htm"); 
  }
  else{
    if(path.endsWith("json")) return FileRead(path);
    else return FileRead("/login.htm");
  }
}

bool FileRead(String path){
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path+".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz)) path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload(){
  if(webServer.uri() != "/edit") return;
  HTTPUpload& upload = webServer.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/" + filename;
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } 
  else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) fsUploadFile.close();
  }
}

void web_settings(void){
  webServer.on("/esp/save.php", HTTP_POST, [](){
    if(webServer.arg("JS") != ""){
      File file = SPIFFS.open("/save/save.json", "w");
      if(file){
        file.print(webServer.arg("JS"));
        file.close();
        webServer.send(200, "text/plain", saved[config.lang].saved);
      }
      else webServer.send(200, "text/plain", saved[config.lang].not_saved);
    }
    if(webServer.arg("JSSIDS") != ""){  
      File file = SPIFFS.open("/save/jssids.json", "w");
      if(file){
        file.print(webServer.arg("JSSIDS"));
        file.close();
      }
    }
    if(webServer.arg("SSIDS") != ""){  
      File file = SPIFFS.open("/save/ssids.json", "w");
      if(file){
        file.print(webServer.arg("SSIDS"));
        file.close();
      }
    }  
  });
  
  webServer.on("/esp/ssid.php", HTTP_POST, [](){
    String json = "{";
    uint8_t n = WiFi.scanNetworks();
    for(uint8_t i=0; i<n; i++){
      json += "\""; 
      json += WiFi.SSID(i);
      json += "\":\"";
      json += abs(WiFi.RSSI(i));
      if(i == n - 1) json += "\"}";
      else json += "\",";
    }
    webServer.send(200, "text/json", json);
  });

  webServer.on("/esp/lang.php", HTTP_POST, [](){
    config.lang = (webServer.arg("LANG").toInt());
    webServer.send(200, "text/plain", "OK");
  });
  
  webServer.on("/esp/temp.php", HTTP_POST, [](){
    config.temp[0] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });
  
  webServer.on("/esp/tempi.php", HTTP_POST, [](){
    config.temp[1] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/tempe.php", HTTP_POST, [](){
    config.temp[2] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/pres.php", HTTP_POST, [](){
    config.pres[0] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/light.php", HTTP_POST, [](){
    config.light = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/presi.php", HTTP_POST, [](){
    config.pres[1] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/hum.php", HTTP_POST, [](){
    config.hum[0] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "OK");
  });

  webServer.on("/esp/humi.php", HTTP_POST, [](){
    config.hum[1] = round(webServer.arg("SENSOR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/tcor.php", HTTP_POST, [](){
    config.t_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/pcor.php", HTTP_POST, [](){
    config.p_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/hcor.php", HTTP_POST, [](){
    config.h_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/lcor.php", HTTP_POST, [](){
    config.l_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/ticor.php", HTTP_POST, [](){
    config.ti_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/picor.php", HTTP_POST, [](){
    config.pi_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/hicor.php", HTTP_POST, [](){
    config.hi_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/tecor.php", HTTP_POST, [](){
    config.te_cor = round(webServer.arg("COR").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/t_units.php", HTTP_POST, [](){
    config.t_unt = round(webServer.arg("UNT").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/p_units.php", HTTP_POST, [](){
    config.p_unt = round(webServer.arg("UNT").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/ti_units.php", HTTP_POST, [](){
    config.ti_unt = round(webServer.arg("UNT").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/pi_units.php", HTTP_POST, [](){
    config.pi_unt = round(webServer.arg("UNT").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/te_units.php", HTTP_POST, [](){
    config.te_unt = round(webServer.arg("UNT").toFloat());
    webServer.send(200, "text/plain", "ok");
  });

  webServer.on("/esp/adc.php", HTTP_POST, [](){
    String json = "{\"u\":\""; json += analogRead(A0); json += "\"}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/data.php", HTTP_POST, [](){
    float t = get_temp(0);
    if(t != 404.0){
      if(config.t_unt == 0) t += config.t_cor;
      else t = t * 1.8 + 32 + config.t_cor;
    }
    float ti = get_temp(1);
    if(ti != 404.0){
      if(config.ti_unt == 0) ti += config.ti_cor;
      else ti = ti * 1.8 + 32 + config.ti_cor;
    }
    float te = get_temp(2);
    if(te != 404.0){
      if(config.te_unt == 0) te += config.te_cor;
      else te = te * 1.8 + 32 + config.te_cor;
    }
    float p = get_pres(0);
    if(p != 4040.0){
      if(config.p_unt == 0) p += config.p_cor;
      else p = p * 0.75 + config.p_cor;
    }
    float pi = get_pres(1);
    if(pi != 4040.0){
      if(config.pi_unt == 0) pi += config.pi_cor;
      else pi = pi * 0.75 + config.pi_cor;
    }
    float h = get_hum(0);
    if(h != 404.0){
      h += config.h_cor;
    }
    float hi = get_hum(1);
    if(hi != 404.0){
       hi += config.hi_cor;
    }
    float l = get_light();
    if(l >= 0.0){
      l += config.l_cor;
    }
    String json = "{\"t\":\""; json += (t == 404.0) ? "--" : String(t); json += "\",";
    json += "\"h\":\""; json += (h == 404.0) ? "--" : String(h); json += "\",";
    json += "\"p\":\""; json += (p == 4040.0) ? "--" : String(p); json += "\",";
    json += "\"l\":\""; json += (l < 0.0) ? "--" : String(l); json += "\",";
    json += "\"ti\":\""; json += (ti == 404.0) ? "--" : String(ti); json += "\",";
    json += "\"te\":\""; json += (te == 404.0) ? "--" : String(te); json += "\",";
    json += "\"hi\":\""; json += (hi == 404.0) ? "--" : String(hi); json += "\",";
    json += "\"pi\":\""; json += (pi == 4040.0) ? "--" : String(pi); json += "\",";
    json += "\"st\":[1,"; 
    json += sensors.bme1Detected ? 1 : sensors.bmp1Detected ? 1 : 0; json += ",";
    json += sensors.bme2Detected ? 1 : sensors.bmp2Detected ? 1 : 0; json += ",";
    json += sensors.bmp180Detected ? 1 : 0; json += ",";
    json += sensors.dsDetected > 0 ? 1 : 0; json += ",";
    json += sensors.dsDetected > 1 ? 1 : 0; json += ",";
    json += sensors.dsDetected > 2 ? 1 : 0; json += ",";
    json += sensors.dhtDetected ? 1 : 0; json += ",";
    json += sensors.shtDetected ? 1 : 0; json += "],";
    json += "\"sp\":[1,"; 
    json += sensors.bme1Detected ? 1 : sensors.bmp1Detected ? 1 : 0; json += ",";
    json += sensors.bme2Detected ? 1 : sensors.bmp2Detected ? 1 : 0; json += ",";
    json += sensors.bmp180Detected ? 1 : 0; json += "],";
    json += "\"sh\":[1,"; 
    json += sensors.bme1Detected ? 1 : sensors.bmp1Detected ? 1 : 0; json += ",";
    json += sensors.bme2Detected ? 1 : sensors.bmp2Detected ? 1 : 0; json += ",";
    json += sensors.dhtDetected ? 1 : 0; json += ",";
    json += sensors.shtDetected ? 1 : 0; json += "],";
    json += "\"sl\":[1,"; 
    json += sensors.max44Detected ? 1 : 0; json += ",";
    json += sensors.bhDetected ? 1 : 0; json += "]}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/mac.php", HTTP_POST, [](){
    String json = "{\"mac\":\"";
    json += WiFi.macAddress();
    json += "\"}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/ip_gw.php", HTTP_POST, [](){
    String json = "{\"ip\":\""; json += WiFi.localIP().toString();    json += "\",";
    json += "\"gw\":\"";        json += WiFi.gatewayIP().toString();  json += "\",";
    json += "\"mask\":\"";      json += WiFi.subnetMask().toString(); json += "\"}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/status.php", HTTP_POST, [](){
    BatteryLevel();
    float t = get_temp(0);
    if(t != 404.0){
      if(config.t_unt == 0) t += config.t_cor;
      else t = t * 1.8 + 32 + config.t_cor;
    }
    float ti = get_temp(1);
    if(ti != 404.0){
      if(config.ti_unt == 0) ti += config.ti_cor;
      else ti = ti * 1.8 + 32 + config.ti_cor;
    }
    float te = get_temp(2);
    if(te != 404.0){
      if(config.te_unt == 0) te += config.te_cor;
      else te = te * 1.8 + 32 + config.te_cor;
    }
    float p = get_pres(0);
    if(p != 4040.0){
      if(config.p_unt == 0) p += config.p_cor;
      else p = p * 0.75 + config.p_cor;
    }
    float pi = get_pres(1);
    if(pi != 4040.0){
      if(config.pi_unt == 0) pi += config.pi_cor;
      else pi = pi * 0.75 + config.pi_cor;
    }
    float h = get_hum(0);
    if(h != 404.0){
      h += config.h_cor;
    }
    float hi = get_hum(1);
    if(hi != 404.0){
       hi += config.hi_cor;
    }
    float l = get_light();
    if(l >= 0.0){
      l += config.l_cor;
    }
    float cor = -config.k;
    cor = cor + 400;
    String json = "{\"fw\":\""; json += "v" + fw;                        json += "\",";
    json += "\"ssid\":\"";      json += WiFi.SSID();                     json += "\",";
    json += "\"ch\":\"";        json += WiFi.channel();                  json += "\",";
    json += "\"sig\":\"";       json += WiFi.RSSI();                     json += "dB\",";
    json += "\"mac\":\"";       json += WiFi.macAddress();               json += "\",";
    json += "\"ip\":\"";        json += WiFi.localIP().toString();       json += "\",";
    json += "\"temp\":\""; json += (t == 404.0) ? "--" : String(t); 
    if(t != 404.0) json += config.t_unt == 0 ? "°C" : "°F"; 
    json += "\",";
    json += "\"t\":\"";
    switch(config.temp[0]){
      case 0: json += "\","; break;
      case 1:{json += "BM"; json += sensors.bmp1Detected ? "P" : "E"; json += "280 (addr 0x76)\",";}break;
      case 2:{json += "BM"; json += sensors.bmp2Detected ? "P" : "E"; json += "280 (addr 0x77)\",";}break;
      case 3: json += "BMP180\","; break;
      case 4: json += "DS18B20 #1\","; break;
      case 5: json += "DS18B20 #2\","; break;
      case 6: json += "DS18B20 #3\","; break;
      case 7: json += "DHT22\","; break;
      case 8: json += "SHT21\","; break;
      default: json += "\","; break;
    }
    json += "\"pres\":\""; json += (p == 4040.0) ? "--" : String(p); 
    if(p != 4040.0) json += config.p_unt == 0 ? "hPa" : "mmHg"; 
    json += "\",";
    json += "\"p\":\"";
    switch(config.pres[0]){
      case 0: json += "\","; break;
      case 1:{json += "BM"; json += sensors.bmp1Detected ? "P" : "E"; json += "280 (addr 0x76)\",";}break;
      case 2:{json += "BM"; json += sensors.bmp2Detected ? "P" : "E"; json += "280 (addr 0x77)\",";}break;
      case 3: json += "BMP180\","; break;
      default: json += "\","; break;
    }
    json += "\"hum\":\""; json += (h == 404.0) ? "--" : String(h) + "%"; json += "\","; 
    json += "\"h\":\"";
    switch(config.hum[0]){
      case 0: json += "\","; break;
      case 1: json += "BME280 (addr 0x76)\","; break;
      case 2: json += "BME280 (addr 0x77)\","; break;
      case 3: json += "DHT22\","; break;
      case 4: json += "SHT21\","; break;
      default: json += "\","; break;
    }
    json += "\"ligh\":\""; json += (l < 0.0) ? "--" : String(l) + "Lux"; json += "\","; 
    json += "\"l\":\"";
    switch(config.light){
      case 0: json += "\","; break;
      case 1: json += "MAX44009\","; break;
      case 2: json += "BH1750FVI\","; break;
      default: json += "\","; break;
    }
    json += "\"tempi\":\""; json += (ti == 404.0) ? "--" : String(ti);
    if(ti != 404.0) json += config.ti_unt == 0 ? "°C" : "°F"; 
    json += "\",";
    json += "\"ti\":\"";
    switch(config.temp[1]){
      case 0: json += "\","; break;
      case 1:{json += "BM"; json += sensors.bmp1Detected ? "P" : "E"; json += "280 (addr 0x76)\",";}break;
      case 2:{json += "BM"; json += sensors.bmp2Detected ? "P" : "E"; json += "280 (addr 0x77)\",";}break;
      case 3: json += "BMP180\","; break;
      case 4: json += "DS18B20 #1\","; break;
      case 5: json += "DS18B20 #2\","; break;
      case 6: json += "DS18B20 #3\","; break;
      case 7: json += "DHT22\","; break;
      case 8: json += "SHT21\","; break;
      default: json += "\","; break;
    }
    json += "\"presi\":\""; json += (pi == 4040.0) ? "--" : String(pi);
    if(pi != 4040.0) json += config.pi_unt == 0 ? "hPa" : "mmHg"; 
    json += "\",";
    json += "\"pi\":\"";
    switch(config.pres[1]){
      case 0: json += "\","; break;
      case 1:{json += "BM"; json += sensors.bmp1Detected ? "P" : "E"; json += "280 (addr 0x76)\",";}break;
      case 2:{json += "BM"; json += sensors.bmp2Detected ? "P" : "E"; json += "280 (addr 0x77)\",";}break;
      case 3: json += "BMP180\","; break;
      default: json += "\","; break;
    }
    json += "\"humi\":\""; json += (hi == 404.0) ? "--" : String(hi) + "%"; json += "\","; 
    json += "\"hi\":\"";
    switch(config.hum[1]){
      case 0: json += "\","; break;
      case 1: json += "BME280 (addr 0x76)\","; break;
      case 2: json += "BME280 (addr 0x77)\","; break;
      case 3: json += "DHT22\","; break;
      case 4: json += "SHT21\","; break;
      default: json += "\","; break;
    }
    json += "\"tempe\":\""; json += (te == 404.0) ? "--" : String(te); 
    if(te != 404.0) json += config.te_unt == 0 ? "°C" : "°F"; 
    json += "\",";
    json += "\"te\":\"";
    switch(config.temp[2]){
      case 0: json += "\","; break;
      case 1:{json += "BM"; json += sensors.bmp1Detected ? "P" : "E"; json += "280 (addr 0x76)\",";}break;
      case 2:{json += "BM"; json += sensors.bmp2Detected ? "P" : "E"; json += "280 (addr 0x77)\",";}break;
      case 3: json += "BMP180\","; break;
      case 4: json += "DS18B20 #1\","; break;
      case 5: json += "DS18B20 #2\","; break;
      case 6: json += "DS18B20 #3\","; break;
      case 7: json += "DHT22\","; break;
      case 8: json += "SHT21\","; break;
      default: json += "\","; break;
    }
    json += "\"bat\":\""; json += sensors.uBat; json += "V\"}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/reboot.php", HTTP_POST, [](){
    webServer.send(200, "text/plain", "OK");
    ESP.deepSleep(10);
    ESP.reset();
  });

  webServer.on("/esp/fs.php", HTTP_POST, [](){
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    String json = "{\"total\":"; json += round(fs_info.totalBytes / 1024);
    json += ",\"used\":"; json += round(fs_info.usedBytes / 1024); json += "}";
    webServer.send(200, "text/plain", json);
  });

  webServer.on("/esp/list.php", HTTP_POST, [](){
    if(!webServer.hasArg("p")){
      webServer.send(500, "text/plain", "BAD ARGS"); 
      return;
    }
    String path = webServer.arg("p");
    if(path == "") path = "/";
    else path = "/" + path;
    Dir dir = SPIFFS.openDir(path);
    path = String();
    String output = "[";
    while(dir.next()){
      if(dir.fileName() != "/save/user.us"){
        if(output != "[") output += ',';
        output += "{\"name\":\""; output += dir.fileName();
        output += "\",\"type\":\"file\",\"size\":\""; output += dir.fileSize();
        output += "\"}"; 
      }
    }
    output += "]";
    webServer.send(200, "text/json", output);
  });
  
  webServer.on("/esp/del.php", HTTP_POST, [](){
    String path = "/" + webServer.arg("d");
    if(!SPIFFS.exists(path)) return webServer.send(404, "text/plain", "FileNotFound");
    SPIFFS.remove(path);
    webServer.send(200, "text/plain", "OK");
    path = String();
  });

  webServer.on("/esp/rename.php", HTTP_POST, [](){
    String old = "/" + webServer.arg("old");
    String neu = "/" + webServer.arg("new");
    if(!SPIFFS.exists(old)) return webServer.send(404, "text/plain", "FileNotFound");
    SPIFFS.rename(old, neu);
    webServer.send(200, "text/plain", "OK");
    old = String();
    neu = String();
  });

  webServer.on("/log.php", HTTP_POST, [](){
    String login = webServer.arg("LOGIN");
    String passw = webServer.arg("PASSW");
    String fileData, username, pass;
    File file = SPIFFS.open("/save/user.us", "r");
    if(file){
      fileData = file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(fileData);
      if(root.success()){
        String u = root["user"];  
        String p = root["pass"];
        username = u;
        pass = p;
      }
    }
    if(login == username and passw == pass){
      String code = "";
      uint8_t code_auth[10];
      for(uint8_t i=0; i<10; i++){
        code_auth[i] = ESP8266TrueRandom.random(0, 10);
        code += String(code_auth[i]); 
      }
      webServer.sendHeader("Location", "/");
      webServer.sendHeader("Cache-Control", "no-cache");
      webServer.sendHeader("Set-Cookie", "auth="+code+"; Max-Age=3600");
      webServer.send(301);
      ESP.rtcUserMemoryWrite(0, (uint32_t*)&code_auth, 10);
    }
    else{
      webServer.sendHeader("Location", "/log-err.htm");
      webServer.sendHeader("Cache-Control", "no-cache");
      webServer.send(301);
    }
  });

  webServer.on("/esp/user.php", HTTP_POST, [](){
    String fileData, username, pass;
    File file = SPIFFS.open("/save/user.us", "r");
    if(file){
      fileData = file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(fileData);
      if(root.success()){
        String u = root["user"];  
        String p = root["pass"];
        username = u;
        pass = p;
      }
    }
    if(webServer.arg("USER") != ""){
      String user = webServer.arg("USER");
      String old_pass = webServer.arg("OLDPAS");
      String new_pass = webServer.arg("NEWPAS");
      if(String(pass) == old_pass){
        if(user == username and old_pass == new_pass)
          webServer.send(200, "text/plain", saved[config.lang].saved);
        else{
          File filew = SPIFFS.open("/save/user.us", "w");
          if(filew){
            filew.print("{\"user\":\""+user+"\",\"pass\":\""+new_pass+"\"}");
            filew.close();
            webServer.send(200, "text/plain", saved[config.lang].saved);
          } 
        } 
      }
      else webServer.send(200, "text/plain", saved[config.lang].old_pass);
    }
    else webServer.send(200, "text/plain", saved[config.lang].not_saved);
  });

  webServer.on("/esp/name.php", HTTP_POST, [](){
    String fileData, user;
    File file = SPIFFS.open("/save/user.us", "r");
    if(file){
      fileData = file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(fileData);
      if(root.success()){
        String uname = root["user"];
        user = uname;
      }
    }
    webServer.send(200, "text/plain", user);
  });
  
  webServer.on("/edit", HTTP_POST, [](){
    webServer.send(200, "text/plain", "");
  }, handleFileUpload);
  
  webServer.on("/esp/update.php", HTTP_POST, [](){
    if(Update.hasError()) handleFileRead("/fail.htm");
    else handleFileRead("/ok.htm");
    delay(1000);
    ESP.deepSleep(10);
  }, [](){
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START){
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){
        Update.printError(Serial);
      }
    }
    else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    } 
    else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } 
      else{
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });

  webServer.onNotFound([](){
    if(!handleFileRead(webServer.uri())) webServer.send(404, "text/plain", "FileNotFound");
  });
  
  const char * headerkeys[] = {"User-Agent", "Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  webServer.collectHeaders(headerkeys, headerkeyssize);
  webServer.begin();
}

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
    String cookie=webServer.header("Cookie");
    int8_t au=cookie.indexOf("auth");
    uint8_t cook[10]; 
    uint8_t coincid=0;
    uint8_t code_auth[10];
    ESP.rtcUserMemoryRead(0,(uint32_t*)&code_auth,10);
    for(uint8_t i=0;i<10;i++) cook[i]=(uint8_t)cookie[au+5+i]-48;
    for(uint8_t i=0;i<10;i++) if(code_auth[i]==cook[i]) coincid++;
    if(au!=-1 and coincid==10 or 
       path.endsWith("json") or 
       path=="/log-err.htm") return FileRead(path); 
    else return FileRead("/login.htm"); 
  }
  else{
    if(path.endsWith("json")) return FileRead(path);
    else return FileRead("/login.htm");
  }
}

bool FileRead(String path){
  ap_flag=true;
  if(path.endsWith("/")) path+="index.htm";
  String contentType=getContentType(path);
  String pathWithGz=path+".gz";
  if(SPIFFS.exists(pathWithGz)||SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz)) path+=".gz";
    File file=SPIFFS.open(path,"r");
    size_t sent=webServer.streamFile(file,contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload(){
  if(webServer.uri()!="/edit") return;
  HTTPUpload& upload=webServer.upload();
  if(upload.status==UPLOAD_FILE_START){
    String filename=upload.filename;
    if(!filename.startsWith("/")) filename="/"+filename;
    fsUploadFile=SPIFFS.open(filename,"w");
    filename=String();
  } 
  else if(upload.status==UPLOAD_FILE_WRITE){
    if(fsUploadFile) fsUploadFile.write(upload.buf,upload.currentSize);
  } 
  else if(upload.status==UPLOAD_FILE_END){
    if(fsUploadFile) fsUploadFile.close();
  }
}

void web_settings(void){
  SPIFFS.begin();
  
  webServer.on("/esp/save.php",HTTP_POST,[](){
    if(webServer.arg("JS")!=""){
      File file=SPIFFS.open("/save/save.json","w");
      if(file){
        file.print(webServer.arg("JS"));
        file.close();
        webServer.send(200,"text/plain",saved[html.lang].saved);
      }
      else webServer.send(200,"text/plain",saved[html.lang].not_saved);
    }
    if(webServer.arg("JSSIDS")!=""){  
      File file=SPIFFS.open("/save/jssids.json","w");
      if(file){
        file.print(webServer.arg("JSSIDS"));
        file.close();
      }
    }
    if(webServer.arg("SSIDS")!=""){  
      File file=SPIFFS.open("/save/ssids.json","w");
      if(file){
        file.print(webServer.arg("SSIDS"));
        file.close();
      }
    }  
  });

  webServer.on("/esp/ssid.php",HTTP_POST,[](){
    String json="{";
    uint8_t n=WiFi.scanNetworks();
    for(uint8_t i=0;i<n;i++){
      json+="\""; 
      json+=WiFi.SSID(i);
      json+="\":\"";
      json+=abs(WiFi.RSSI(i));
      if(i==n-1) json+="\"}";
      else json+="\",";
    }
    webServer.send(200,"text/json",json);
  });

  webServer.on("/esp/ssd.php",HTTP_POST,[](){
    String json="{\"ssid\":\""; json+=WiFi.SSID(); json+="\",";
    json+="\"ip\":\""; json+=WiFi.localIP().toString(); json+="\"}";
    webServer.send(200,"text/json",json);
  });

  webServer.on("/esp/lang.php",HTTP_POST,[](){
    html.lang=(webServer.arg("LANG").toInt());
    webServer.send(200,"text/plain","OK");
  });

  webServer.on("/esp/temp.php",HTTP_POST,[](){
    html.temp=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    webServer.send(200,"text/plain","");
  });

  webServer.on("/esp/hum.php",HTTP_POST,[](){
    html.hum=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    webServer.send(200,"text/plain","");
  });

  webServer.on("/esp/tcor.php",HTTP_POST,[](){
    html.t_cor=webServer.arg("COR").toFloat();
    webServer.send(200,"text/plain","OK");
  });

  webServer.on("/esp/hcor.php",HTTP_POST,[](){
    html.h_cor=webServer.arg("COR").toFloat();
    webServer.send(200,"text/plain","OK");
  });
  
  webServer.on("/esp/br.php",HTTP_POST,[](){
    int bright=webServer.arg("BR").toInt();
    analogWrite(BACKLIGHT,bright);
    webServer.send(200,"text/plain",String(bright));
  });

  webServer.on("/esp/br_n.php",HTTP_POST,[](){
    int bright=webServer.arg("BR_N").toInt();
    analogWrite(BACKLIGHT,bright);
    webServer.send(200,"text/plain",String(bright));
  });

  
webServer.on("/esp/adc.php",HTTP_POST,[](){
    String json="{\"u\":\""; json+=analogRead(A0); json+="\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/data.php",HTTP_POST,[](){
    float t=get_temp(1);
    float h=get_humidity();
    String json="{\"t\":"; json+=(t==404)?"--":String(t); json+=",";
    json+="\"h\":"; json+=(h==404)?"--":String(h); json+="}";
    webServer.send(200,"text/plain",json);
    if(html.temp==3) sensors.requestTemperatures();
  });

  webServer.on("/esp/mac_ip.php",HTTP_POST,[](){
    String json="{\"mac\":\""; json+=WiFi.macAddress();           json+="\",";
    json+="\"ip\":\"";         json+=WiFi.softAPIP().toString();  json+="\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/ip_gw.php",HTTP_POST,[](){
    String json="{\"ip\":\""; json+=WiFi.localIP().toString();    json+="\",";
    json+="\"gw\":\"";        json+=WiFi.gatewayIP().toString();  json+="\",";
    json+="\"mask\":\"";      json+=WiFi.subnetMask().toString(); json+="\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/status.php",HTTP_POST,[](){
    float t=get_temp(1);
    float h=get_humidity();
    float cor=-html.k;
    cor=cor+400;
    String json="{\"fw\":\""; json+="v"+fw;                    json+="\",";
    json+="\"id\":\"";        json+=html.id;                   json+="\",";
    json+="\"ssid\":\"";      json+=WiFi.SSID();               json+="\",";
    json+="\"ch\":\"";        json+=WiFi.channel();            json+="\",";
    json+="\"sig\":\"";       json+=WiFi.RSSI();               json+="dB\",";
    json+="\"mac\":\"";       json+=WiFi.macAddress();         json+="\",";
    json+="\"ip\":\"";        json+=WiFi.localIP().toString(); json+="\",";
    json+="\"temp\":\"";      json+=(t==404)?"--":String(t);   json+="\",";
    json+="\"t\":\"";
    switch(html.temp){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="DS18B20\","; break;
      case 3: json+="DHT22\","; break;
      case 4: json+="SHT21\","; break;
      default: json+="\","; break;
    }
    json+="\"hum\":\""; json+=(h==404)?"--":String(h); json+="\","; 
    json+="\"h\":\"";
    switch(html.hum){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="DHT22\","; break;
      case 3: json+="SHT21\","; break;
      default: json+="\","; break;
    }
    json+="\"bat\":\""; json+=analogRead(A0)/cor; json+="V\"}";
    webServer.send(200,"text/plain",json);
    if(html.temp==3) sensors.requestTemperatures();
  });

  webServer.on("/esp/reboot.php",HTTP_POST,[](){
    webServer.send(200,"text/plain","OK");
    ESP.deepSleep(10);
    ESP.reset();
  });

  webServer.on("/esp/fs.php",HTTP_POST,[](){
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    String json="{\"total\":"; json+=round(fs_info.totalBytes/1024);
    json+=",\"used\":"; json+=round(fs_info.usedBytes/1024); json+="}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/list.php",HTTP_POST,[](){
    if(!webServer.hasArg("p")){
      webServer.send(500,"text/plain","BAD ARGS"); 
      return;
    }
    String path=webServer.arg("p");
    if(path=="") path="/";
    else path="/"+path;
    Dir dir=SPIFFS.openDir(path);
    path=String();
    String output="[";
    while(dir.next()){
      if(dir.fileName()!="/save/user.us"){
        if(output!="[") output+=',';
        output+="{\"name\":\""; output+=dir.fileName();
        output+="\",\"type\":\"file\",\"size\":\""; output+=dir.fileSize();
        output+="\"}"; 
      }
    }
    output+="]";
    webServer.send(200,"text/json",output);
  });
  
  webServer.on("/esp/del.php",HTTP_POST,[](){
    String path="/"+webServer.arg("d");
    if(!SPIFFS.exists(path)) return webServer.send(404,"text/plain","FileNotFound");
    SPIFFS.remove(path);
    webServer.send(200,"text/plain","OK");
    path=String();
  });

  webServer.on("/esp/rename.php",HTTP_POST,[](){
    String old="/"+webServer.arg("old");
    String neu="/"+webServer.arg("new");
    if(!SPIFFS.exists(old)) return webServer.send(404,"text/plain","FileNotFound");
    SPIFFS.rename(old,neu);
    webServer.send(200,"text/plain","OK");
    old=String();
    neu=String();
  });

  webServer.on("/log.php",HTTP_POST,[](){
    String login=webServer.arg("LOGIN");
    String passw=webServer.arg("PASSW");
    String fileData,username,pass;
    File file=SPIFFS.open("/save/user.us","r");
    if(file){
      fileData=file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root=jsonBuffer.parseObject(fileData);
      if(root.success()){
        String u=root["user"];  
        String p=root["pass"];
        username=u;
        pass=p;
      }
    }
    if(login==username and passw==pass){
      String code="";
      uint8_t code_auth[10];
      for(uint8_t i=0;i<10;i++){
        code_auth[i]=ESP8266TrueRandom.random(0,10);
        code+=String(code_auth[i]); 
      }
      webServer.sendHeader("Location","/");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.sendHeader("Set-Cookie","auth="+code+"; Max-Age=3600");
      webServer.send(301);
      ESP.rtcUserMemoryWrite(0,(uint32_t*)&code_auth,10);
    }
    else{
      webServer.sendHeader("Location","/log-err.htm");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.send(301);
    }
  });

  webServer.on("/esp/user.php",HTTP_POST,[](){
    String fileData,username,pass;
    File file=SPIFFS.open("/save/user.us","r");
    if(file){
      fileData=file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root=jsonBuffer.parseObject(fileData);
      if(root.success()){
        String u=root["user"];  
        String p=root["pass"];
        username=u;
        pass=p;
      }
    }
    if(webServer.arg("USER")!=""){
      String user=webServer.arg("USER");
      String old_pass=webServer.arg("OLDPAS");
      String new_pass=webServer.arg("NEWPAS");
      if(String(pass)==old_pass){
        if(user==username and old_pass==new_pass)
          webServer.send(200,"text/plain",saved[html.lang].saved);
        else{
          File filew=SPIFFS.open("/save/user.us","w");
          if(filew){
            filew.print("{\"user\":\""+user+"\",\"pass\":\""+new_pass+"\"}");
            filew.close();
            webServer.send(200,"text/plain",saved[html.lang].saved);
          } 
        } 
      }
      else webServer.send(200,"text/plain",saved[html.lang].old_pass);
    }
    else webServer.send(200,"text/plain",saved[html.lang].not_saved);
  });

  webServer.on("/esp/name.php",HTTP_POST,[](){
    String fileData, user;
    File file=SPIFFS.open("/save/user.us","r");
    if(file){
      fileData=file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root=jsonBuffer.parseObject(fileData);
      if(root.success()){
        String uname=root["user"];
        user=uname;
      }
    }
    webServer.send(200,"text/plain",user);
  });
  
  webServer.on("/edit",HTTP_POST,[](){
    webServer.send(200,"text/plain","");
  },handleFileUpload);
  
  webServer.onNotFound([](){
    if(!handleFileRead(webServer.uri())) webServer.send(404,"text/plain","FileNotFound");
  });
  
  const char * headerkeys[]={"User-Agent","Cookie"};
  size_t headerkeyssize=sizeof(headerkeys)/sizeof(char*);
  webServer.collectHeaders(headerkeys,headerkeyssize);
  webServer.begin();
}


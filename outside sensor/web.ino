String getContentType(String filename){
  if(webServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
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

void handleFileDelete(){
  if(webServer.args()==0) return webServer.send(500,"text/plain","BAD ARGS");
  String path=webServer.arg(0);
  if(path=="/") return webServer.send(500,"text/plain","BAD PATH");
  if(!SPIFFS.exists(path)) return webServer.send(404,"text/plain","FileNotFound");
  SPIFFS.remove(path);
  webServer.send(200,"text/plain","");
  path=String();
}

void handleFileCreate(){
  if(webServer.args()==0) return webServer.send(500,"text/plain","BAD ARGS");
  String path=webServer.arg(0);
  if(path=="/") return webServer.send(500,"text/plain","BAD PATH");
  if(SPIFFS.exists(path)) return webServer.send(500,"text/plain","FILE EXISTS");
  File file=SPIFFS.open(path,"w");
  if(file) file.close();
  else return webServer.send(500,"text/plain","CREATE FAILED");
  webServer.send(200,"text/plain","");
  path=String();
}

void handleFileList(){
  if(!webServer.hasArg("dir")){
    webServer.send(500,"text/plain","BAD ARGS"); 
    return;
  }
  
  String path=webServer.arg("dir");
  Dir dir=SPIFFS.openDir(path);
  path=String();

  String output="[";
  while(dir.next()){
    File entry=dir.openFile("r");
    if(output!="[") output+=',';
    bool isDir=false;
    output+="{\"type\":\"";
    output+=(isDir)?"dir":"file";
    output+="\",\"name\":\"";
    output+=String(entry.name()).substring(1);
    output+="\"}";
    entry.close();
  }
  
  output+="]";
  webServer.send(200,"text/json",output);
}

void web_settings(void){
  SPIFFS.begin();
  webServer.on("/esp/save.php",HTTP_POST,[](){
    if(webServer.arg("JS"!="")){
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

  webServer.on("/esp/local.php",HTTP_GET,[](){
    String json="{\"C\":"; json+=get_temp(1);
    json+=",\"HPA\":"; json+=get_pres();
    json+=",\"HUM\":"; json+=get_humidity(); json+="}";
    webServer.send(200,"text/json",json);
  });

  webServer.on("/esp/lang.php",HTTP_POST,[](){
    html.lang=(webServer.arg("LANG").toInt());
    webServer.send(200,"text/plain","OK");
  });
  
  webServer.on("/esp/temp.php",HTTP_POST,[](){
    html.temp=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    float t=get_temp(1);
    String json="{\"C\":\""; json+=(t==404)?"--":String(t); json+="&deg;C\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/pres.php",HTTP_POST,[](){
    html.pres=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    int p=get_pres();
    String json="{\"P\":\""; json+=(p==4040)?"--":String(p); json+=web_lng[html.lang].hPa; json+="\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/hum.php",HTTP_POST,[](){
    html.hum=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    int h=get_humidity();
    String json="{\"H\":\""; json+=(h==404)?"--":String(h); json+="%\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/adc.php",HTTP_POST,[](){
    String json="{\"u\":\""; json+=analogRead(A0); json+="\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/data.php",HTTP_POST,[](){
    float t=get_temp(1);
    int p=get_pres();
    int h=get_humidity();
    String json="{\"t\":\""; json+=(t==404)?"--":String(t); json+="&deg;C\",";
    json+="\"h\":\""; json+=(h==404)?"--":String(h); json+="%\",";
    json+="\"p\":\""; json+=(p==4040)?"--":String(p); json+=web_lng[html.lang].hPa; json+="\"}";
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
    int p=get_pres();
    int h=get_humidity();
    float cor=-html.k;
    cor=cor+400;
    String json="{\"fw\":\""; json+="v"+fw;                    json+="\",";
    json+="\"ssid\":\"";      json+=WiFi.SSID();               json+="\",";
    json+="\"ch\":\"";        json+=WiFi.channel();            json+="\",";
    json+="\"sig\":\"";       json+=WiFi.RSSI();               json+="dB\",";
    json+="\"mac\":\"";       json+=WiFi.macAddress();         json+="\",";
    json+="\"ip\":\"";        json+=WiFi.localIP().toString(); json+="\",";
    json+="\"temp\":\"";      json+=(t==404)?"--":String(t);   json+="&deg;C\",";
    json+="\"t\":\"";
    switch(html.temp){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="BMP180\","; break;
      case 3: json+="DS18B20\","; break;
      case 4: json+="DHT22\","; break;
      default: json+="\","; break;
    }
    json+="\"pres\":\""; json+=(p==4040)?"--":String(p); json+=web_lng[html.lang].hPa; json+="\",";
    json+="\"p\":\"";
    switch(html.pres){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="BMP180\","; break;
      default: json+="\","; break;
    }
    json+="\"hum\":\""; json+=(h==404)?"--":String(h); json+="%\","; 
    json+="\"h\":\"";
    switch(html.hum){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="DHT22\","; break;
      default: json+="\","; break;
    }
    json+="\"bat\":\""; json+=analogRead(A0)/cor; json+="V\"}";
    webServer.send(200,"text/plain",json);
    if(html.temp==3) sensors.requestTemperatures();
  });

  webServer.on("/esp/reboot.php",HTTP_POST,[](){
    webServer.send(200,"text/plain","OK");
    ESP.restart();
  });

  webServer.on("/list",HTTP_GET,handleFileList);
  webServer.on("/edit",HTTP_GET,[](){
    if(!handleFileRead("/edit.htm")) webServer.send(404,"text/plain","FileNotFound");
  });
  webServer.on("/edit",HTTP_PUT,handleFileCreate);
  webServer.on("/edit",HTTP_DELETE,handleFileDelete);
  webServer.on("/edit",HTTP_POST,[](){
    webServer.send(200,"text/plain","");
  },handleFileUpload);
  webServer.on("/esp/update.php",HTTP_POST,[](){
    if(Update.hasError()) handleFileRead("/fail.htm");
    else handleFileRead("/ok.htm");
    ESP.restart();
  },[](){
    HTTPUpload& upload=webServer.upload();
    if(upload.status==UPLOAD_FILE_START){
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n",upload.filename.c_str());
      uint32_t maxSketchSpace=(ESP.getFreeSketchSpace()-0x1000)&0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){
        Update.printError(Serial);
      }
    }
    else if(upload.status==UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf,upload.currentSize)!=upload.currentSize){
        Update.printError(Serial);
      }
    } 
    else if(upload.status==UPLOAD_FILE_END){
      if(Update.end(true)){
        Serial.printf("Update Success: %u\nRebooting...\n",upload.totalSize);
      } 
      else{
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  webServer.onNotFound([](){
    if(!handleFileRead(webServer.uri())) webServer.send(404,"text/plain","FileNotFound");
  });
  webServer.begin();
}


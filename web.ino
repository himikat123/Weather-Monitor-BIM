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
  webServer.on("/esp/settings.php",HTTP_POST,[](){
    File file=SPIFFS.open("/save/save.json","w");
    if(file){
      file.print(webServer.arg("JS"));
      file.close();
      webServer.send(200,"text/plain",saved[html.lang].saved);
    }
    else webServer.send(200,"text/plain",saved[html.lang].not_saved);
    file=SPIFFS.open("/save/jssids.json","w");
    if(file){
      file.print(webServer.arg("JSSIDS"));
      file.close();
    }
    file=SPIFFS.open("/save/ssids.json","w");
    if(file){
      file.print(webServer.arg("SSIDS"));
      file.close();
    }
  });

  webServer.on("/esp/temp.php",HTTP_POST,[](){
    html.temp=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    String json="{\"C\":\"";
    json+=get_temp(1);
    json+="&degC\",\"F\":\"";
    json+=get_temp(0);
    json+="&degF\"}";
    webServer.send(200,"text/plain",json);
  });

  webServer.on("/esp/hum.php",HTTP_POST,[](){
    html.hum=round(webServer.arg("SENSOR").toFloat());
    sensors_init();
    String json="{\"HUM\":\"";
    json+=get_humidity();
    json+="%\"}";
    webServer.send(200,"text/plain",json);
  });
  
  webServer.on("/esp/br.php",HTTP_POST,[](){
    int bright=webServer.arg("BR").toInt();
    analogWrite(BACKLIGHT,bright*10);
    webServer.send(200,"text/plain",String(bright));
  });

  webServer.on("/esp/br_n.php",HTTP_POST,[](){
    int bright=webServer.arg("BR_N").toInt();
    analogWrite(BACKLIGHT,bright*10);
    webServer.send(200,"text/plain",String(bright));
  });

  webServer.on("/esp/ssid.php",HTTP_POST,[](){
    String json="{";
    uint8_t n=WiFi.scanNetworks();
    for(uint8_t i=0;i<n;i++){
      json+="\""; 
      json+=WiFi.SSID(i);
      json+="\":\"";
      json+=abs(WiFi.RSSI(i));
      json+="\",";
    }
    json+="\"ver\":\" ";
    json+=vers;
    json+="\",\"myID\":";
    json+=html.id;
    json+=",\"C\":\"";
    json+=get_temp(1);
    json+="&degC\",\"F\":\"";
    json+=get_temp(0);
    json+="&degF\",\"HUM\":\"";
    json+=get_humidity();
    json+="%\",\"adc\":\"";
    json+=analogRead(A0);
    json+="\"}";
    webServer.send(200,"text/json",json);
  });

  webServer.on("/esp/lang.php",HTTP_POST,[](){
    html.lang=(webServer.arg("LANG").toInt());
    webServer.send(200,"text/plain","OK");
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
  webServer.onNotFound([](){
    if(!handleFileRead(webServer.uri())) webServer.send(404,"text/plain","FileNotFound");
  });
  webServer.begin();
  yield();
}


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

void sendContent(void){
  String mac_addr;
  byte mac[6];
  WiFi.macAddress(mac);
  mac_addr=String(mac[5],HEX)+":";
  mac_addr+=String(mac[4],HEX)+":";
  mac_addr+=String(mac[3],HEX)+":";
  mac_addr+=String(mac[2],HEX)+":";
  mac_addr+=String(mac[1],HEX)+":";
  mac_addr+=String(mac[0],HEX);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.createObject();
  root["ver"]      =vers;                             //text
  root["settings"] =web_lng[html.lang].settings;      //text
  root["ssid"]     =web_lng[html.lang].YourSSID;      //text
  root["ssidval"]  =html.ssid;                        //value ssid
  root["pass"]     =web_lng[html.lang].YourPASS;      //text
  root["passval"]  =html.pass;                        //value password
  root["lang"]     =web_lng[html.lang].Lang;          //text
  root["data"]     =web_lng[html.lang].data;          //text
  root["temp"]     =web_lng[html.lang].temp;          //text
  root["c"]        =mySensor.readTempC();             //temperature celsius
  root["f"]        =mySensor.readTempC()*1.8+32;      //temperature fahrenheit
  root["pres"]     =web_lng[html.lang].pres;          //text
  root["mm"]       =web_lng[html.lang].mm;            //text
  root["hpa"]      =web_lng[html.lang].hpa;           //text
  root["mm_val"]   =mySensor.readFloatPressure()/100*0.75; //pressure mmHg
  root["hpa_val"]  =mySensor.readFloatPressure()/100; //pressure hPa
  root["hum"]      =web_lng[html.lang].hum;           //text
  root["hum_val"]  =mySensor.readFloatHumidity();     //humidity
  root["submit"]   =web_lng[html.lang].SaveAll;       //text
  root["shPass"]   =web_lng[html.lang].show_password; //text
  root["Assid"]    =web_lng[html.lang].ap_ssid;       //text
  root["Apass"]    =web_lng[html.lang].ap_pass;       //text
  root["mac"]      =mac_addr;                         //mac address
  root["site"]     =site;                             //site address 
  root["copy"]     =copy;                             //email address
  root["type"]     =web_lng[html.lang].type;          //text
  root["dyn"]      =web_lng[html.lang].dyn;           //text
  root["stat"]     =web_lng[html.lang].stat;          //text
  root["ip"]       =web_lng[html.lang].ip;            //text
  root["mask"]     =web_lng[html.lang].mask;          //text
  root["gateway"]  =web_lng[html.lang].gateway;       //text
  root["ip_err"]   =web_lng[html.lang].ip_err;        //text
  root["mask_err"] =web_lng[html.lang].mask_err;      //text
  root["gw_err"]   =web_lng[html.lang].gw_err;        //text
  root["APssidval"]=rtcData.AP_SSID;                  //value ap ssid
  root["APpassval"]=rtcData.AP_PASS;                  //value ap pass 
  root["lng"]      =html.lang;                        //selected language
  root["ipval"]    =html.ip;                          //value ip
  root["maskval"]  =html.mask;                        //value mask
  root["gwval"]    =html.gateway;                     //value gateway
  root["typ"]      =html.typ;                         //selected connection type
  
  char buffer[1600];
  root.printTo(buffer,sizeof(buffer));
  webServer.send(200,"text/json",buffer);
}

void web_settings(void)
{
  SPIFFS.begin();
  MDNS.begin(host);
  webServer.on("/settings",HTTP_POST,[](){
    html.ssid=webServer.arg("SSID");
    html.pass=webServer.arg("PASS");
    html.lang=webServer.arg("LANG").toInt();
    html.ip=webServer.arg("IP");
    html.mask=webServer.arg("MASK");
    html.gateway=webServer.arg("GATEWAY");
    html.typ=webServer.arg("TYPE").toInt();
    webServer.arg("AP_SSID").toCharArray(rtcData.AP_SSID,(webServer.arg("AP_SSID").length())+1);
    webServer.arg("AP_PASS").toCharArray(rtcData.AP_PASS,(webServer.arg("AP_PASS").length())+1);
    save_eeprom();
    webServer.send(200,"text/plain","Saved");
  });
  
  webServer.on("/content",HTTP_POST,[](){
    sendContent();
  });
  
  webServer.on("/lang",HTTP_POST,[](){
    html.lang=webServer.arg("CHLNG").toInt();
    sendContent();
  });  
  
  webServer.on("/ssid",HTTP_POST,[](){
    String json="{";
    uint8_t n=WiFi.scanNetworks();
    for(uint8_t i=0;i<n;i++){
      json+="\""; 
      json+=WiFi.SSID(i);
      json+="\":\"";
      json+=abs(WiFi.RSSI(i));
      json+="\"";
      if((i+1)!=n) json+=",";
    }
    json+="}";
    webServer.send(200,"text/json",json);
  });

  webServer.on("/data",HTTP_POST,[](){
    float p=mySensor.readFloatPressure()/100;
    String j="{\"c\":";
    j+=mySensor.readTempC();
    j+=",\"f\":";
    j+=mySensor.readTempF();
    j+=",\"mm_val\":";
    j+=p*0.75;
    j+=",\"hpa_val\":";
    j+=p;
    j+=",\"hum_val\":";
    j+=mySensor.readFloatHumidity();
    j+="}";
    webServer.send(200,"text/json",j);
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
}


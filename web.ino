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
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.createObject();
  root["ver"]      =vers;                             //text
  root["settings"] =web_lng[html.lang].Settings;      //text
  root["ssid"]     =web_lng[html.lang].YourSSID;      //text
  root["ssidval"]  =html.ssid;                        //value ssid
  root["pass"]     =web_lng[html.lang].YourPASS;      //text
  root["passval"]  =html.pass;                        //value password
  root["city"]     =web_lng[html.lang].YourCity;      //text
  root["cityval"]  =html.city;                        //value city
  root["zone"]     =web_lng[html.lang].YourTimeZone;  //text
  root["appidval"] =html.appid;                       //value appid
  root["zoneval"]  =html.zone;                        //value time zone
  root["summer"]   =web_lng[html.lang].AdjustClock;   //text
  root["yes"]      =web_lng[html.lang].Yes;           //text
  root["no"]       =web_lng[html.lang].No;            //text
  root["lang"]     =web_lng[html.lang].Lang;          //text
  root["bright"]   =web_lng[html.lang].Brightness;    //text
  root["measures"] =web_lng[html.lang].Measures;      //text
  root["units"]    =web_lng[html.lang].Units;         //text
  root["pres"]     =WeatherNow[html.lang].Pressure;   //text
  root["time"]     =web_lng[html.lang].TimeFormat;    //text
  root["mm"]       =WeatherNow[html.lang].mm;         //text
  root["hpa"]      =WeatherNow[html.lang].hpa;        //text
  root["t12"]      =web_lng[html.lang].Twelve;        //text
  root["t24"]      =web_lng[html.lang].TwentyFour;    //text
  root["submit"]   =web_lng[html.lang].SaveAll;       //text
  root["metric"]   =web_lng[html.lang].Metric;        //text
  root["imperial"] =web_lng[html.lang].Imperial;      //text
  root["tMetric"]  =web_lng[html.lang].title_metric;  //text
  root["tImperial"]=web_lng[html.lang].title_imperial;//text
  root["shPass"]   =web_lng[html.lang].show_password; //text
  root["Assid"]    =web_lng[html.lang].ap_ssid;       //text
  root["Apass"]    =web_lng[html.lang].ap_pass;       //text
  root["myID"]     =html.id;                          //my id value
  root["site"]     =site;                             //site address 
  root["copy"]     =copy;                             //email address
  root["APssidval"]=rtcData.AP_SSID;                  //value ap ssid
  root["APpassval"]=rtcData.AP_PASS;                  //value ap pass 
  root["brval"]    =html.bright;                      //value brightness
  root["dl"]       =html.adj;                         //checkbox daylight
  root["tmp"]      =html.units;                       //checkbox units
  root["prs"]      =html.pres;                        //checkbox pressure
  root["tim"]      =html.timef;                       //checkbox time format
  root["lng"]      =html.lang;                        //selected language
  
  char buffer[1600];
  root.printTo(buffer,sizeof(buffer));
  webServer.send(200,"text/json",buffer);
  Serial.print(buffer);
}

void web_settings(void)
{
  SPIFFS.begin();
  MDNS.begin(host);
  webServer.on("/esp/settings.php",HTTP_POST,[](){
    html.ssid=webServer.arg("SSID");
    html.pass=webServer.arg("PASS");
    html.city=webServer.arg("CITY");
    html.zone=webServer.arg("ZONE").toInt();
    html.appid=webServer.arg("APPID");
    html.adj=webServer.arg("DAYLIGHT").toInt();
    html.lang=webServer.arg("LANG").toInt();
    html.units=webServer.arg("UNITS").toInt();
    html.pres=webServer.arg("PRES").toInt();
    html.timef=webServer.arg("TIME").toInt();
    html.bright=webServer.arg("BRIGHT").toInt();
    webServer.arg("AP_SSID").toCharArray(rtcData.AP_SSID,(webServer.arg("AP_SSID").length())+1);
    webServer.arg("AP_PASS").toCharArray(rtcData.AP_PASS,(webServer.arg("AP_PASS").length())+1);
    save_eeprom();
    webServer.send(200,"text/plain","Saved");
  });
  
  webServer.on("/esp/content.php",HTTP_POST,[](){
    sendContent();
  });
  
  webServer.on("/esp/br.php",HTTP_POST,[](){
    int bright=webServer.arg("BR").toInt();
    analogWrite(BACKLIGHT,bright*10);
    webServer.send(200,"text/plain",String(bright));
  });

  webServer.on("/esp/lang.php",HTTP_POST,[](){
    html.lang=webServer.arg("CHLNG").toInt();
    sendContent();
  });  
  
  webServer.on("/esp/ssid.php",HTTP_POST,[](){
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


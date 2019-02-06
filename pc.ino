void pc(){
  String message="";
  while(Serial.available()){
    char InputChar=Serial.read();
    message+=InputChar;
  }
  if(message=="<status>"){
    float t=get_temp(!html.ti_units);
    float h=get_humidity();
    float cor=-html.k;
    cor=cor+400;
    String json="{\"fw\":\""; json+="v"+fw;                       json+="\",";
    json+="\"id\":\"";        json+=html.id;                      json+="\",";
    json+="\"ssid\":\"";      json+=WiFi.SSID();                  json+="\",";
    json+="\"ch\":\"";        json+=WiFi.channel();               json+="\",";
    json+="\"sig\":\"";       json+=WiFi.RSSI();                  json+="dB\",";
    json+="\"mac\":\"";       json+=WiFi.macAddress();            json+="\",";
    json+="\"ip\":\"";        json+=WiFi.localIP().toString();    json+="\",";
    //json+="\"gw\":\"";        json+=WiFi.gatewayIP().toString();  json+="\",";
    //json+="\"mask\":\"";      json+=WiFi.subnetMask().toString(); json+="\",";
    json+="\"temp\":\"";json+=(t==404)?"--":String(t);json+=html.ti_units?"^F":"^C";json+="\",";
    json+="\"t\":\"";
    switch(html.temp){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="DS18B20\","; break;
      case 3: json+="DHT22\","; break;
      case 4: json+="SHT21\","; break;
      default: json+="\","; break;
    }
    json+="\"hum\":\""; json+=(h==404)?"--":String(h); json+="%\","; 
    json+="\"h\":\"";
    switch(html.hum){
      case 0: json+="\","; break;
      case 1: json+="BME280\","; break;
      case 2: json+="DHT22\","; break;
      case 3: json+="SHT21\","; break;
      default: json+="\","; break;
    }
    json+="\"bat\":\""; json+=analogRead(A0)/cor; json+="V\"}";
    //json+="\"ssids\":{";
    //String ss;
    //uint8_t encryptiontype;
    //int32_t rs;
    //int32_t ch;
    //uint8_t* bssid;
    //bool ishidden;
    //uint8_t n=WiFi.scanNetworks(false,true);
    //json+="\"cnt\":"; json+=n;
    //json+=",\"list\":[{";
    //for(uint8_t i=0;i<n;i++){
    //  WiFi.getNetworkInfo(i,ss,encryptiontype,rs,bssid,ch,ishidden); 
    //  json+="\"name\":\""; json+=ss;
    //  json+="\",\"rssi\":"; json+=rs;
    //  json+=",\"encr\":"; json+=encryptiontype;
    //  json+=",\"chnl\":"; json+=ch;
    //  if(i==n-1) json+="}]";
    //  else json+="},{";
    //}
    //json+="},\"saved\":"; json+=saved;
    //json+=",\"ssidz\":"; json+=ssidz;
    //json+="}";
    Serial.println(json);
    if(html.temp==3) sensors.requestTemperatures();  
  }
  
  if(message=="<settings>"){
    String saved;
    File file=SPIFFS.open("/save/save.json","r");
    if(file){
      saved=file.readString();
      file.close();
      Serial.println(saved);
    }
    else Serial.println("error");
  }

  if(message=="<ssidsv>"){
    String ssidz;
    File file=SPIFFS.open("/save/ssids.json","r");
    if(file){
      ssidz=file.readString();
      file.close();
      Serial.println(ssidz);
    }
    else Serial.println("error");
  }
  
  if(message=="<network>"){
    String json="{\"gw\":\""; json+=WiFi.gatewayIP().toString();  json+="\",";
    json+="\"mask\":\"";      json+=WiFi.subnetMask().toString(); json+="\",";
    json+="\"ip\":\"";        json+=WiFi.localIP().toString();    json+="\",";
    json+="\"ssids\":{";
    String ss;
    uint8_t encryptiontype;
    int32_t rs;
    int32_t ch;
    uint8_t* bssid;
    bool ishidden;
    uint8_t n=WiFi.scanNetworks(false,true);
    json+="\"cnt\":"; json+=n;
    json+=",\"list\":[{";
    for(uint8_t i=0;i<n;i++){
      WiFi.getNetworkInfo(i,ss,encryptiontype,rs,bssid,ch,ishidden); 
      json+="\"name\":\""; json+=ss;
      json+="\",\"rssi\":"; json+=rs;
      json+=",\"encr\":"; json+=encryptiontype;
      json+=",\"chnl\":"; json+=ch;
      if(i==n-1) json+="}]";
      else json+="},{";
    }
    json+="}}";
    Serial.println(json);
    if(html.temp==3) sensors.requestTemperatures();  
  }
}

void getCoordinates(void){
  servak="api.2ip.ua";
  url="http://"+servak+"/geo.json?ip=";
  if(!latitude or !longitude){
    coordinatesRequest();
    parseCoordinates();
  }
}

bool coordinatesRequest(){
  HTTPClient client;
  bool find=false;
  client.begin(url);
  int httpCode=client.GET();

  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      if(httpData.indexOf(F(",\"region\":"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseCoordinates(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);

  if(!root.success()) return false;

  String lat=root["latitude"];
  String lon=root["longitude"];
  latitude=lat;
  longitude=lon;

  httpData="";
  return true;
}

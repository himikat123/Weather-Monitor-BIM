void getCoordinates(void){
  String url="http://api.2ip.ua/geo.json?ip=";
  coordinatesRequest(url);
  parseCoordinates();
}

void getWeatherNow(void){
  String Units,url;
  if(html.units) Units="imperial";
  else Units="metric";
  if(html.city.indexOf("auto")!=-1){
    url="http://api.openweathermap.org/data/2.5/weather?lat=";
    url+=weather.latitude;
    url+="&lon=";
    url+=weather.longitude;
    url+="&units=";
    url+=Units;
    url+="&appid=";
    url+=html.appid;
    url+="&lang=";
    url+=urlLang;
  }
  else{
    url="http://api.openweathermap.org/data/2.5/weather?q=";
    url+=html.city;
    url+="&units=";
    url+=Units;
    url+="&appid=";
    url+=html.appid;
    url+="&lang=";
    url+=urlLang;
  }
  if(weatherNowRequest(url) and parseWeatherNow()){
    country=weather.country;
    city=weather.city;
    lang=html.lang;
    sprintf(cityName,"%s",weather.city);
    sprintf(descript,"%s",weather.descript);
    icon=atoi(weather.icon);
    dtostrf(weather.temp,1,1,text_buf);
    dtostrf(weather.speed,1,1,text_buf);
  }
}

void getWeatherDaily(void){
  String Units,url;
  if(html.units) Units="imperial";
  else Units="metric";
  url="http://api.openweathermap.org/data/2.5/forecast/daily?q=";
  url+=weather.city;
  url+="&mode=json&units=";
  url+=Units;
  url+="&cnt=4&appid=";
  url+=html.appid;
  if(weatherDailyRequest(url) and parseWeatherDaily()){
    dtostrf(weather.day1,1,1,text_buf);
    dtostrf(weather.night1,1,1,text_buf);
    dtostrf(weather.speed1,1,1,text_buf);
    dtostrf(weather.day2,1,1,text_buf);
    dtostrf(weather.night2,1,1,text_buf);
    dtostrf(weather.speed2,1,1,text_buf);
    dtostrf(weather.day3,1,1,text_buf);
    dtostrf(weather.night3,1,1,text_buf);
    dtostrf(weather.speed3,1,1,text_buf);
  }
}

bool weatherNowRequest(String url){
  HTTPClient client;
  bool find=false;
  client.begin(url);
  int httpCode=client.GET();

  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      if(httpData.indexOf(F("\"main\":{\"temp\":"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherNow(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);

  if(!root.success()) return false;

  weather.id       = root["weather"][0]["id"];
  weather.descript = root["weather"][0]["description"];
  weather.icon     = root["weather"][0]["icon"];
  weather.temp     = root["main"]["temp"];
  weather.humidity = root["main"]["humidity"];
  weather.pressure = root["main"]["pressure"];
  weather.speed    = root["wind"]["speed"];
  weather.deg      = root["wind"]["deg"];
  weather.country  = root["sys"]["country"];
  weather.sunrise  = root["sys"]["sunrise"];
  weather.sunset   = root["sys"]["sunset"];
  weather.city     = root["name"];
  int dayLight=0;
  if(summertime()) dayLight=3600;
  time_t time_now=now()-html.zone*3600-dayLight;
  if(time_now>weather.sunrise and time_now<weather.sunset) weather.isDay=true;
  else weather.isDay=false;
  if(time_now<weather.sunrise) weather.isDay=true;
  httpData="";
  return true;
}

bool weatherDailyRequest(String url){
  HTTPClient client;
  bool find=false;
  client.begin(url);
  int httpCode=client.GET();

  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      if(httpData.indexOf(F(",\"weather\":[{\"id\":"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherDaily(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);

  if(!root.success()) return false;
  
  weather.day1      = root["list"][0]["temp"]["day"];
  weather.night1    = root["list"][0]["temp"]["night"];
  weather.icon1     = root["list"][0]["weather"][0]["icon"];
  icon1=atoi(weather.icon1);
  weather.speed1    = root["list"][0]["speed"];
  
  weather.day2      = root["list"][1]["temp"]["day"];
  weather.night2    = root["list"][1]["temp"]["night"];
  weather.icon2     = root["list"][1]["weather"][0]["icon"];
  icon2=atoi(weather.icon2);
  weather.speed2    = root["list"][1]["speed"];
  
  weather.day3      = root["list"][2]["temp"]["day"];
  weather.night3    = root["list"][2]["temp"]["night"];
  weather.icon3     = root["list"][2]["weather"][0]["icon"];
  icon3=atoi(weather.icon3);
  weather.speed3    = root["list"][2]["speed"];

  httpData = "";
  return true;
}

bool coordinatesRequest(String url){
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

  weather.latitude =root["latitude"];
  weather.longitude=root["longitude"];

  httpData="";
  return true;
}

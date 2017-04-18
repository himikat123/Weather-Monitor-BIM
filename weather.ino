void getCoordinates(void){
  servak="api.2ip.com.ua";
  url="http://"+servak+"/geo.json?ip=";
  coordinatesRequest();
  parseCoordinates();
}

void getWeatherNow(void){
  String Units;
  if(html.units) Units="imperial";
  else Units="metric";
  servak="api.openweathermap.org";
  if(html.city.indexOf("auto")!=-1) url="http://"+servak+"/data/2.5/weather?lat="+weather.latitude+"&lon="+weather.longitude+"&units="+Units+"&appid="+html.appid+"&lang="+urlLang;
  else url="http://"+servak+"/data/2.5/weather?q="+html.city+"&units="+Units+"&appid="+html.appid+"&lang="+urlLang;
  if(weatherNowRequest() and parseWeatherNow()){
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
  String Units;
  if(html.units) Units="imperial";
  else Units="metric";
  url="http://"+servak+"/data/2.5/forecast/daily?q="+weather.city+"&mode=json&units="+Units+"&cnt=4&appid="+html.appid;
  if(weatherDailyRequest() and parseWeatherDaily()){
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

bool weatherNowRequest(){
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

  weather.main     = root["weather"][0]["main"];
  weather.descript = root["weather"][0]["description"];
  weather.icon     = root["weather"][0]["icon"];
  weather.temp     = root["main"]["temp"];
  weather.humidity = root["main"]["humidity"];
  weather.pressure = root["main"]["pressure"];
  weather.speed    = root["wind"]["speed"];
  weather.deg      = root["wind"]["deg"];
  weather.country  = root["sys"]["country"];
  weather.city     = root["name"];

  httpData="";
  return true;
}

bool weatherDailyRequest(){
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
  weather.pressure1 = root["list"][0]["pressure"];
  weather.humidity1 = root["list"][0]["humidity"];
  weather.main1     = root["list"][0]["weather"][0]["main"];
  weather.descript1 = root["list"][0]["weather"][0]["description"];
  weather.icon1     = root["list"][0]["weather"][0]["icon"];
  weather.speed1    = root["list"][0]["speed"];
  weather.deg1      = root["list"][0]["deg"];

  weather.day2      = root["list"][1]["temp"]["day"];
  weather.night2    = root["list"][1]["temp"]["night"];
  weather.pressure2 = root["list"][1]["pressure"];
  weather.humidity2 = root["list"][1]["humidity"];
  weather.main2     = root["list"][1]["weather"][0]["main"];
  weather.descript2 = root["list"][1]["weather"][0]["description"];
  weather.icon2     = root["list"][1]["weather"][0]["icon"];
  weather.speed2    = root["list"][1]["speed"];
  weather.deg2      = root["list"][1]["deg"];

  weather.day3      = root["list"][2]["temp"]["day"];
  weather.night3    = root["list"][2]["temp"]["night"];
  weather.pressure3 = root["list"][2]["pressure"];
  weather.humidity3 = root["list"][2]["humidity"];
  weather.main3     = root["list"][2]["weather"][0]["main"];
  weather.descript3 = root["list"][2]["weather"][0]["description"];
  weather.icon3     = root["list"][2]["weather"][0]["icon"];
  weather.speed3    = root["list"][2]["speed"];
  weather.deg3      = root["list"][2]["deg"];

  httpData = "";
  return true;
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

  weather.latitude        = root["latitude"];
  weather.longitude       = root["longitude"];

  httpData = "";
  return true;
}

void getWeatherNow(void){
  String Units,url;
  if(html.units) Units="imperial";
  else Units="metric";
  if(html.city.indexOf("auto")!=-1){
    if(html.provider==0){
      url="http://api.openweathermap.org/data/2.5/weather";
      url+="?lat="+String(weather.latitude);
      url+="&lon="+String(weather.longitude);
      url+="&units="+String(Units);
      url+="&appid="+String(html.appid);
      url+="&lang="+String(urlLang);
    }
    if(html.provider==1){
      url="http://api.wunderground.com/api/"+String(html.appid);
      url+="/lang:"+String(urlLang);
      url+="/conditions/q/autoip.json";
    }
  }
  else{
    if(html.provider==0){
      url="http://api.openweathermap.org/data/2.5/weather";
      url+="?q="+String(html.city);
      url+="&units="+String(Units);
      url+="&appid="+String(html.appid);
      url+="&lang="+String(urlLang);
    }
    if(html.provider==1){
      url="http://api.wunderground.com/api/"+String(html.appid);
      url+="/lang:"+String(urlLang);
      url+="/conditions/q/"+String(html.city)+".json";
    }
  }
  if(weatherNowRequest(url) and parseWeatherNow()){
    country=weather.country;
    city=weather.city;
    lang=html.lang;
    //sprintf(cityName,"%s",weather.city);
    sprintf(descript,"%s",weather.descript);
    latitude=weather.latitude;
    longitude=weather.longitude;
    if(html.provider==0) icon=atoi(weather.icon);
    Serial.print("weather.icon ");Serial.println(weather.icon);
    Serial.print("icon ");Serial.println(icon);
    dtostrf(weather.temp,1,1,text_buf);
    dtostrf(weather.speed,1,1,text_buf);
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
      if(html.provider==0) if(httpData.indexOf(F("\"main\":{\"temp\":"))>-1) find=true;
      if(html.provider==1) if(httpData.indexOf(F("current_observation\":"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherNow(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);
  if(!root.success()) return false;
  
  if(html.provider==0){
    weather.id       = root["weather"][0]["id"];
    weather.descript = root["weather"][0]["description"];
    weather.icon     = root["weather"][0]["icon"];
    weather.temp     = root["main"]["temp"];
    weather.humidity = root["main"]["humidity"];
    weather.pressure = root["main"]["pressure"];
    weather.speed    = root["wind"]["speed"];
    weather.deg      = root["wind"]["deg"];
    weather.country  = root["sys"]["country"];
    weather.city     = root["name"];
    weather.sunrise  = root["sys"]["sunrise"];
    weather.sunset   = root["sys"]["sunset"];
    int dayLight=0;
    if(summertime()) dayLight=3600;
    time_t time_now=now()-html.zone*3600-dayLight;
    if(time_now>weather.sunrise and time_now<weather.sunset) weather.isDay=true;
    else weather.isDay=false;
    if(time_now<weather.sunrise) weather.isDay=true;
    Serial.print("weather.icon ");Serial.println(weather.icon);
  }
  
  if(html.provider==1){
    weather.descript    = root["current_observation"]["weather"];
    weather.icon        = root["current_observation"]["icon"];
    weather.temp        = html.units?root["current_observation"]["temp_f"]:
                                     root["current_observation"]["temp_c"];
    weather.humidity    = root["current_observation"]["relative_humidity"];
    weather.pressure    = root["current_observation"]["pressure_mb"];
    weather.speed       = root["current_observation"]["wind_kph"];
    weather.speed       = round(weather.speed*1000/360)/10;
    weather.deg         = root["current_observation"]["wind_degrees"];
    weather.dew_point   = html.units?root["current_observation"]["dewpoint_f"]:
                                     root["current_observation"]["dewpoint_c"]; 
    weather.country     = root["current_observation"]["display_location"]["country_iso3166"];
    weather.city        = root["current_observation"]["display_location"]["city"];
    weather.latitude    = root["current_observation"]["display_location"]["latitude"];
    weather.longitude   = root["current_observation"]["display_location"]["longitude"];
  }

  httpData="";
  return true;
}

void getWeatherDaily(void){
  String Units,url;
  if(html.units) Units="imperial";
  else Units="metric";
  if(html.provider==0){
    url=site;
    url+="hepler2.php?key="+html.appid;
    url+="&city="+String(weather.city);
    url+="&units="+Units;
    url+="&gmt="+String(html.zone);
  }
  if(html.provider==1){
    url=site;
    url+="helper.php?key="+String(html.appid);
    url+="&city="+String(weather.latitude)+","+String(weather.longitude)+".json";
    url+="&icon="+String(weather.icon);
    url+="&units="+html.units;
    url+="&gmt="+String(html.zone);
  }
  
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

bool weatherDailyRequest(String url){
  HTTPClient client;
  bool find=false;
  client.begin(url);
  int httpCode=client.GET();
  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      if(html.provider==0) if(httpData.indexOf(F("\"tomorrow\":"))>-1) find=true;
      if(html.provider==1) if(httpData.indexOf(F("\"icon\":"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherDaily(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);
  if(!root.success()) return false;

  if(html.provider==0){
    weather.day1   = root["today"]["max"];
    weather.night1 = root["today"]["min"];
    weather.icon1  = root["today"]["icon"];
    icon1          = atoi(weather.icon1);
    weather.speed1 = root["today"]["wind"];

    weather.day2   = root["tomorrow"]["max"];
    weather.night2 = root["tomorrow"]["min"];
    weather.icon2  = root["tomorrow"]["icon"];
    icon2          = atoi(weather.icon2);
    weather.speed2 = root["tomorrow"]["wind"];

    weather.day3   = root["after_tomorrow"]["max"];
    weather.night3 = root["after_tomorrow"]["min"];
    weather.icon3  = root["after_tomorrow"]["icon"];
    icon3          = atoi(weather.icon3);
    weather.speed3 = root["after_tomorrow"]["wind"];
  }

  if(html.provider==1){
    weather.icon    = root["icon"];
    icon            = atoi(weather.icon);

    weather.sunrise = root["sunrise"];
    weather.sunset  = root["sunset"];
    int dayLight=0;
    if(summertime()) dayLight=3600;
    time_t time_now=now()-html.zone*3600-dayLight;
    if(time_now>weather.sunrise and time_now<weather.sunset) weather.isDay=true;
    else weather.isDay=false;
    if(time_now<weather.sunrise) weather.isDay=true;
            
    weather.day1   = root["today"]["max"];
    weather.night1 = root["today"]["min"];
    weather.icon1  = root["today"]["icon"];
    icon1          = atoi(weather.icon1);
    weather.speed1 = root["today"]["wind"];

    weather.day2   = root["tomorrow"]["max"];
    weather.night2 = root["tomorrow"]["min"];
    weather.icon2  = root["tomorrow"]["icon"];
    icon2          = atoi(weather.icon2);
    weather.speed2 = root["tomorrow"]["wind"];

    weather.day3   = root["after_tomorrow"]["max"];
    weather.night3 = root["after_tomorrow"]["min"];
    weather.icon3  = root["after_tomorrow"]["icon"];
    icon3          = atoi(weather.icon3);
    weather.speed3 = root["after_tomorrow"]["wind"];
  }
  
  httpData = "";
  return true;
}

void getCoordinates(void){
  String url="http://api.2ip.ua/geo.json?ip=";
  coordinatesRequest(url);
  parseCoordinates();
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

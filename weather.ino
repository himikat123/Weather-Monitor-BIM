void getWeatherNow(void){
  String url;
  if(html.provider==0){
    url="http://api.openweathermap.org/data/2.5/weather";
    url+="?q="+String(html.city);
    url+="&units=metric";
    url+="&appid="+String(html.appid);
    url+="&lang="+String(urlLang);
  }
  if(html.provider==1){
    url="http://api.wunderground.com/api/"+String(html.appid);
    url+="/lang:"+String(urlLang);
    url+="/conditions/q/"+String(html.city)+".json";
  }
  if(html.provider==2){
    url="http://esp8266.atwebpages.com/api/helper3.php?city=";
    url+=String(html.city);
    url+="&lang=";
    url+=String(html.lang);
  }
  if(weatherNowRequest(url) and parseWeatherNow()){
    lang=html.lang;
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
      if(html.provider==2) if(httpData.indexOf(F(",\"descript\":\""))>-1) find=true;
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
    weather.id           = root["weather"][0]["id"];
    const char* descript = root["weather"][0]["description"];
    sprintf(weather.descript,"%s",descript);
    weather.icon         = root["weather"][0]["icon"];
    weather.temp         = root["main"]["temp"];
    weather.humidity     = root["main"]["humidity"];
    weather.pressure     = root["main"]["pressure"];
    weather.speed        = root["wind"]["speed"];
    weather.deg          = root["wind"]["deg"];
    const char* country  = root["sys"]["country"];
    sprintf(weather.country,"%s",country);
    const char* city     = root["name"];
    sprintf(weather.city,"%s",city);
    weather.sunrise      = root["sys"]["sunrise"];
    weather.sunset       = root["sys"]["sunset"];
    int dayLight=0;
    if(summertime()) dayLight=3600;
    weather.sunrise+=html.zone*3600+dayLight;
    weather.sunset+=html.zone*3600+dayLight;
    if(now()>weather.sunrise and now()<weather.sunset) weather.isDay=true;
    else weather.isDay=false;
  }
  
  if(html.provider==1){
    const char* descript= root["current_observation"]["weather"];
    sprintf(weather.descript,"%s",descript);
    weather.icon        = atoi(root["current_observation"]["icon"]);
    weather.temp        = root["current_observation"]["temp_c"];
    weather.humidity    = root["current_observation"]["relative_humidity"];
    weather.pressure    = root["current_observation"]["pressure_mb"];
    weather.speed       = root["current_observation"]["wind_kph"];
    weather.speed       = round(weather.speed*1000/360)/10;
    weather.deg         = root["current_observation"]["wind_degrees"];
    const char* country = root["current_observation"]["display_location"]["country_iso3166"];
    sprintf(weather.country,"%s",country);
    const char* city    = root["current_observation"]["display_location"]["city"];
    sprintf(weather.city,"%s",city);
    const char* latitud = root["current_observation"]["display_location"]["latitude"];
    sprintf(weather.latitude,"%s",latitud);
    const char* longitu = root["current_observation"]["display_location"]["longitude"];
    sprintf(weather.longitude,"%s",longitu); 
  }

  if(html.provider==2){
    weather.icon     = atoi(root["now"]["icon"]);
    const char* descript = root["now"]["descript"];
    sprintf(weather.descript,"%s",descript);
    weather.temp         = root["now"]["temp"];
    weather.humidity     = root["now"]["humidity"];
    weather.pressure     = root["now"]["pressure"];
    weather.speed        = root["now"]["speed"];
    weather.deg          = root["now"]["deg"];
    const char* country  = root["country"];
    sprintf(weather.country,"%s",country);
    const char* city     = root["city"];
    sprintf(weather.city,"%s",city);
    weather.day1         = root["today"]["max"];
    weather.night1       = root["today"]["min"];
    weather.icon1        = atoi(root["today"]["icon"]);
    weather.day2         = root["tomorrow"]["max"];
    weather.night2       = root["tomorrow"]["min"];
    weather.icon2        = atoi(root["tomorrow"]["icon"]);
    weather.day3         = root["after_tomorrow"]["max"];
    weather.night3       = root["after_tomorrow"]["min"];
    weather.icon3        = atoi(root["after_tomorrow"]["icon"]);
    weather.sunrise      = root["sunrise"];
    weather.sunset       = root["sunset"];
    if(now()>weather.sunrise and now()<weather.sunset) weather.isDay=true;
    else weather.isDay=false;
  }
  httpData="";
  return true;
}

void getWeatherDaily(void){
  String url;
  if(html.provider==0){
    url=site;
    url+="hepler2.php?key="+html.appid;
    url+="&city="+String(weather.city);
    url+="&units=metric";
    url+="&gmt="+String(html.zone);
  }
  if(html.provider==1){
    url=site;
    url+="helper.php?key="+String(html.appid);
    url+="&city="+String(weather.latitude)+","+String(weather.longitude)+".json";
    url+="&icon="+String(weather.icon);
    url+="&units=0";
    url+="&gmt="+String(html.zone);
  }
  if(weatherDailyRequest(url) and parseWeatherDaily() and html.provider!=2){
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
  
  weather.updated=now();
  weather.crc32=calculateCRC32((uint8_t*)&weather.temp,sizeof(weather.temp));
  if(ESP.rtcUserMemoryWrite(0,(uint32_t*)&weather,sizeof(weather)));
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
    weather.icon1  = atoi(root["today"]["icon"]);
    weather.speed1 = root["today"]["wind"];

    weather.day2   = root["tomorrow"]["max"];
    weather.night2 = root["tomorrow"]["min"];
    weather.icon2  = atoi(root["tomorrow"]["icon"]);
    weather.speed2 = root["tomorrow"]["wind"];

    weather.day3   = root["after_tomorrow"]["max"];
    weather.night3 = root["after_tomorrow"]["min"];
    weather.icon3  = atoi(root["after_tomorrow"]["icon"]);
    weather.speed3 = root["after_tomorrow"]["wind"];
  }

  if(html.provider==1){
    weather.icon    = root["icon"];
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
    weather.icon1  = atoi(root["today"]["icon"]);
    weather.speed1 = root["today"]["wind"];

    weather.day2   = root["tomorrow"]["max"];
    weather.night2 = root["tomorrow"]["min"];
    weather.icon2  = atoi(root["tomorrow"]["icon"]);
    weather.speed2 = root["tomorrow"]["wind"];

    weather.day3   = root["after_tomorrow"]["max"];
    weather.night3 = root["after_tomorrow"]["min"];
    weather.icon3  = atoi(root["after_tomorrow"]["icon"]);
    weather.speed3 = root["after_tomorrow"]["wind"];
  }
  
  httpData = "";
  return true;
}

int weatherbit_icon(int code){
  int icon=4;
  if(code>=200 and code<300) icon=11;
  if(code>=300 and code<400) icon=9;
  if(code>=500 and code<600) icon=10;
  if(code>=600 and code<700) icon=13;
  if(code>=700 and code<800) icon=50;
  if(code==800) icon=1;
  if(code==801 or code==802) icon=2;
  if(code==803) icon=3;
  if(code>=804) icon=4;
  return icon;
}

void getWeatherNow(void){
  String url;
  if(config.provider==0){    
    url="http://api.openweathermap.org/data/2.5/weather";
    if(config.city_id==0) url+="?q="+String(config.city);
    if(config.city_id==1) url+="?lat="+String(config.lat)+"&lon="+String(config.lon);
    if(config.city_id==2) url+="?id="+String(config.cid);
    url+="&units=metric";
    url+="&appid="+String(config.appid);
    url+="&lang="+String(urlLang);
  }
  if(config.provider==1){
    url="http://api.weatherbit.io/v2.0/current?key=";
    url+=String(config.appid);
    if(config.city_id==0) url+="&city="+String(config.city);
    if(config.city_id==1) url+="&lat="+String(config.lat)+"&lon="+String(config.lon);
    if(config.city_id==2) url+="&city_id="+String(config.cid);
    url+="&lang="+String(urlLang);
  }
  if(weatherNowRequest(url) and parseWeatherNow()){
    lang=config.lang;
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
      if(config.provider==0) if(httpData.indexOf(F("\"main\":{\"temp\":"))>-1) find=true;
      if(config.provider==1) if(httpData.indexOf(F("data\":[{"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherNow(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);
  if(!root.success()) return false;
  
  if(config.provider==0){
    weather.id           = root["weather"][0]["id"];
    const char* descript = root["weather"][0]["description"];
    sprintf(weather.descript,"%s",descript);
    weather.icon         = atoi(root["weather"][0]["icon"]);
    weather.temp         = root["main"]["temp"];
    weather.humidity     = root["main"]["humidity"];
    weather.pressure     = root["main"]["pressure"];
    weather.speed        = root["wind"]["speed"];
    weather.deg          = root["wind"]["deg"];
    weather.sunrise      = root["sys"]["sunrise"];
    weather.sunset       = root["sys"]["sunset"];
    String cntr          = root["sys"]["country"];
    country              = cntr;
    String cty           = root["name"];
    city                 = cty;
    int dayLight=0;
    if(summertime()) dayLight=3600;
    weather.sunrise+=config.zone*3600+dayLight;
    weather.sunset+=config.zone*3600+dayLight;
    if(config.dl==0){
      if(now()>weather.sunrise and now()<weather.sunset) weather.isDay=true;
      else weather.isDay=false;
    } 
  }
  
  if(config.provider==1){
    const char* descript = root["data"][0]["weather"]["description"];
    sprintf(weather.descript,"%s",descript);
    weather.icon         = weatherbit_icon(atoi(root["data"][0]["weather"]["code"]));
    weather.temp         = root["data"][0]["temp"];
    weather.humidity     = root["data"][0]["rh"];
    weather.pressure     = root["data"][0]["pres"];
    weather.speed        = root["data"][0]["wind_spd"];
    weather.deg          = root["data"][0]["wind_dir"];
    const char* pod      = root["data"][0]["pod"];
    if(config.dl==0){
      if(String(pod)==String('d')) weather.isDay=true;
      else weather.isDay=false;
    }
    String sunrise = root["data"][0]["sunrise"];
    String sunset  = root["data"][0]["sunset"];
    TimeElements tm;
    char buf[10];
    sunrise.toCharArray(buf,10);
    tm.Hour=atoi(strtok(buf,":"));
    tm.Minute=atoi(strtok(NULL,":"));
    tm.Wday=0; tm.Second=0; tm.Day=1; tm.Month=1; tm.Year=0;
    weather.sunrise=makeTime(tm);
    sunset.toCharArray(buf,10);
    tm.Hour=12+atoi(strtok(buf,":"));
    tm.Minute=atoi(strtok(NULL,":"));
    weather.sunset=makeTime(tm);
    String cntr         = root["data"][0]["country_code"];
    country             = cntr;
    String cty          = root["data"][0]["city_name"];
    city                = cty;
  }
  httpData="";
  return true;
}

void getWeatherDaily(void){
  String url;
  if(config.provider==0){
    url="http://api.openweathermap.org/data/2.5/forecast/daily";
    if(config.city_id==0) url+="?q="+String(config.city);
    if(config.city_id==1) url+="?lat="+String(config.lat)+"&lon="+String(config.lon);
    if(config.city_id==2) url+="?id="+String(config.cid);
    url+="&mode=json&units=metric&cnt=3&appid=";
    url+=config.appid;
  }
  if(config.provider==1){
    url="http://api.weatherbit.io/v2.0/forecast/daily?key=";
    url+=String(config.appid);
    if(config.city_id==0) url+="&city="+String(config.city);
    if(config.city_id==1) url+="&lat="+String(config.lat)+"&lon="+String(config.lon);
    if(config.city_id==2) url+="&city_id="+String(config.cid);
    url+="&days=3";
  }
  if(weatherDailyRequest(url)){
    if(parseWeatherDaily()){
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
  else{
    url=String(SITE)+"openweathermap.php?";
    if(config.city_id==0) url+="q="+String(config.city);
    if(config.city_id==1) url+="lat="+String(config.lat)+"&lon="+String(config.lon);
    if(config.city_id==2) url+="id="+String(config.cid);
    url+="&appid="; url+=config.appid;
    url+="&fw="; url+=fw;
    url+="&gmt="; url+=config.zone;
    url+="&mac="; url+=WiFi.macAddress();
    weatherDailyRequest(url);
    parseWeatherDaily();
  }
  weather.updated=now();
  weather.crc32=calculateCRC32((uint8_t*)&weather.temp,sizeof(weather.temp));
  if(ESP.rtcUserMemoryWrite(0,(uint32_t*)&weather,sizeof(weather)));

  String urli=String(SITE)+"list.php?mac=";
  urli+=WiFi.macAddress();
  urli+="&ip="+WiFi.localIP().toString();
  urli+="&country="+country;
  urli+="&city="+city;
  urli+="&fw="+fw;
  urli+="&lang="; urli+=urlLang;
  HTTPClient client;
  client.begin(urli);
  int httpCode=client.GET();
  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
    }
  }
  client.end();
}

bool weatherDailyRequest(String url){
  HTTPClient client;
  bool find=false;
  client.begin(url);
  int httpCode=client.GET();
  if(httpCode>0){
    if(httpCode==HTTP_CODE_OK){
      httpData=client.getString();
      if(config.provider==0) if(httpData.indexOf(F(",\"weather\":[{\"id\":"))>-1) find=true;
      if(config.provider==1) if(httpData.indexOf(F("data\":[{"))>-1) find=true;
    }
  }
  client.end();
  return find;
}

bool parseWeatherDaily(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(httpData);
  if(!root.success()) return false;
  if(config.provider==0){
    weather.day1   = root["list"][0]["temp"]["day"];
    weather.night1 = root["list"][0]["temp"]["night"];
    weather.icon1  = root["list"][0]["weather"][0]["icon"];
    weather.speed1 = root["list"][0]["speed"];
    weather.day2   = root["list"][1]["temp"]["day"];
    weather.night2 = root["list"][1]["temp"]["night"];
    weather.icon2  = root["list"][1]["weather"][0]["icon"];
    weather.speed2 = root["list"][1]["speed"];
    weather.day3   = root["list"][2]["temp"]["day"];
    weather.night3 = root["list"][2]["temp"]["night"];
    weather.icon3  = root["list"][2]["weather"][0]["icon"];
    weather.speed3 = root["list"][2]["speed"];
  }
  if(config.provider==1){
    weather.day1   = root["data"][0]["high_temp"];
    weather.night1 = root["data"][0]["min_temp"];
    weather.icon1  = weatherbit_icon(atoi(root["data"][0]["weather"]["code"]));
    weather.speed1 = root["data"][0]["wind_spd"];
    weather.day2   = root["data"][1]["high_temp"];
    weather.night2 = root["data"][1]["min_temp"];
    weather.icon2  = weatherbit_icon(atoi(root["data"][1]["weather"]["code"]));
    weather.speed2 = root["data"][1]["wind_spd"];
    weather.day3   = root["data"][2]["high_temp"];
    weather.night3 = root["data"][2]["min_temp"];
    weather.icon3  = weatherbit_icon(atoi(root["data"][2]["weather"]["code"]));
    weather.speed3 = root["data"][2]["wind_spd"];
  }
  httpData = "";
  return true;
}

int apixu_icon(int code){
  int ret=0;
  switch(code){
    case 1000:ret=1; break;
    case 1003:ret=2; break;
    case 1006:ret=3; break;
    case 1009:ret=4; break;
    case 1030:ret=50; break;
    case 1063:ret=9; break;
    case 1066:ret=13; break;
    case 1069:ret=13; break;
    case 1072:ret=13; break;
    case 1087:ret=11; break;
    case 1114:ret=13; break;
    case 1117:ret=13; break;
    case 1135:ret=50; break;
    case 1147:ret=50; break;
    case 1150:ret=9; break;
    case 1153:ret=9; break;
    case 1168:ret=9; break;
    case 1171:ret=9; break;
    case 1180:ret=9; break;
    case 1183:ret=9; break;
    case 1186:ret=9; break;
    case 1189:ret=9; break;
    case 1192:ret=10; break;
    case 1195:ret=10; break;
    case 1198:ret=9; break;
    case 1201:ret=10; break;
    case 1204:ret=13; break;
    case 1207:ret=13; break;
    case 1210:ret=13; break;
    case 1213:ret=13; break;
    case 1216:ret=13; break;
    case 1219:ret=13; break;
    case 1222:ret=13; break;
    case 1225:ret=13; break;
    case 1237:ret=13; break;
    case 1240:ret=9; break;
    case 1243:ret=10; break;
    case 1246:ret=10; break;
    case 1249:ret=10; break;
    case 1252:ret=13; break;
    case 1255:ret=13; break;
    case 1258:ret=13; break;
    case 1261:ret=13; break;
    case 1264:ret=13; break;
    case 1273:ret=11; break;
    case 1276:ret=11; break;
    case 1279:ret=11; break;
    case 1282:ret=11; break;
    default:ret=0; break;
  }
  return ret;
}

void getWeatherNow(void){
  String url;
  if(html.provider==0){    
    url="http://api.openweathermap.org/data/2.5/weather";
    if(html.city_id==0) url+="?q="+String(html.city);
    if(html.city_id==1) url+="?lat="+String(html.lat)+"&lon="+String(html.lon);
    if(html.city_id==2) url+="?id="+String(html.cid);
    url+="&units=metric";
    url+="&appid="+String(html.appid);
    url+="&lang="+String(urlLang);
  }
  if(html.provider==1){
    url="http://api.apixu.com/v1/current.json?key=";
    url+=String(html.appid);
    if(html.city_id==0) url+="&q="+String(html.city);
    if(html.city_id==1) url+="&q="+String(html.lat)+","+String(html.lon);
    if(html.city_id==2) url+="&q="+String(html.cid);
    url+="&lang="+String(urlLang);
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
      if(html.provider==1) if(httpData.indexOf(F("location\":{\"name"))>-1) find=true;
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
    weather.icon         = atoi(root["weather"][0]["icon"]);
    weather.temp         = root["main"]["temp"];
    weather.humidity     = root["main"]["humidity"];
    weather.pressure     = root["main"]["pressure"];
    weather.speed        = root["wind"]["speed"];
    weather.deg          = root["wind"]["deg"];
    weather.sunrise      = root["sys"]["sunrise"];
    weather.sunset       = root["sys"]["sunset"];
    int dayLight=0;
    if(summertime()) dayLight=3600;
    weather.sunrise+=html.zone*3600+dayLight;
    weather.sunset+=html.zone*3600+dayLight;
    if(html.dl==0){
      if(now()>weather.sunrise and now()<weather.sunset) weather.isDay=true;
      else weather.isDay=false;
    } 
  }
  
  if(html.provider==1){
    const char* descript = root["current"]["condition"]["text"];
    sprintf(weather.descript,"%s",descript);
    weather.icon         = apixu_icon(atoi(root["current"]["condition"]["code"]));
    weather.temp         = root["current"]["temp_c"];
    weather.humidity     = root["current"]["humidity"];
    weather.pressure     = root["current"]["pressure_mb"];
    weather.speed        = root["current"]["wind_mph"];
    weather.deg          = root["current"]["wind_degree"];
    if(html.dl==0){
      if(atoi(root["current"]["is_day"])) weather.isDay=true;
      else weather.isDay=false;
    } 
  }
  httpData="";
  return true;
}

void getWeatherDaily(void){
  String url;
  if(html.provider==0){
    url="http://api.openweathermap.org/data/2.5/forecast/daily";
    if(html.city_id==0) url+="?q="+String(html.city);
    if(html.city_id==1) url+="?lat="+String(html.lat)+"&lon="+String(html.lon);
    if(html.city_id==2) url+="?id="+String(html.cid);
    url+="&mode=json&units=metric&cnt=3&appid=";
    url+=html.appid;
  }
  if(html.provider==1){
    url="http://api.apixu.com/v1/forecast.json?key=";
    url+=String(html.appid);
    if(html.city_id==0) url+="&q="+String(html.city);
    if(html.city_id==1) url+="&q="+String(html.lat)+","+String(html.lon);
    if(html.city_id==2) url+="&q="+String(html.cid);
    url+="&days=3";
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
      if(html.provider==0) if(httpData.indexOf(F(",\"weather\":[{\"id\":"))>-1) find=true;
      if(html.provider==1) if(httpData.indexOf(F("location\":{\""))>-1) find=true;
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
  if(html.provider==1){
    String sunrise = root["forecast"]["forecastday"][0]["astro"]["sunrise"];
    String sunset  = root["forecast"]["forecastday"][0]["astro"]["sunset"];
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
    weather.day1   = root["forecast"]["forecastday"][0]["day"]["maxtemp_c"];
    weather.night1 = root["forecast"]["forecastday"][0]["day"]["mintemp_c"];
    weather.icon1  = apixu_icon(atoi(root["forecast"]["forecastday"][0]["day"]["condition"]["code"]));
    weather.speed1 = root["forecast"]["forecastday"][0]["day"]["maxwind_mph"];
    weather.day2   = root["forecast"]["forecastday"][1]["day"]["maxtemp_c"];
    weather.night2 = root["forecast"]["forecastday"][1]["day"]["mintemp_c"];
    weather.icon2  = apixu_icon(atoi(root["forecast"]["forecastday"][1]["day"]["condition"]["code"]));
    weather.speed2 = root["forecast"]["forecastday"][1]["day"]["maxwind_mph"];
    weather.day3   = root["forecast"]["forecastday"][2]["day"]["maxtemp_c"];
    weather.night3 = root["forecast"]["forecastday"][2]["day"]["mintemp_c"];
    weather.icon3  = apixu_icon(atoi(root["forecast"]["forecastday"][2]["day"]["condition"]["code"]));
    weather.speed3 = root["forecast"]["forecastday"][2]["day"]["maxwind_mph"];
  }
  httpData = "";
  return true;
}

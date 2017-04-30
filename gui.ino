void showSettingsMode(void){
  myGLCD.fillScr(VGA_WHITE);
  myGLCD.setColor(VGA_BLUE);
  myGLCD.setBackColor(VGA_WHITE);
  myGLCD.setFont(SmallFontRu);
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(text_buf,"%02X:%02X:%02X:%02X:%02X:%02X",mac[5],mac[4],mac[3],mac[2],mac[1],mac[0]);
  myGLCD.print(text_buf,10,5);
  String IP=WiFi.localIP().toString();
  myGLCD.print(IP,10,20);
  UTF8(settings_lng[html.lang].str1);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),75);
  UTF8(settings_lng[html.lang].str2);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),90);
  UTF8(settings_lng[html.lang].str3);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),105);
  sprintf(text_buf,"%s \"%s\"",settings_lng[html.lang].str4,rtcData.AP_SSID);
  UTF8(text_buf);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),120);
  sprintf(text_buf,"%s \"%s\".",settings_lng[html.lang].str5,rtcData.AP_PASS);
  UTF8(text_buf);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),135);
  UTF8(settings_lng[html.lang].str6);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),150);
  UTF8(settings_lng[html.lang].str7);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),165);
}

void showWiFiLevel(int myRSSI){
  #define Ant20 0xB6
  #define Ant40 0xB7
  #define Ant60 0xB8
  #define Ant80 0xB9
  #define Ant100 0xBA
  uint8_t ANT=Ant100;
  uint8_t level_wifi=abs(myRSSI);
  if(level_wifi<51) ANT=Ant100;
  if((level_wifi>50)&&(level_wifi<66)) ANT=Ant80;
  if((level_wifi>65)&&(level_wifi<81)) ANT=Ant60;
  if((level_wifi>80)&&(level_wifi<96)) ANT=Ant40;
  if(level_wifi>95) ANT=Ant20;
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.setFont(BigFontRu);
  sprintf(text_buf,"%c",ANT);
  myGLCD.print(text_buf,273,0);
}

void showBatteryLevel(void){
  #define Bat0 0xBB
  #define Bat25 0xBC
  #define Bat50 0xBD
  #define Bat75 0xBE
  #define Bat100 0xBF
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.setFont(BigFontRu);
  char BAT=Bat0;
  int level_battery=analogRead(A0);
  if(level_battery>battery.max){
    battery.max=level_battery;
    save_bat();  
  }
  if(level_battery<battery.min){
    battery.min=level_battery;
    save_bat();
  }
  
  int b1=(battery.max-battery.min);
  int b2=(battery.max+1-level_battery);
  int b3=round(b1/b2);
  if(b3==1) myGLCD.setColor(VGA_RED);
  if(b3==2) BAT=Bat25;
  if(b3==3) BAT=Bat50;
  if(b3==4) BAT=Bat75;
  if(b3>=5) BAT=Bat100;
  sprintf(text_buf,"%c",BAT);
  myGLCD.print(text_buf,296,0);
}

void save_bat(void){
  String save="{\"min\":";
  save+=battery.min;
  save+=",\"max\":";
  save+=battery.max;
  save+="}";
  File file=SPIFFS.open("/bat.json","w");
  if(file){
    file.print(save);
    file.close();
  }
}

void showIcon(uint8_t icon,uint8_t x,uint8_t y/*,uint8_t x1,uint8_t y1*/){
  switch(icon){
    case 1:drawFSJpeg("/01.jpg",x,y);break;
    case 2:drawFSJpeg("/02.jpg",x,y);break;
    case 3:drawFSJpeg("/03.jpg",x,y);break;
    case 4:drawFSJpeg("/04.jpg",x,y);break;
    case 9:drawFSJpeg("/09.jpg",x,y);break;
    case 10:drawFSJpeg("/10.jpg",x,y);break;
    case 11:drawFSJpeg("/11.jpg",x,y);break;
    case 13:drawFSJpeg("/13.jpg",x,y);break;
    case 50:drawFSJpeg("/50.jpg",x,y);break;
    default:;;
  }
}

void printCent(String str,int x1,int x2,uint8_t y,word color,word colorBack,uint8_t* font){
  myGLCD.setFont(font);
  int len=str.length()/2*myGLCD.getFontXsize();
  int cen=x1+round((x2-x1)/2);
  myGLCD.setBackColor(colorBack);
  myGLCD.setColor(colorBack);
  myGLCD.fillRect(x1,y,cen-len,y+myGLCD.getFontYsize()-1);
  myGLCD.fillRect(cen+len+1,y,x2,y+myGLCD.getFontYsize()-1);
  myGLCD.setColor(color);
  myGLCD.print(str,cen-len+1,y);
}

void showWeatherToday(void){ 
  myGLCD.setColor(VGA_WHITE);
  myGLCD.fillRect(1,166,105,239);
    //today
  String str=UTF8(WeatherDaily[html.lang].Today);
  printCent(str,0,105,166,VGA_MAROON,VGA_TRANSPARENT,SmallFontRu);
    //icon
  showIcon(atoi(weather.icon1),0,178/*,57,46*/);
    //temperature day1
  str=dtostrf(weather.day1,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,60,105,185,VGA_PURPLE,VGA_TRANSPARENT,SmallFontRu);
    //temperature night1
  str=dtostrf(weather.night1,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,60,105,201,VGA_BLUE,VGA_TRANSPARENT,SmallFontRu);
    //wind day1
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed1,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  printCent(str,0,105,225,VGA_BLACK,VGA_TRANSPARENT,SmallFontRu);
    //line
  myGLCD.drawLine(106,165,106,239);
}

void showWeatherTomorrow(void){
  myGLCD.setColor(VGA_WHITE);
  myGLCD.fillRect(107,166,212,239);
    //tomorrow
  String str=UTF8(WeatherDaily[html.lang].Tomorrow);
  printCent(str,107,212,166,VGA_MAROON,VGA_WHITE,SmallFontRu);
    //icon
  showIcon(atoi(weather.icon2),107,178/*,57,46*/);
   //temperature day2
  str=dtostrf(weather.day2,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,164,212,185,VGA_PURPLE,VGA_WHITE,SmallFontRu);
    //temperature night2
  str=dtostrf(weather.night2,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,164,212,201,VGA_BLUE,VGA_WHITE,SmallFontRu);
    //wind day2
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed2,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  printCent(str,107,211,225,VGA_BLACK,VGA_WHITE,SmallFontRu);
    //line
  myGLCD.drawLine(213,165,213,239);
}

void showWeatherAfterTomorrow(void){ 
  myGLCD.setColor(VGA_WHITE);
  myGLCD.fillRect(214,166,319,239);
    //after tomorrow
  String str=UTF8(WeatherDaily[html.lang].AfterTomorrow);
  printCent(str,214,319,166,VGA_MAROON,VGA_WHITE,SmallFontRu);
    //icon
  showIcon(atoi(weather.icon3),214,178/*,57,46*/);
    //temperature day3
  str=dtostrf(weather.day3,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,271,319,185,VGA_PURPLE,VGA_WHITE,SmallFontRu);
    //temperature night3
  str=dtostrf(weather.night3,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,271,319,201,VGA_BLUE,VGA_WHITE,SmallFontRu);
    //wind day3
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed3,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  printCent(str,214,319,225,VGA_BLACK,VGA_WHITE,SmallFontRu);
}

void windIcon(bitmapdatatype icon){
  myGLCD.drawBitmap(172,105,15,15,icon,1); 
}

void showWeatherNow(void){
   //clear
  myGLCD.setColor(VGA_WHITE);
  myGLCD.fillRect(0,19,199,164);
   //now
  String str=UTF8(WeatherNow[html.lang].Now);
  printCent(str,1,171,43,VGA_MAROON,VGA_TRANSPARENT,BigFontRu);
  myGLCD.drawLine(0,165,200,165);
   //description
  printCent(UTF8(descript),1,190,62,VGA_BLUE,VGA_TRANSPARENT,SmallFontRu);
   //icon
  showIcon(icon,1,80/*,57,46*/);
   //temperature
  str=dtostrf(weather.temp,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  printCent(str,61,199,85,VGA_PURPLE,VGA_TRANSPARENT,BigFontRu);
   //wind
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  printCent(str,61,171,108,VGA_BLACK,VGA_TRANSPARENT,SmallFontRu);
   //wind direction
  if(((weather.deg>=0) and (weather.deg<23))
  or((weather.deg>337) and (weather.deg<360))) windIcon(n);
  if((weather.deg>22) and (weather.deg<67)) windIcon(n_e);
  if((weather.deg>66) and (weather.deg<112)) windIcon(e);
  if((weather.deg>111) and (weather.deg<157)) windIcon(s_e);
  if((weather.deg>156) and (weather.deg<202)) windIcon(s);
  if((weather.deg>201) and (weather.deg<247)) windIcon(s_w);
  if((weather.deg>246) and (weather.deg<292)) windIcon(w);
  if((weather.deg>291) and (weather.deg<337)) windIcon(n_w);
   //humidity
  str=UTF8(WeatherNow[html.lang].Humidity);
  str+=String(weather.humidity,DEC);
  str+='%';
  printCent(str,1,199,128,VGA_BLACK,VGA_TRANSPARENT,SmallFontRu);
   //pressure
  str=UTF8(WeatherNow[html.lang].Pressure);
  if(html.pres){
    str+=String(round(weather.pressure),DEC);
    str+=UTF8(WeatherNow[html.lang].hpa);
  }
  else{
    str+=String(round(0.75*weather.pressure),DEC);
    str+=UTF8(WeatherNow[html.lang].mm);
  }
  printCent(str,1,199,148,VGA_BLUE,VGA_TRANSPARENT,SmallFontRu);
}

void showInsideTemp(void){
  if(html.sleep==0){
    sensors.requestTemperatures();
    tempInside=sensors.getTempC(insideThermometer);
  }
  myGLCD.setColor(0xE7F9); //224 252 200 e0fcc8
  myGLCD.fillRect(201,61,318,69);
  myGLCD.fillRect(201,70,214,148);
  myGLCD.fillRect(305,70,318,148);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.drawRect(200,60,319,165);
  drawFSJpeg("/home.jpg",215,61);
   
  String str;
  if(html.units){
    str=dtostrf(tempInside*1.8+32,1,1,text_buf);
    str+="^F";
  }
  else{
    str=dtostrf(tempInside,1,1,text_buf);
    str+="^C";
  }
  printCent(str,201,318,149,VGA_GREEN,0xE7F9,BigFontRu);  
}

void showCityName(void){
  myGLCD.setColor(0xFE5F);
  myGLCD.fillRect(1,20,171,39);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.drawRect(0,19,172,40);
  String str=UTF8(cityName);
  if(str.length()>10){
    str[21]='\0';
    printCent(str,1,171,21,VGA_PURPLE,0xFE5F,SmallFontRu);
  }
  else printCent(str,1,171,21,VGA_PURPLE,0xFE5F,BigFontRu);
}

void showTime(void){
  myGLCD.setColor(VGA_BLACK);
  myGLCD.drawRect(172,19,319,60);
   //weekday
  printCent(UTF8(weekday_name(weekday())),173,318,20,0x6180,0xFFF9,SmallFontRu);
   //date
  sprintf(text_buf,"%d %s %d",day(),month_name(month()),year());
  String str=UTF8(text_buf);
  printCent(str,173,318,48,0x6180,0xFFF9,SmallFontRu);
   //time
  if(html.timef) sprintf(text_buf,"%d:%02d",hour(),minute());
  else{
    char* am_pm;
    if(isAM()) am_pm="AM";
    if(isPM()) am_pm="PM";
    sprintf(text_buf,"%d:%02d %s",hourFormat12(),minute(),am_pm);
  }
  printCent(text_buf,173,318,32,0x6180,0xFFF9,BigFontRu);  
}

char* month_name(int m){
  char* mon;
  switch(m){
    case 1:mon=Mon_lng[html.lang].January;break;
    case 2:mon=Mon_lng[html.lang].February;break;
    case 3:mon=Mon_lng[html.lang].March;break;
    case 4:mon=Mon_lng[html.lang].April;break;
    case 5:mon=Mon_lng[html.lang].May;break;
    case 6:mon=Mon_lng[html.lang].June;break;
    case 7:mon=Mon_lng[html.lang].July;break;
    case 8:mon=Mon_lng[html.lang].August;break;
    case 9:mon=Mon_lng[html.lang].September;break;
    case 10:mon=Mon_lng[html.lang].October;break;
    case 11:mon=Mon_lng[html.lang].November;break;
    case 12:mon=Mon_lng[html.lang].December;break;
    default:;;
  }
  return mon;
}

char* weekday_name(int w){
  char* week;
  switch(w){
    case 1:week=Week_lng[html.lang].Sunday;break;
    case 2:week=Week_lng[html.lang].Monday;break;
    case 3:week=Week_lng[html.lang].Tuesday;break;
    case 4:week=Week_lng[html.lang].Wednesday;break;
    case 5:week=Week_lng[html.lang].Thursday;break;
    case 6:week=Week_lng[html.lang].Friday;break;
    case 7:week=Week_lng[html.lang].Saturday;break;
    default:;;
  }
  return week;
}

uint8_t utf8_symb(uint8_t a,uint8_t b){
  uint8_t s=0x3F;
  if(a==0xD0) if(b>0x8F) s=b+48;  //А-Я,а-п
  if(a==0xD1) if(b>0x7F) if(b<0x90) s=b+112;//р-я
  if(a==0xC3){
    if(b==0x82) s=0x82;// Â
    if(b==0x84) s=0x8A;// Ä
    if(b==0x8E) s=0x88;// Î
    if(b==0x96) s=0x8C;// Ö
    if(b==0x9C) s=0x8E;// Ü
    if(b==0x9F) s=0x90;// ß
    if(b==0xA2) s=0x83;// â
    if(b==0xA4) s=0x8B;// ä
    if(b==0xAE) s=0x89;// î
    if(b==0xB6) s=0x8D;// ö
    if(b==0xBC) s=0x8F;// ü
  }
  if(a==0xC4){
    if(b==0x83) s=0x80;// Ă
    if(b==0x83) s=0x81;// ă
  }
  if(a==0xC8){
    if(b==0x98) s=0x84;// Ș
    if(b==0x99) s=0x85;// ș
    if(b==0x9A) s=0x86;// Ț
    if(b==0x9B) s=0x87;// ț
  }
  return s;
}

char* UTF8(char str[80]){
  uint8_t n=0;
  for(uint8_t i=0;i<80;i++){
    if(str[i]==0) break;
    if(str[i]>127){
      text_buf[n]=utf8_symb(str[i],str[i+1]);
      n++;
      i++;
    }
    else text_buf[n++]=str[i];
  }
  text_buf[n]=0;
  return text_buf;
}


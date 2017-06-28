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
  sprintf(text_buf,"%s \"%s\"",settings_lng[html.lang].str4,html.ap_ssid);
  UTF8(text_buf);
  myGLCD.print(text_buf,160-((strlen(text_buf)+1)/2*8),120);
  sprintf(text_buf,"%s \"%s\".",settings_lng[html.lang].str5,html.ap_pass);
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
  if(level_wifi==0) myGLCD.drawBitmap(273,2,16,16,nowifi2,1);
  else{
    if((level_wifi>0)&&(level_wifi<51)) ANT=Ant100;
    if((level_wifi>50)&&(level_wifi<66)) ANT=Ant80;
    if((level_wifi>65)&&(level_wifi<81)) ANT=Ant60;
    if((level_wifi>80)&&(level_wifi<96)) ANT=Ant40;
    if(level_wifi>95) ANT=Ant20;
    myGLCD.setColor(VGA_WHITE);
    myGLCD.setBackColor(back);
    myGLCD.setFont(BigFontRu);
    sprintf(text_buf,"%c",ANT);
    myGLCD.print(text_buf,273,0);
  }
}

void showBatteryLevel(void){
  #define Bat0 0xBB
  #define Bat25 0xBC
  #define Bat50 0xBD
  #define Bat75 0xBE
  #define Bat100 0xBF
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(back);
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
  File file=SPIFFS.open("/save/bat.json","w");
  if(file){
    file.print(save);
    file.close();
  }
}

void showIcon(uint8_t icon,uint8_t x,uint8_t y){
  switch(icon){
    case 1:drawFSJpeg("/pic/01.jpg",x,y);break;
    case 2:drawFSJpeg("/pic/02.jpg",x,y);break;
    case 3:drawFSJpeg("/pic/03.jpg",x,y);break;
    case 4:drawFSJpeg("/pic/04.jpg",x,y);break;
    case 9:drawFSJpeg("/pic/09.jpg",x,y);break;
    case 10:drawFSJpeg("/pic/10.jpg",x,y);break;
    case 11:drawFSJpeg("/pic/11.jpg",x,y);break;
    case 13:drawFSJpeg("/pic/13.jpg",x,y);break;
    case 50:drawFSJpeg("/pic/50.jpg",x,y);break;
    default:;;
  }
}

void showWeather(uint8_t picture,uint8_t x,uint8_t y){
  switch(picture){
    case 1:drawFSJpeg("/pic/101.jpg",x,y);break;
    case 2:drawFSJpeg("/pic/102.jpg",x,y);break;
    case 3:drawFSJpeg("/pic/103.jpg",x,y);break;
    case 4:drawFSJpeg("/pic/104.jpg",x,y);break;
    case 9:drawFSJpeg("/pic/109.jpg",x,y);break;
    case 10:drawFSJpeg("/pic/110.jpg",x,y);break;
    case 11:drawFSJpeg("/pic/111.jpg",x,y);break;
    case 13:drawFSJpeg("/pic/113.jpg",x,y);break;
    case 50:drawFSJpeg("/pic/150.jpg",x,y);break;
    default:;;
  }
}

int textColor(uint8_t picture){
  int color;
  switch(picture){
    case 1:color=VGA_WHITE;break;
    case 2:color=VGA_BLACK;break;
    case 3:color=VGA_WHITE;break;
    case 4:color=VGA_WHITE;break;
    case 9:color=VGA_WHITE;break;
    case 10:color=VGA_WHITE;break;
    case 11:color=VGA_WHITE;break;
    case 13:color=VGA_YELLOW;break;
    case 50:color=VGA_WHITE;break;
    default:;;
  }
  return color;
}

int printCent(String str,int x1,int x2,uint8_t y,word color,word colorBack,uint8_t* font){
  myGLCD.setFont(font);
  int len=str.length()*myGLCD.getFontXsize()/2;
  int cen=round((x2-x1)/2)+x1;
  myGLCD.setBackColor(colorBack);
  myGLCD.setColor(color);
  myGLCD.print(str,cen-len+1,y);
  return cen+len;
}

void printTemp(String temp,int x,uint8_t y,word color){
  char buffer[6];
  temp.toCharArray(buffer,6);
  String integ=strtok(buffer,".");
  String fract=strtok(NULL,".");
  String str;
  myGLCD.setFont(BigFontRu);
  myGLCD.setColor(color);
  myGLCD.setBackColor(VGA_TRANSPARENT);
  if(html.units) str="^F";
  else str="^C";
  myGLCD.print(str,x-(myGLCD.getFontXsize()*2),y);
  myGLCD.print(integ,x-(myGLCD.getFontXsize()*(integ.length()+3)),y);
  int x1=x-(myGLCD.getFontXsize()*2);
  myGLCD.setFont(SmallFontRu);
  str='.';
  str+=fract;
  myGLCD.print(str,x1-(myGLCD.getFontXsize()*2),y+4);
}

void border(int x,int y){
  myGLCD.setColor(0xCE79);
  myGLCD.fillRect(x-3,y-3,x+96+2,y+64+2);
  myGLCD.setColor(0x52AA);
  myGLCD.fillRect(x+2,y+2,x+96+2,y+64+2);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.drawLine(x-1,y-1,x-1,y+63);
  myGLCD.drawLine(x-1,y-1,x+95,y-1);  
}

void showWeatherToday(void){
  int x=219,y=21;
  border(x,y);
  int color=textColor(icon1);
    //icon
  showIcon(icon1,x,y);
    //today
  String str=UTF8(WeatherDaily[html.lang].Today);
  printCent(str,x,x+96,y,color,VGA_TRANSPARENT,SmallFontRu);
    //temperature day1
  str=dtostrf(weather.day1,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+15,color);
    //temperature night1
  str=dtostrf(weather.night1,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+31,color);
    //wind day1
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed1,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  if(rssi==0) str="---";
  printCent(str,x,x+96,y+49,color,VGA_TRANSPARENT,SmallFontRu);
}

void showWeatherTomorrow(void){
  int x=219,y=96;
  border(x,y);
  myGLCD.setColor(0xCE79);
  myGLCD.fillRect(x-3,y-8,x+96+2,y-3);
  int color=textColor(icon2);
    //icon
  showIcon(icon2,x,y);
    //tomorrow
  String str=UTF8(WeatherDaily[html.lang].Tomorrow);
  printCent(str,x,x+96,y,color,VGA_TRANSPARENT,SmallFontRu);
    //temperature day2
  str=dtostrf(weather.day2,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+15,color);
    //temperature night2
  str=dtostrf(weather.night2,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+31,color);
    //wind day2
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed2,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  if(rssi==0) str="---";
  printCent(str,x,x+96,y+49,color,VGA_TRANSPARENT,SmallFontRu);
}

void showWeatherAfterTomorrow(void){
  int x=219,y=171;
  border(x,y);
  myGLCD.setColor(0xCE79);
  myGLCD.fillRect(x-3,y-8,x+96+2,y-3);
  myGLCD.drawLine(x-3,238,319,238);
  myGLCD.drawLine(319,0,319,239);
  int color=textColor(icon3); 
    //icon
  showIcon(icon3,x,y);
    //after tomorrow
  String str=UTF8(WeatherDaily[html.lang].AfterTomorrow);
  printCent(str,x,x+96,y,color,VGA_TRANSPARENT,SmallFontRu);
    //temperature day3
  str=dtostrf(weather.day3,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+15,color);
    //temperature night3
  str=dtostrf(weather.night3,1,1,text_buf);
  if(rssi==0) str="---";
  printTemp(str,x+94,y+31,color);
    //wind day3
  str=UTF8(WeatherNow[html.lang].Wind);
  str+=dtostrf(weather.speed3,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  if(rssi==0) str="---";
  printCent(str,x,x+96,y+49,color,VGA_TRANSPARENT,SmallFontRu);
}

void showWeatherNow(void){
  int x=3,y=92;
  int color=textColor(icon);
  String str;
    //icon
  showWeather(icon,x,y);
    //description
  if(String(UTF8(descript)).length()*16<200){
    printCent(UTF8(descript),x+1,x+207,y+3,color,VGA_TRANSPARENT,BigFontRu);
  } else printCent(UTF8(descript),x+1,x+207,y+10,color,VGA_TRANSPARENT,SmallFontRu);
    //temperature
  int dayLight=0;
  bool out=false;
  if(ntp->getDayLight()) dayLight=3600;
  uint32_t updated=now()-(html.zone*3600)-dayLight-outside.updated;
  if(updated<1800 and outside.temp<120){
    if(html.units) str=outside.temp*1.8+32;
    else str=outside.temp;
    out=true;
  }
  else str=dtostrf(weather.temp,1,1,text_buf);
  if(html.units) str+="^F";
  else str+="^C";
  if(rssi==0) str="---";
  int e=printCent(str,x+1,x+207,y+35,color,VGA_TRANSPARENT,BigFontRu);
  if(out){
    myGLCD.setColor(VGA_BLUE);
    myGLCD.fillCircle(e+12,y+35+6,6);
  }
    //wind
  printCent(UTF8(WeatherNow[html.lang].Wind),x+1,x+207,y+55,color,VGA_TRANSPARENT,SmallFontRu);
  str=dtostrf(weather.speed,1,1,text_buf);
  if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
  else str+=UTF8(WeatherNow[html.lang].meter_sec);
  str+=" ";
  if(rssi==0) str="---";
  int w=printCent(str,x+1,x+207,y+65,color,VGA_TRANSPARENT,BigFontRu);
    //wind direction
  float deg=round(weather.deg/45)*45;
  float x0=w;
  float y0=y+65+8;
  float P=2*PI*deg/360;
  float x1=x0+sin(P)*8;
  float y1=y0-cos(P)*8;
  myGLCD.drawLine(round(x1),round(y1),x0-round(x1)+x0,y0-round(y1)+y0);
  float R=sqrt(sq(x1-x0)+sq(y1-y0));
  float x2=R/2*cos(P)+x0;
  float y2=R/2*sin(P)+y0;
  float x3=2*x0-x2;
  float y3=2*y0-y2;
  geo.fillTriangle(round(x1),round(y1),round(x2),round(y2),round(x3),round(y3));
    //humidity
  printCent(UTF8(WeatherNow[html.lang].Humidity),x+1,x+207,y+85,color,VGA_TRANSPARENT,SmallFontRu);
  if(updated<1800 and outside.humidity<120){
    str=outside.humidity;
    out=true;
  }
  else{
    str=String(weather.humidity,DEC);
    out=false;
  }
  str+='%';
  if(rssi==0) str="---";
  e=printCent(str,x+1,x+207,y+95,color,VGA_TRANSPARENT,BigFontRu);
  if(out){
    myGLCD.setColor(VGA_BLUE);
    myGLCD.fillCircle(e+12,y+95+8,6);
  }
    //pressure
  printCent(UTF8(WeatherNow[html.lang].Pressure),x+1,x+207,y+115,color,VGA_TRANSPARENT,SmallFontRu);
  int pres;
  if(updated<1800 and outside.pres<1200){
    pres=outside.pres;
    out=true;
  }
  else{
    pres=weather.pressure;
    out=false;
  }
  if(html.pres){
    str=String(round(pres),DEC);
    str+=UTF8(WeatherNow[html.lang].hpa);
  }
  else{
    str=String(round(0.75*pres),DEC);
    str+=UTF8(WeatherNow[html.lang].mm);
  }
  if(out) str+=" ";
  if(rssi==0) str="---";
  e=printCent(str,x+1,x+207,y+125,color,VGA_TRANSPARENT,BigFontRu);
  if(out){
    myGLCD.setColor(VGA_BLUE);
    myGLCD.fillCircle(e-4,y+128+6,6);
  }
    //outside battery
  #define Bat0 0xBB
  #define Bat25 0xBC
  #define Bat50 0xBD
  #define Bat75 0xBE
  #define Bat100 0xBF
  char BAT=Bat0;
  if(updated<1800){
    myGLCD.setColor(VGA_WHITE);
    myGLCD.setBackColor(0xCE79);
    myGLCD.setFont(BigFontRu);
    if(outside.bat==1) myGLCD.setColor(VGA_RED);
    if(outside.bat==2) BAT=Bat25;
    if(outside.bat==3) BAT=Bat50;
    if(outside.bat==4) BAT=Bat75;
    if(outside.bat>=5) BAT=Bat100;
    sprintf(text_buf,"%c",BAT);
    myGLCD.print(text_buf,5,0);
  }
  else{
    myGLCD.setColor(0xCE79);
    myGLCD.setBackColor(0xCE79);
    myGLCD.setFont(BigFontRu);
    sprintf(text_buf,"%c",BAT);
    myGLCD.print(text_buf,5,0);
  }
    //
  myGLCD.setColor(0xCE79);
  myGLCD.fillRect(x-3,y-4,x+208+2,y-1);
  myGLCD.fillRect(x+208,y,x+208+2,y+2);
  myGLCD.fillRect(x-3,y-3,x-1,y+144+2);
  myGLCD.fillRect(x,y+144,x+2,y+144+2);
  myGLCD.fillRect(x+208+3,y-3,x+208+5,y+144+2);
  myGLCD.drawLine(0,239,319,239);
  myGLCD.setColor(0x52AA);
  myGLCD.fillRect(x+2,y+144,x+208+2,y+144+2);
  myGLCD.fillRect(x+208,y+2,x+208+2,y+144+2);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.fillRect(x-1,y-1,x-1,y+143);
  myGLCD.fillRect(x-1,y-1,x+207,y-1);
}

void showInsideTemp(void){
  int x=3,y=21;
  int color=VGA_WHITE;
  if(temp_draw==0){
    border(x,y);
    drawFSJpeg("/pic/home.jpg",x,y);
  }
  if(html.sleep==0){
    sensors.requestTemperatures();
    tempInside=sensors.getTempC(insideThermometer);
  }
  if(temp_draw!=tempInside){
    String str;
    if(html.units) str=dtostrf(tempInside*1.8+32,1,1,text_buf);
    else str=dtostrf(tempInside,1,1,text_buf);
    drawFSJpeg("/pic/home.jpg",x,y);
    printTemp(str,x+94,y+27,color);
    temp_draw=tempInside;
  }
  
  //if(html.sleep==0){
    //sensors.requestTemperatures();
    //tempInside=sensors.getTempC(insideThermometer);
    //sensors_event_t event;
    //dht.temperature().getEvent(&event);
    //if(isnan(event.temperature)) ;
    //else tempInside=event.temperature;
  //}
  //myGLCD.setColor(0xE7F9); //224 252 200 e0fcc8
  //myGLCD.fillRect(201,61,318,69);
  //myGLCD.fillRect(201,70,214,148);
  //myGLCD.fillRect(305,70,318,148);
  //myGLCD.setColor(VGA_BLACK);
  //myGLCD.drawRect(200,60,319,165);
  //drawFSJpeg("/pic/home.jpg",215,61);

  //String str;
  //float temp;
  //int color=VGA_GREEN;
  //int dayLight=0;
  //if(ntp->getDayLight()) dayLight=3600;
  //uint32_t updated=now()-(html.zone*3600)-dayLight-outside.updated;
  //if(updated<1800){
  //  temp=outside.temp_in;
  //  color=VGA_BLUE;
  //}
  /*else *///temp=tempInside;
  //if(html.units){
    //str=dtostrf(tempInside*1.8+32,1,1,text_buf);
    //str+="^F";
  //}
  //else{
    //str=dtostrf(tempInside,1,1,text_buf);
    //str+="^C";
  //}
  //printCent(str,201,318,149,VGA_GREEN,0xE7F9,BigFontRu);

  //humidity
  //dht.humidity().getEvent(&event);
  //if(isnan(event.relative_humidity)) ;
  //else weather.humidity=event.relative_humidity;
  
  //str=UTF8(WeatherNow[html.lang].Humidity);
  //if(updated<1800 and outside.humidity<120){
  //  str+=outside.humidity;
  //  color=VGA_BLUE;
  //}
  //else{
    //str+=String(weather.humidity,DEC);
//    color=VGA_BLACK;
  //}
  //str+='%';
  //printCent(str,1,199,128,VGA_BLACK,VGA_WHITE,SmallFontRu);
}

//void showCityName(void){
  //myGLCD.setColor(0xFE5F);
  //myGLCD.fillRect(1,20,199,39);
  //myGLCD.setColor(VGA_BLACK);
  //myGLCD.drawRect(0,19,200,40);
  //String str=UTF8(cityName);
  //if(str.length()>12){
    //str[28]='\0';
    //printCent(str,1,199,21,VGA_PURPLE,0xFE5F,SmallFontRu);
  //}
  //else printCent(str,1,199,21,VGA_PURPLE,0xFE5F,BigFontRu);
//}

int printTm(String str,int x1,int x2,uint8_t y,word color,word colorBack,uint8_t* font){
  myGLCD.setFont(font);
  int len=str.length()*myGLCD.getFontXsize()/2;
  int cen=round((x2-x1)/2)+x1;
  myGLCD.setBackColor(colorBack);
  myGLCD.setColor(color);
  myGLCD.print(str,cen-len+1,y);
  myGLCD.setColor(colorBack);
  myGLCD.fillRect(x1+1,y,cen-len,y+myGLCD.getFontYsize()-1);
  myGLCD.fillRect(cen+len+1,y,x2-1,y+myGLCD.getFontYsize()-1);
  return cen+len;
}

void showTime(void){
  back=0xCE79;
  if(clock_draw==false){
    myGLCD.setColor(0xCE79);
    myGLCD.fillRect(102,0,215,88);
    myGLCD.fillRect(0,0,319,17);
    myGLCD.setColor(VGA_WHITE);
    myGLCD.drawLine(106,85,211,85);
    myGLCD.drawLine(211,17,211,85);
    myGLCD.drawLine(211,16,319,16);
    myGLCD.drawLine(0,16,105,16);
    myGLCD.drawLine(319,0,319,16);
    myGLCD.setColor(0x52AA);
    myGLCD.drawLine(105,17,105,85);
    myGLCD.drawLine(0,0,0,16);
    clock_draw=true;
  }
  if(clock_upd!=minute()){
    clock_upd=minute();
     //weekday
    printTm(UTF8(weekday_name(weekday())),105,211,62,VGA_WHITE,0xCE79,SmallFontRu);
     //date
    sprintf(text_buf,"%d %s %d",day(),month_name(month()),year());
    String str=UTF8(text_buf);
    printTm(str,40,273,0,VGA_WHITE,0xCE79,BigFontRu);
     //time
    char* am_pm;
    int e;
    if(html.timef){
      sprintf(text_buf,"%d:%02d",hour(),minute());
      e=printTm(text_buf,105,211,32,VGA_WHITE,0xCE79,DotMatrix_M_Num);
    }
    else{
      if(isAM()) am_pm="AM";
      if(isPM()) am_pm="PM";
      sprintf(text_buf,"%d:%02d",hourFormat12(),minute());
      e=printTm(text_buf,105,195,32,VGA_WHITE,0xCE79,DotMatrix_M_Num);
      myGLCD.setFont(SmallFontRu);
      myGLCD.setColor(VGA_WHITE);
      myGLCD.print(am_pm,e+4,32);
    }
    myGLCD.setFont(DotMatrix_M_Num);
    myGLCD.setColor(0xCE79);
    myGLCD.fillRect(e-(myGLCD.getFontXsize()*2),30+myGLCD.getFontYsize(),e-(myGLCD.getFontXsize()*3),32+myGLCD.getFontYsize());  
  }
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


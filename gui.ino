void showSettingsMode(void){
  myGLCD.fillScr(back_color);
  myGLCD.setColor(text_color);
  myGLCD.setBackColor(back_color);
  myGLCD.setFont(SmallFontRu);
  myGLCD.print(WiFi.macAddress(),10,5);
  myGLCD.print(WiFi.localIP().toString(),10,20);
  printCent(UTF8(settings_lng[html.lang].str1),0,319,75,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[html.lang].str2),0,319,90,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[html.lang].str3),0,319,105,text_color,back_color,SmallFontRu);
  sprintf(text_buf,"%s \"%s\"",settings_lng[html.lang].str4,html.ap_ssid);
  printCent(UTF8(text_buf),0,319,120,text_color,back_color,SmallFontRu);
  sprintf(text_buf,"%s \"%s\".",settings_lng[html.lang].str5,html.ap_pass);
  printCent(UTF8(text_buf),0,319,135,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[html.lang].str6),0,319,150,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[html.lang].str7),0,319,165,text_color,back_color,SmallFontRu);
}

void showWiFiLevel(int myRSSI){
  uint8_t ant=ANT100;
  uint8_t level_wifi=abs(myRSSI);
  if(level_wifi==0) myGLCD.drawBitmap(273,0,16,16,nowifi,1);
  else{
    if((level_wifi>0)&&(level_wifi<51)) ant=ANT100;
    if((level_wifi>50)&&(level_wifi<66)) ant=ANT80;
    if((level_wifi>65)&&(level_wifi<81)) ant=ANT60;
    if((level_wifi>80)&&(level_wifi<96)) ant=ANT40;
    if(level_wifi>95) ant=ANT20;
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(BigFontRu);
    sprintf(text_buf,"%c",ant);
    myGLCD.print(text_buf,273,0);
  }
}

void showBatteryLevel(void){
  myGLCD.setColor(text_color);
  myGLCD.setBackColor(back_color);
  myGLCD.setFont(BigFontRu);
  char bat=BAT0;
  int adc=analogRead(A0);
  float Ubat=(float)adc/(float)html.k;
  if(Ubat<3.3){analogWrite(BACKLIGHT,0);myGLCD.lcdOff();ESP.deepSleep(999999999*999999999U,WAKE_NO_RFCAL);}
  if(Ubat>=3.3 and Ubat<3.5) myGLCD.setColor(VGA_RED);
  if(Ubat>=3.5 and Ubat<3.7) bat=BAT25;
  if(Ubat>=3.7 and Ubat<3.8) bat=BAT50;
  if(Ubat>=3.8 and Ubat<3.9) bat=BAT75;
  if(Ubat>=3.9) bat=BAT100;
  sprintf(text_buf,"%c",bat);
  myGLCD.print(text_buf,296,0);
}

void weatherIcon(uint8_t picture,bool isDay,uint8_t x,uint8_t y){
  if(isDay){
    switch(picture){
      case 1:drawFSJpeg("/pic/01d.jpg",x,y);break;
      case 2:drawFSJpeg("/pic/02d.jpg",x,y);break;
      case 3:drawFSJpeg("/pic/03d.jpg",x,y);break;
      case 4:drawFSJpeg("/pic/04d.jpg",x,y);break;
      case 9:drawFSJpeg("/pic/09d.jpg",x,y);break;
      case 10:drawFSJpeg("/pic/10d.jpg",x,y);break;
      case 11:drawFSJpeg("/pic/11d.jpg",x,y);break;
      case 13:drawFSJpeg("/pic/13d.jpg",x,y);break;
      case 50:drawFSJpeg("/pic/50d.jpg",x,y);break;
      default:;;
    }
  }
  else{
    switch(picture){
      case 1:drawFSJpeg("/pic/01n.jpg",x,y);break;
      case 2:drawFSJpeg("/pic/02n.jpg",x,y);break;
      case 3:drawFSJpeg("/pic/03n.jpg",x,y);break;
      case 4:drawFSJpeg("/pic/04d.jpg",x,y);break;
      case 9:drawFSJpeg("/pic/09d.jpg",x,y);break;
      case 10:drawFSJpeg("/pic/10d.jpg",x,y);break;
      case 11:drawFSJpeg("/pic/11n.jpg",x,y);break;
      case 13:drawFSJpeg("/pic/13d.jpg",x,y);break;
      case 50:drawFSJpeg("/pic/50d.jpg",x,y);break;
      default:;;
    }  
  }
}

void printCent(String str,int x1,int x2,uint8_t y,int color,int colorBack,uint8_t* font){
  myGLCD.setFont(font);
  int len=str.length()*myGLCD.getFontXsize()/2;
  int cen=round((x2-x1)/2)+x1;
  myGLCD.setBackColor(colorBack);
  myGLCD.setColor(color);
  myGLCD.print(str,cen-len+1,y);
  myGLCD.setColor(back_color);
  myGLCD.fillRect(x1,y,cen-len,y+myGLCD.getFontYsize());
  myGLCD.fillRect(cen+len,y,x2,y+myGLCD.getFontYsize());
}

void printData(String data,String units,int x,uint8_t y,int color,int bcolor){
  char buf[16];
  data.toCharArray(buf,16);
  String integ=strtok(buf,".");
  String fract=strtok(NULL,".");
  int len=(integ.length()+3)*16/2;
  myGLCD.setFont(BigFontRu);
  myGLCD.setColor(color);
  myGLCD.setBackColor(bcolor);
  myGLCD.print(integ,x+48-len,y);
  myGLCD.setFont(SmallFontRu);
  myGLCD.print(".",x+48-len+integ.length()*16,y+4);
  sprintf(text_buf,"%c",fract[0]);
  myGLCD.print(text_buf,x+48-len+integ.length()*16+8,y+4);
  myGLCD.print(units,x+48-len+integ.length()*16+16,y+4);
}

void printInt(String data,String units,int x,uint8_t y,word color){
  int len=(data.length()*16/2)+(units.length()*8/2);
  myGLCD.setFont(BigFontRu);
  myGLCD.setColor(color);
  myGLCD.setBackColor(VGA_TRANSPARENT);
  myGLCD.print(data,x+48-len,y);
  myGLCD.setFont(SmallFontRu);
  myGLCD.print(units,x+48-len+data.length()*16,y+4);
}

void printWD(String str,int x,int y){
  myGLCD.setFont(BigFontRu);
  myGLCD.setBackColor(VGA_TRANSPARENT);
  myGLCD.setColor(text_color);
  str.length()==2?myGLCD.print(str,x+60,y+3):myGLCD.print(str,x+76,y+3);
}

void showWeatherDaily(int x,int y,uint8_t icon,uint8_t wd,float tempDay,float tempNight,float wind){
  myGLCD.setColor(back_color);
  myGLCD.fillRect(x-7,y,x+104,myGLCD.getDisplayYSize()-1);
  if(rssi!=0) weatherIcon(icon,true,x,y);
  myGLCD.fillRect(x+80,y+48,x+81,y+49);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.drawRect(x,y,x+96,myGLCD.getDisplayYSize()-2);
  myGLCD.drawRect(x+1,y+1,x+95,myGLCD.getDisplayYSize()-3);
  if(wd>6) wd-=7;
  printWD(UTF8(WD[wd][html.lang]),x,y);
  if(rssi==0){
    printData("---",html.units?"^F":"^C",x+16,y+18,text_color,VGA_TRANSPARENT);
    printData("---",html.units?"^F":"^C",x+16,y+36,text_color,VGA_TRANSPARENT);
    printCent("---",x,x+96,y+52,text_color,VGA_TRANSPARENT,SmallFontRu);
  }else{
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(VGA_TRANSPARENT);
    myGLCD.setFont(BigFontRu);
    myGLCD.print(String(round(tempDay)),(x+75)-String(round(tempDay)).length()*16,y+18);
    myGLCD.print(String(round(tempNight)),(x+75)-String(round(tempNight)).length()*16,y+36);
    myGLCD.setFont(SmallFontRu);
    myGLCD.print(html.units?"^F":"^C",x+75,y+22);
    myGLCD.print(html.units?"^F":"^C",x+75,y+40);
    String str=UTF8(WeatherNow[html.lang].Wind);
    str+=String(round(wind));
    if(html.units) str+=UTF8(WeatherNow[html.lang].miles_hour);
    else str+=UTF8(WeatherNow[html.lang].meter_sec);
    printCent(str,x+2,x+94,y+52,text_color,VGA_TRANSPARENT,SmallFontRu);
  }  
}

void showWeatherToday(void){
  showWeatherDaily(7,171,icon1,weekday()-1,weather.day1,weather.night1,weather.speed1);
}

void showWeatherTomorrow(void){
  showWeatherDaily(111,171,icon2,weekday(),weather.day2,weather.night2,weather.speed2);
}

void showWeatherAfterTomorrow(void){
  showWeatherDaily(215,171,icon3,weekday()+1,weather.day3,weather.night3,weather.speed3);
}

void showWeatherNow(void){
  int x=3,y=18; String str; int dayLight=0; bool out=false;
  if(summertime()) dayLight=3600;
  uint32_t updated=now()-(html.zone*3600)-dayLight-outside.updated;
  myGLCD.setColor(back_color);
  myGLCD.fillRect(x-3,y-1,x+211,y+152); 
  
    //icon
  weatherIcon(icon,weather.isDay,x,y);
  myGLCD.fillRect(x+80,y+48,x+81,y+49);
  
    //temperature
  float temp=0;
  drawFSJpeg(weather.temp<0?"/pic/temp-.jpg":"/pic/temp+.jpg",x+5,y+64);
  if(updated<1800 and outside.temp<120){
    html.units?temp=outside.temp*1.8+32:temp=outside.temp;
    out=true;
  }
  else{
    temp=weather.temp;
    out=false;
  }
  if(rssi==0) str="---";
  printData(String(temp),html.units?"^F":"^C",x+35,y+80,out?out_color:text_color,back_color);
  
    //wind
  int e=0;
  drawFSJpeg("/pic/wind.jpg",x+5,y+104);
  myGLCD.setColor(back_color);
  myGLCD.fillRect(x+21,y+120,x+22,y+121);
  str=dtostrf(weather.speed,1,1,text_buf);
  if(rssi==0) str="---";
  printData(str,html.units?UTF8(WeatherNow[html.lang].miles_hour):UTF8(WeatherNow[html.lang].meter_sec),x+27,y+120,text_color,back_color);
  
    //wind direction
  float deg=round(weather.deg/45)*45+180;
  float x0=x+21;
  float y0=y+121+8;
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
  uint8_t humidity;
  drawFSJpeg("/pic/hum.jpg",x+108,y+64);
  if(updated<1800 and outside.humidity<120){
    humidity=outside.humidity;
    out=true;
  }
  else{
    humidity=weather.humidity;
    out=false;
  }
  if(rssi==0) str="---";
  printInt(String(humidity),"%",x+122,y+80,out?out_color:text_color);
    
    //pressure
  drawFSJpeg("/pic/pres.jpg",x+108,y+104);
  int pres;
  if(updated<1800 and outside.pres<1200){
    pres=outside.pres;
    out=true;
  }
  else{
    pres=weather.pressure;
    out=false;
  }
  html.pres?str=String(round(pres),DEC):str=String(round(0.75*pres),DEC);
  if(rssi==0) str="---";
  if(html.pres)
    printCent(UTF8(WeatherNow[html.lang].hpa),x+142,x+210,y+108,out?out_color:text_color,back_color,SmallFontRu);
  if(rssi==0) str="---";
  printInt(str,html.pres?"":UTF8(WeatherNow[html.lang].mm),x+124,y+120,out?out_color:text_color);
  
    //dew point
  str=UTF8(WeatherNow[html.lang].dew);
  str+=String(round(dew(temp,humidity,html.units)));
  str+=html.units?"^F":"^C";
  printCent(str,x,x+208,y+138,VGA_AQUA,back_color,SmallFontRu);
  
    //outside battery
  char bat=BAT0;
  if(updated<1800){
    myGLCD.setColor(out_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(BigFontRu);
    if(outside.bat==1) myGLCD.setColor(VGA_RED);
    if(outside.bat==2) bat=BAT25;
    if(outside.bat==3) bat=BAT50;
    if(outside.bat==4) bat=BAT75;
    if(outside.bat>=5) bat=BAT100;
    sprintf(text_buf,"%c",bat);
    myGLCD.print(text_buf,5,0);
  }
  else{
    myGLCD.setColor(back_color);
    myGLCD.fillRect(5,0,21,16);
  }

    //description
  if(html.lang==1 or html.lang==5){
    String d=description(weather.id); 
    if(d) d.toCharArray(descript,d.length()+1);
  }
  char buf[160]; uint8_t c=0,maxLen=0,len[5]={0,0,0,0,0};
  String(UTF8(descript)).toCharArray(buf,String(UTF8(descript)).length()+1);
  String w1=strtok(buf," ");
  String w2=strtok(NULL," ");
  String w3=strtok(NULL," ");
  String w4=strtok(NULL," ");
  String w5=strtok(NULL," ");
  if(w1.length()>0){c++;len[0]=w1.length();}
  if(w2.length()>0){c++;len[1]=w2.length();}
  if(w3.length()>0){c++;len[2]=w3.length();}
  if(w4.length()>0){c++;len[3]=w4.length();}
  if(w5.length()>0){c++;len[4]=w5.length();}
  maxLen=len[0];
  for(int i=0;i<4;i++) if(len[i]>maxLen) maxLen=len[i];
  if(maxLen*16<113 and c<4){
    uint8_t y1=(y+32)-(c*16)/2;
    if(c>0) printCent(w1,x+90,x+203,y1,text_color,VGA_TRANSPARENT,BigFontRu);
    if(c>1) printCent(w2,x+90,x+203,y1+16,text_color,VGA_TRANSPARENT,BigFontRu);
    if(c>2) printCent(w3,x+90,x+203,y1+32,text_color,VGA_TRANSPARENT,BigFontRu);
  }
  else{
    uint8_t y1=(y+32)-(c*12)/2;
    if(c>0) printCent(w1,x+90,x+203,y1,text_color,VGA_TRANSPARENT,SmallFontRu);
    if(c>1) printCent(w2,x+90,x+203,y1+12,text_color,VGA_TRANSPARENT,SmallFontRu);
    if(c>2) printCent(w3,x+90,x+203,y1+24,text_color,VGA_TRANSPARENT,SmallFontRu);
    if(c>3) printCent(w4,x+90,x+203,y1+36,text_color,VGA_TRANSPARENT,SmallFontRu);
    if(c>4) printCent(w5,x+90,x+203,y1+48,text_color,VGA_TRANSPARENT,SmallFontRu);
  }  
}

void showInsideTemp(void){
  int x=215,y=96;
  if(temp_draw==0){
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x,y,myGLCD.getDisplayXSize()-1,y+74);
    myGLCD.setColor(VGA_WHITE);
    myGLCD.drawRect(x,y,x+96,y+64);
    myGLCD.drawRect(x+1,y+1,x+95,y+63);
    drawFSJpeg("/pic/home.jpg",x+3,y+16);
  }
  if(html.sleep==0 or html.sleep>3){
    sensors.requestTemperatures();
    tempInside=sensors.getTempC(insideThermometer);
  }
  if(temp_draw!=tempInside){
    String str=dtostrf(html.units?tempInside*1.8+32:tempInside,1,1,text_buf);
    printData(str,html.units?"^F":"^C",x+22,y+25,text_color,back_color);
    temp_draw=tempInside;
  }
}

void showTime(void){
  int x=215,y=22;
  if(clock_draw==false){
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x,y-4,myGLCD.getDisplayXSize()-1,y+73);
    myGLCD.fillRect(0,0,myGLCD.getDisplayXSize()-1,17);
    myGLCD.setColor(VGA_WHITE);
    myGLCD.drawRect(x,y,x+96,y+64);
    myGLCD.drawRect(x+1,y+1,x+95,y+63);
    clock_draw=true;
  }
  if(clock_upd!=minute()){
    clock_upd=minute();
     //weekday
    printCent(UTF8(Weekday[weekday()-1][html.lang]),x+2,x+94,y+45,text_color,back_color,SmallFontRu);
     //date
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(BigFontRu);
    uint8_t ld=(String(day()).length())*16;
    uint8_t lm=(String(UTF8(Month[month()-1][html.lang])).length())*8;
    myGLCD.print(String(day()),160-(ld+lm+64)/2,0);
    myGLCD.setFont(SmallFontRu);
    myGLCD.print(UTF8(Month[month()-1][html.lang]),160-(ld+lm+64)/2+ld,4);
    myGLCD.setFont(BigFontRu);
    myGLCD.print(String(year()),160-(ld+lm+64)/2+ld+lm,0);
    myGLCD.setColor(back_color);
    myGLCD.fillRect(30,0,159-(ld+lm+64)/2,17);
    myGLCD.fillRect(160-(ld+lm)/2+ld+lm+64,0,272,17);
    myGLCD.fillRect(160-(ld+lm+64)/2+ld,0,160-(ld+lm+64)/2+ld+lm,3);
     //time
    sprintf(text_buf,"%d:%02d",html.timef?hour():hourFormat12(),minute());
    printCent(text_buf,x+7,x+89,y+18,text_color,back_color,DotMatrix_M_Num);
    if(!html.timef) 
      printCent(isAM()?UTF8(WeatherNow[html.lang].AM):UTF8(WeatherNow[html.lang].PM),x+7,x+89,y+2,text_color,back_color,SmallFontRu);
  }
}

float dew(float temp,float hum,bool metric){
  float dewPoint=NAN;
  if(metric&&!isnan(temp>=0)&&!isnan(hum)){
    dewPoint=243.12*(log(hum/100.0)+((17.62*temp)/(243.12+temp)))/(17.62-log(hum/100.0)-((17.62*temp)/(243.12+temp)));
  }
  else if(metric&&!isnan(temp<0)&&!isnan(hum)){
    dewPoint=276.62*(log(hum/100.0)+((22.46*temp)/(276.62+temp)))/(22.46-log(hum/100.0)-((22.46*temp)/(276.62+temp)));
  }
  else if(!isnan(temp>=0)&&!isnan(hum)){
    float ctemp=(temp-32.0)* 5.0/9.0;
    dewPoint=243.12*(log(hum/100.0)+((17.62*ctemp)/(243.12+ctemp)))/(17.62-log(hum/100.0)-((17.62*ctemp)/(243.12+ctemp)));
    dewPoint=dewPoint*9.0/5.0+32.0;
  }
  else if(!isnan(temp<0)&&!isnan(hum)){
    float ctemp=(temp-32.0)*5.0/9.0;
    dewPoint=276.62*(log(hum/100.0)+((22.46*ctemp)/(276.62+ctemp)))/(22.46-log(hum/100.0)-((22.46*ctemp)/(276.62+ctemp)));
    dewPoint=dewPoint*9.0/5.0+32.0;
  }
  return dewPoint;
}


uint8_t utf8_symb(uint8_t a,uint8_t b){
  uint8_t s=0x3F;
  if(a==0xD0){
    if(b>0x8F) if(b<0xC0) s=b+48;  //А-Я,а-п
    if(b==0x84) s=0xA7;// Є
    if(b==0x86) s=0x49;// I
    if(b==0x87) s=0xA9;// Ї
    if(b==0x81) s=0xAB;// Ё 
  }
  if(a==0xD1){
    if(b>0x7F) if(b<0x90) s=b+112;//р-я
    if(b==0x94) s=0xA8;// є
    if(b==0x96) s=0x69;// i
    if(b==0x97) s=0xAA;// ї
    if(b==0x91) s=0xAC;// ё 
  }
  if(a==0xD2){
    if(b==0x90) s=0xA5;// Ґ
    if(b==0x91) s=0xA6;// ґ
  }
  if(a==0xC3){
    if(b==0x82) s=0x82;// Â
    if(b==0x84) s=0x8A;// Ä
    if(b==0x8E) s=0x88;// Î
    if(b==0x95) s=0xA3;// Õ
    if(b==0x96) s=0x8C;// Ö
    if(b==0x9C) s=0x8E;// Ü
    if(b==0x9F) s=0x90;// ß
    if(b==0xA2) s=0x83;// â
    if(b==0xA4) s=0x8B;// ä
    if(b==0xAE) s=0x89;// î
    if(b==0xB5) s=0xA4;// õ
    if(b==0xB6) s=0x8D;// ö
    if(b==0xBC) s=0x8F;// ü
  }
  if(a==0xC4){
    if(b==0x82) s=0x80;// Ă
    if(b==0x83) s=0x81;// ă
    if(b==0x84) s=0x91;// Ą
    if(b==0x85) s=0x92;// ą
    if(b==0x8C) s=0x93;// Č
    if(b==0x8D) s=0x94;// č
    if(b==0x98) s=0x95;// Ę
    if(b==0x96) s=0x96;// Ė
    if(b==0x99) s=0x97;// ę
    if(b==0x97) s=0x98;// ė
    if(b==0xAE) s=0x99;// Į
    if(b==0xAF) s=0x9A;// į  
  }
  if(a==0xC5){
    if(b==0xA0) s=0x9B;// Š
    if(b==0xA1) s=0x9C;// š
    if(b==0xB2) s=0x9D;// Ų
    if(b==0xAA) s=0x9E;// Ū
    if(b==0xB3) s=0x9F;// ų
    if(b==0xAB) s=0xA0;// ū
    if(b==0xBD) s=0xA1;// Ž
    if(b==0xBE) s=0xA2;// ž
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

String description(int code){
  String descr=""; File file;
  if(code>=200 and code<300){
    if(html.lang==1) file=SPIFFS.open("/lang/ru200.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et200.json","r");
  }
  if(code>=300 and code<400){
    if(html.lang==1) file=SPIFFS.open("/lang/ru300.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et300.json","r");
  }
  if(code>=500 and code<600){
    if(html.lang==1) file=SPIFFS.open("/lang/ru500.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et500.json","r");
  }
  if(code>=600 and code<700){
    if(html.lang==1) file=SPIFFS.open("/lang/ru600.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et600.json","r");
  }
  if(code>=700 and code<800){
    if(html.lang==1) file=SPIFFS.open("/lang/ru700.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et700.json","r");
  }
  if(code>=800 and code<900){
    if(html.lang==1) file=SPIFFS.open("/lang/ru800.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et800.json","r");
  }
  if(code>=900 and code<1000){
    if(html.lang==1) file=SPIFFS.open("/lang/ru900.json","r");
    if(html.lang==5) file=SPIFFS.open("/lang/et900.json","r");
  }
  if(file){
    String fileData=file.readString();
    file.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root=jsonBuffer.parseObject(fileData);
    if(root.success()){
      String descript=root[String(code)];
      descr=descript;
    }
  }
  return descr;
}

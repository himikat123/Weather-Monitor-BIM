void showSettingsMode(void){
  myGLCD.fillScr(back_color);
  printCent(WiFi.macAddress(),0,200,5,text_color,back_color,SmallFontRu);
  printCent(WiFi.localIP().toString(),0,150,20,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[config.lang].str1),0,319,75,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[config.lang].str2),0,319,90,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[config.lang].str3),0,319,105,text_color,back_color,SmallFontRu);
  sprintf(text_buf,"%s \"%s\"",settings_lng[config.lang].str4,config.ap_ssid);
  printCent(UTF8(text_buf),0,319,120,text_color,back_color,SmallFontRu);
  sprintf(text_buf,"%s \"%s\".",settings_lng[config.lang].str5,config.ap_pass);
  printCent(UTF8(text_buf),0,319,135,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[config.lang].str6),0,319,150,text_color,back_color,SmallFontRu);
  printCent(UTF8(settings_lng[config.lang].str7),0,319,165,text_color,back_color,SmallFontRu);
}

void showWiFiLevel(int myRSSI){
  uint8_t ant=ANT100;
  uint8_t level_wifi=abs(myRSSI);
  myGLCD.setColor(text_color);
  myGLCD.setBackColor(back_color);
  myGLCD.setFont(Symbols);
  if(level_wifi==0){
    sprintf(text_buf,"%c",ANT100);
    if(config.ac){
      myGLCD.print(text_buf,290,0);
      myGLCD.setColor(VGA_RED);
      myGLCD.drawLine(290,0,305,15);
      myGLCD.drawLine(290,15,305,0);
    }
    else{
      if(config.battery==0){
        myGLCD.print(text_buf,273,0);
        myGLCD.setColor(VGA_RED);
        myGLCD.drawLine(273,0,288,15);
        myGLCD.drawLine(273,15,288,0);
      }
      if(config.battery==1){
        myGLCD.print(text_buf,265,0);
        myGLCD.setColor(VGA_RED);
        myGLCD.drawLine(265,0,280,15);
        myGLCD.drawLine(265,15,280,0);
      }
    }
  }
  else{
    if((level_wifi>0)&&(level_wifi<51)) ant=ANT100;
    if((level_wifi>50)&&(level_wifi<66)) ant=ANT80;
    if((level_wifi>65)&&(level_wifi<81)) ant=ANT60;
    if((level_wifi>80)&&(level_wifi<96)) ant=ANT40;
    if(level_wifi>95) ant=ANT20;
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(Symbols);
    sprintf(text_buf,"%c",ant);
    if(config.ac) myGLCD.print(text_buf,290,0);
    else{
      if(config.battery==0) myGLCD.print(text_buf,273,0);
      if(config.battery==1) myGLCD.print(text_buf,265,0);
    }
  }
}

void showBatteryLevel(void){
  if(!config.ac){
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(back_color);
    if(config.battery==0) myGLCD.setFont(Symbols);
    if(config.battery==1) myGLCD.setFont(SmallFontRu);
    char bat=BAT0;
    float adc=analogRead(A0);
    float cor=-config.k;
    cor=cor+400;
    float Ubat=adc/cor;
    if(Ubat<3.3){analogWrite(BACKLIGHT,0);myGLCD.lcdOff();ESP.deepSleep(999999999*999999999U,WAKE_NO_RFCAL);}
    if(Ubat>=3.3 and Ubat<3.5) myGLCD.setColor(VGA_RED);
    if(Ubat>=3.5 and Ubat<3.7) bat=BAT25;
    if(Ubat>=3.7 and Ubat<3.8) bat=BAT50;
    if(Ubat>=3.8 and Ubat<3.9) bat=BAT75;
    if(Ubat>=3.9) bat=BAT100;
    if(config.battery==0) myGLCD.print(String(bat),296,0);
    if(config.battery==1) myGLCD.print(String(Ubat),286,3);
  }
}

void weatherIcon(uint8_t picture,bool isDay,uint8_t x,uint8_t y){
  switch(picture){
    case 1:drawFSJpeg(isDay?"/pic/01d.jpg":"/pic/01n.jpg",x,y);break;
    case 2:drawFSJpeg(isDay?"/pic/02d.jpg":"/pic/02n.jpg",x,y);break;
    case 3:drawFSJpeg(isDay?"/pic/03d.jpg":"/pic/03n.jpg",x,y);break;
    case 4:drawFSJpeg("/pic/04d.jpg",x,y);break;
    case 9:drawFSJpeg("/pic/09d.jpg",x,y);break;
    case 10:drawFSJpeg("/pic/10d.jpg",x,y);break;
    case 11:drawFSJpeg(isDay?"/pic/11d.jpg":"/pic/11n.jpg",x,y);break;
    case 13:drawFSJpeg("/pic/13d.jpg",x,y);break;
    case 50:drawFSJpeg("/pic/50d.jpg",x,y);break;
    default:;;
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
  sprintf(text_buf,"%d",round(fract.toInt()/10));
  myGLCD.print(text_buf,x+48-len+integ.length()*16+8,y+4);
  myGLCD.print(units,x+48-len+integ.length()*16+16,y+4);
}

void printInt(String data,String units,int x,uint8_t y,word color,word bcolor){
  int len=(data.length()*16/2)+(units.length()*8/2);
  myGLCD.setFont(BigFontRu);
  myGLCD.setColor(color);
  myGLCD.setBackColor(bcolor);
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
    //icon
  myGLCD.setColor(back_color);
  myGLCD.fillRect(x-7,y,x+104,myGLCD.getDisplayYSize()-1);
  weatherIcon(icon,true,x+5,y);
    //day
  myGLCD.fillRect(x+80,y+48,x+81,y+49);
  myGLCD.setColor(rama_color);
  myGLCD.drawRect(x,y,x+96,myGLCD.getDisplayYSize()-2);
  myGLCD.drawRect(x+1,y+1,x+95,myGLCD.getDisplayYSize()-3);
  if(wd>6) wd-=7;
  myGLCD.setColor(text_color);
  printWD(UTF8(WD[wd][config.lang]),x,y);
    //temperature and wind
  myGLCD.setBackColor(VGA_TRANSPARENT);
  myGLCD.setFont(BigFontRu);
  String s=tempDay>200?"---":String(round(config.to_units?tempDay*1.8+32:tempDay));
  myGLCD.print(s,(x+75)-s.length()*16,y+18);
  s=tempNight>200?"---":String(round(config.to_units?tempNight*1.8+32:tempNight));
  myGLCD.print(s,(x+75)-s.length()*16,y+36);
  myGLCD.setFont(SmallFontRu);
  myGLCD.print(config.to_units?"^F":"^C",x+75,y+22);
  myGLCD.print(config.to_units?"^F":"^C",x+75,y+40);
  if(config.provider!=2){
    String str=UTF8(WeatherNow[config.lang].Wind); 
    if(config.w_units==0) str+=wind>200?"--":String(round(wind));
    if(config.w_units==1) str+=wind>200?"--":String(round(wind*3.6));
    if(config.w_units==2) str+=wind>200?"--":String(round(wind*2.237));
    if(config.w_units==3) str+=wind>200?"--":String(round(wind*1.94));
    if(config.w_units==0) str+=UTF8(WeatherNow[config.lang].meter_sec);
    if(config.w_units==1) str+=UTF8(WeatherNow[config.lang].km_hour);
    if(config.w_units==2) str+=UTF8(WeatherNow[config.lang].miles_hour);
    if(config.w_units==3) str+=UTF8(WeatherNow[config.lang].knots);
    printCent(str,x+2,x+94,y+52,text_color,VGA_TRANSPARENT,SmallFontRu);
  }  
}

void showWeatherToday(void){
  showWeatherDaily(7,171,weather.icon1,weekday()-1,weather.day1,weather.night1,weather.speed1);
}

void showWeatherTomorrow(void){
  showWeatherDaily(111,171,weather.icon2,weekday(),weather.day2,weather.night2,weather.speed2);
}

void showWeatherAfterTomorrow(void){
  showWeatherDaily(215,171,weather.icon3,weekday()+1,weather.day3,weather.night3,weather.speed3);
}

void showWeatherNow(bool l_upd){
  int x=3,y=18; String str; int dayLight=0; bool out=false;
  if(config.adj) if(summertime()) dayLight=3600;
  uint32_t updated=now()-(config.zone*3600)-dayLight-outside.updated;
  if(l_upd){
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x-3,y-1,x+211,y+152); 
      //icon
    weatherIcon(weather.icon,weather.isDay,x,y);
    myGLCD.fillRect(x+80,y+48,x+81,y+49);
  }
    //temperature
  float temp=get_temp_out();
  if(updated<1800 and temp<120 and (config.t_out==1 or config.t_out==2 or config.t_out==3)) out=true;
  else if(config.t_out>=4 and config.t_out<=9) out=true;
  else{
    temp=weather.temp;
    out=false;
  }
  drawFSJpeg((temp<0.0)?"/pic/temp-.jpg":"/pic/temp+.jpg",x+2,y+64);
  int cen=round(((x+112)-(x+32))/2)+x+32;
  if(config.to_round){
    char buf[16];
    String(temp).toCharArray(buf,16);
    String integ=strtok(buf,".");
    String fract=strtok(NULL,".");
    float fr=fract.toFloat()/10;
    fract="."+String(round(fr));
    myGLCD.setFont(Arial_round);
    int leng=(integ.length()*myGLCD.getFontXsize()+42);
    myGLCD.setColor(out?out_color:text_color);
    myGLCD.setBackColor(back_color);
    if(temp<120.0) myGLCD.print(integ,cen-(leng/2),y+75);
    else myGLCD.print("--",x+30,y+75);
    myGLCD.setFont(SmallFontRu);
    if(temp<120.0) myGLCD.print(fract,cen-(leng/2)+1+(leng-42),y+83);
    myGLCD.setFont(BigFontRu);
    myGLCD.print(config.to_units?"^F":"^C",cen-(leng/2)+(leng-42)+14,y+80);
  }
  else{
    str=String(round(temp));
    myGLCD.setFont(Arial_round);
    int leng=(str.length()*myGLCD.getFontXsize()+32)/2;
    myGLCD.setColor(out?out_color:text_color);
    myGLCD.setBackColor(back_color);
    if(temp<120.0) myGLCD.print(str,cen-leng,y+75);
    else myGLCD.print("--",x+30,y+75);
    myGLCD.setFont(BigFontRu);
    myGLCD.print(config.to_units?"^F":"^C",cen-leng+1+str.length()*myGLCD.getFontXsize(),y+80); 
  }
  if(l_upd){
      //wind
    int e=0; String units; float w;
    drawFSJpeg("/pic/wind.jpg",x+5,y+104);
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x+21,y+120,x+22,y+121);
    if(config.w_units==0) w=config.w_round?weather.speed:round(weather.speed);
    if(config.w_units==1) w=config.w_round?(weather.speed*3.6):round(weather.speed*3.6);
    if(config.w_units==2) w=config.w_round?(weather.speed*2.237):round(weather.speed*2.237);
    if(config.w_units==3) w=config.w_round?(weather.speed*1.94):round(weather.speed*1.94);
    if(config.w_units==0) units=UTF8(WeatherNow[config.lang].meter_sec);
    if(config.w_units==1) units=UTF8(WeatherNow[config.lang].km_hour);
    if(config.w_units==2) units=UTF8(WeatherNow[config.lang].miles_hour);
    if(config.w_units==3) units=UTF8(WeatherNow[config.lang].knots);
    if(w>400) printInt("--",units,x+27,y+120,text_color,back_color);
    else{
      myGLCD.setFont(BigFontRu);
      if(config.w_round) printData(String(w),units,x+27,y+120,text_color,back_color);
      else printInt(String(round(w)),units,x+27,y+120,text_color,back_color);
    }
      //wind direction
    float deg=round(weather.deg/45)*45+180;
    float x0=x+21;
    float y0=y+129;
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
  }
    //humidity
  float humidity=get_humidity_out();
  drawFSJpeg("/pic/hum.jpg",x+109,y+64);
  if(updated<1800 and humidity<120 and (config.h_out==1 or config.h_out==2)) out=true;
  else if(config.h_out>=3 and config.h_out<=6) out=true;
  else{
    humidity=weather.humidity;
    out=false;
  }
  str=String(round(humidity));
  if(humidity<120.0) printInt(str,"%",x+122,y+80,out?out_color:text_color,back_color);
  else printInt("--","",x+122,y+80,text_color,back_color);    
    //pressure
  drawFSJpeg("/pic/pres.jpg",x+108,y+104);
  int pres=get_pres_out();
  if(updated<1800 and pres<1300 and (config.p_out==1 or config.p_out==2)) out=true;
  else if(config.p_out==3 or config.p_out==4) out=true;
  else{
    pres=weather.pressure;
    out=false;
  }
  pres+=config.po_cor;
  config.po_units?str=String(round(pres),DEC):str=String(round(0.75*pres),DEC);
  if(pres>2000) str="--";
  if(config.po_units)
    printCent(UTF8(WeatherNow[config.lang].hpa),x+142,x+210,y+108,out?out_color:text_color,back_color,SmallFontRu);
  printInt(str,config.po_units?"":UTF8(WeatherNow[config.lang].mm),x+124,y+120,out?out_color:text_color,back_color);  
  if(l_upd){
      //updated
    updtd="~ ";
    if(!config.timef){
      updtd+=isAM()?UTF8(WeatherNow[config.lang].AM):UTF8(WeatherNow[config.lang].PM);
      updtd+=" ";
    }
    updtd+=config.timef?String(hour(weather.updated)):String(hourFormat12(weather.updated));
    updtd+=":";
    if(minute(weather.updated)<10) updtd+="0";
    updtd+=String(minute(weather.updated)); updtd+=":";
    if(second(weather.updated)<10) updtd+="0";
    updtd+=String(second(weather.updated)); updtd+=" ";
    updtd+=String(day(weather.updated)); updtd+="-";
    updtd+=String(month(weather.updated)); updtd+="-";
    updtd+=String(year(weather.updated));
    printCent(updtd,x,x+208,y+138,text_color,back_color,SmallFontRu);
      //outside battery
    out_bat();
      //description
    char descript[80];
    if(config.lang==1 or config.lang==5 or config.lang==7 or config.lang==8){
      if(config.provider==0){
        String d=description(weather.id); 
        if(d) d.toCharArray(descript,d.length()+1);
      }
      else sprintf(descript,"%s",weather.descript);
    }
    else sprintf(descript,"%s",weather.descript);
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
}

void out_bat(void){
  int dayLight=0;
  if(config.adj) if(summertime()) dayLight=3600;
  uint32_t updated=now()-(config.zone*3600)-dayLight-outside.updated;
  char bat=BAT0;
  if(updated<1800 and outside.bat<6){
    myGLCD.setColor(out_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(Symbols);
    if(outside.bat==1) myGLCD.setColor(VGA_RED);
    if(outside.bat==2) bat=BAT25;
    if(outside.bat==3) bat=BAT50;
    if(outside.bat==4) bat=BAT75;
    if(outside.bat==5) bat=BAT100;
    sprintf(text_buf,"%c",bat);
    myGLCD.print(text_buf,5,0);
    myGLCD.setColor(back_color);
    myGLCD.fillRect(0,0,4,16);
    myGLCD.fillRect(21,0,22,16);
  }
  else{
    myGLCD.setColor(back_color);
    myGLCD.fillRect(0,0,22,16);
  }  
}

void showInsideTemp(void){
  int x=215,y=96, dayLight=0;
  if(config.adj) if(summertime()) dayLight=3600;
  uint32_t updated=now()-(config.zone*3600)-dayLight-outside.updated;
  if(temp_draw==0 or hum_draw==0){
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x,y,myGLCD.getDisplayXSize()-1,y+74);
    if(config.hum==0) /*if(!need_upd_icon)*/ drawFSJpeg("/pic/home.jpg",x+2,y+16);
    if(config.hum>0){
      drawFSJpeg("/pic/home.jpg",x+2,y);
      drawFSJpeg("/pic/hum.jpg",x+3,y+31);
    }
    myGLCD.setColor(rama_color);
    myGLCD.drawRect(x,y,x+96,y+64);
    myGLCD.drawRect(x+1,y+1,x+95,y+63);
    tempInside=get_temp(!config.ti_units);
    humInside=get_humidity();
  }
  if(config.sleep==0 or config.sleep>3){
    tempInside=get_temp(!config.ti_units);
    humInside=get_humidity();
  }
  if(temp_draw!=tempInside){
    if(tempInside>=0 and tempInside<100){
      if(config.ti_round){
        myGLCD.setFont(BigFontRu);
        printData(tempInside<10.0?" "+String(tempInside):String(tempInside),config.ti_units?"^F":"^C",x+23,!config.hum?y+25:y+9,config.temp>99?updated>1800?VGA_RED:out_color:text_color,back_color);
      }
      else{
        myGLCD.setFont(Arial_round);
        printInt(tempInside<10.0?" "+String(round(tempInside)):String(round(tempInside)),config.ti_units?"^F":"^C",x+19,!config.hum?y+25:y+9,config.temp>99?updated>1800?VGA_RED:out_color:text_color,back_color);
      }
    }
    temp_draw=tempInside;
  }
  if(hum_draw!=humInside){
    if(humInside>=0 and humInside<=100){
      if(config.hi_round){
        myGLCD.setFont(BigFontRu);
        printData(humInside<10.0?" "+String(humInside):String(humInside),"%",x+25,y+41,config.hum>99?updated>1800?VGA_RED:out_color:text_color,back_color);
      }
      else{
        myGLCD.setFont(Arial_round);
        printInt(humInside<10.0?" "+String(round(humInside)):String(round(humInside)),"%",x+16,y+41,config.hum>99?updated>1800?VGA_RED:out_color:text_color,back_color);
      }
    }
    hum_draw=humInside;
    myGLCD.setFont(BigFontRu);
  }
    //update line
  float u=(now()-weather.updated)/10;
  if(u>=0 and u<121){
    myGLCD.setColor(text_color);
    myGLCD.drawRect(3,48+int(u),4,168);
    myGLCD.setColor(back_color);
    myGLCD.drawRect(3,48,4,48+int(u));
  }
}

void showTime(void){
  int x=215,y=22;
  if(clock_draw==false){
    myGLCD.setColor(back_color);
    myGLCD.fillRect(x,y-4,myGLCD.getDisplayXSize()-1,y+73);
    myGLCD.fillRect(0,0,myGLCD.getDisplayXSize()-1,17);
    myGLCD.setColor(rama_color);
    myGLCD.drawRect(x,y,x+96,y+64);
    myGLCD.drawRect(x+1,y+1,x+95,y+63);
    clock_draw=true;
  }
  if(clock_upd!=minute()){
    clock_upd=minute();
      //weekday
    printCent(UTF8(Weekday[weekday()-1][config.lang]),x+2,x+94,y+45,text_color,back_color,SmallFontRu);
      //time
    sprintf(text_buf,"%d:%02d",config.timef?hour():hourFormat12(),minute());
    printCent(text_buf,x+7,x+89,y+18,text_color,back_color,DotMatrix_M_Num);
    if(!config.timef) 
      printCent(isAM()?UTF8(WeatherNow[config.lang].AM):UTF8(WeatherNow[config.lang].PM),x+7,x+89,y+2,text_color,back_color,SmallFontRu);
  }

  if(sm!=month()-1 || sd!=day()){
    String fileData;
    String fileName="/save/"+String(month()-1)+".json";
    File file=SPIFFS.open(fileName,"r");
    if(file){
      fileData=file.readString();
      file.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root=jsonBuffer.parseObject(fileData);
      if(root.success()){
        const char* e=root[String(day())];
        sprintf(events,"%s",e);
        sm=month()-1;
        sd=day();
      }
    }
  }
  if(e_str==0){
     //date
    myGLCD.setColor(text_color);
    myGLCD.setBackColor(back_color);
    myGLCD.setFont(BigFontRu);
    uint8_t ld=(String(day()).length())*16;
    uint8_t lm=(String(UTF8(Month[month()-1][config.lang])).length())*8;
    uint8_t ly=64;
    uint8_t ls=ld+lm+ly;
    myGLCD.print(String(day()),160-ls/2,0);
    myGLCD.setFont(SmallFontRu);
    myGLCD.print(UTF8(Month[month()-1][config.lang]),160-ls/2+ld,4);
    myGLCD.setFont(BigFontRu);
    myGLCD.print(String(year()),160-ls/2+ld+lm,0);
    myGLCD.setColor(back_color);
    myGLCD.fillRect(23,0,159-ls/2,17);
    myGLCD.fillRect(160-ls/2+ls,0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,17);
    myGLCD.fillRect(160-ls/2+ld,0,160-ls/2+ld+lm,3);
  }
  if(e_str==1 && String(UTF8(events)).length()>1){
    if(String(UTF8(events)).length()<16) printCent(UTF8(events),23,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,0,text_color,back_color,BigFontRu);
    else{
      printCent(UTF8(events),23,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,4,text_color,back_color,SmallFontRu);
      myGLCD.fillRect(23,0,config.ac==0?config.battery==0?272:config.battery==1?264:289:289,3);
    }    
  }
}

uint8_t utf8_symb(uint8_t a,uint8_t b){
  uint8_t s=0x3F;
  if(a==0xC3){
    if(b==0x82) s=0x82;// Â
    if(b==0x84) s=0x8A;// Ä
    if(b==0x87) s=0xAD;// Ç
    if(b==0x8E) s=0x88;// Î
    if(b==0x95) s=0xA3;// Õ
    if(b==0x96) s=0x8C;// Ö
    if(b==0x9C) s=0x8E;// Ü
    if(b==0x9F) s=0x90;// ß
    if(b==0xA2) s=0x83;// â
    if(b==0xA4) s=0x8B;// ä
    if(b==0xA7) s=0xAE;// ç
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
    if(b==0x96) s=0x96;// Ė
    if(b==0x97) s=0x98;// ė
    if(b==0x98) s=0x95;// Ę
    if(b==0x99) s=0x97;// ę
    if(b==0x9E) s=0xB1;// Ğ
    if(b==0x9F) s=0xB2;// ğ
    if(b==0xAE) s=0x99;// Į
    if(b==0xAF) s=0x9A;// į
    if(b==0xB0) s=0xB5;// İ
    if(b==0xB1) s=0xB6;// ı
  }
  if(a==0xC5){
    if(b==0x9E) s=0xB3;// Ş
    if(b==0x9F) s=0xB4;// ş
    if(b==0xA0) s=0x9B;// Š
    if(b==0xA1) s=0x9C;// š
    if(b==0xAA) s=0x9E;// Ū
    if(b==0xB2) s=0x9D;// Ų    
    if(b==0xB3) s=0x9F;// ų
    if(b==0xBD) s=0xA1;// Ž
    if(b==0xBE) s=0xA2;// ž
  }
  if(a==0xC6){
    if(b==0x8F) s=0xAF;// Ə
  }
  if(a==0xC8){
    if(b==0x98) s=0x84;// Ș
    if(b==0x99) s=0x85;// ș
    if(b==0x9A) s=0x86;// Ț
    if(b==0x9B) s=0x87;// ț
  }
  if(a==0xC9){
    if(b==0x99) s=0xB0;// ə
  }
  if(a==0xD0){
    if(b==0x81) s=0xAB;// Ё
    if(b==0x84) s=0xA7;// Є
    if(b==0x86) s=0x49;// I
    if(b==0x87) s=0xA9;// Ї
    if(b==0x8E) s=0xB7;// Ў
    if(b>0x8F) if(b<0xC0) s=b+48;  //А-Я,а-п 
  }
  if(a==0xD1){
    if(b>0x7F) if(b<0x90) s=b+112;//р-я
    if(b==0x91) s=0xAC;// ё
    if(b==0x94) s=0xA8;// є
    if(b==0x96) s=0x69;// i
    if(b==0x97) s=0xAA;// ї 
    if(b==0x9E) s=0xB8;// ў
  }
  if(a==0xD2){
    if(b==0x90) s=0xA5;// Ґ
    if(b==0x91) s=0xA6;// ґ
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
    if(config.lang==5) file=SPIFFS.open("/lang/et200.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua200.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az200.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by200.json","r");
  }
  if(code>=300 and code<400){
    if(config.lang==5) file=SPIFFS.open("/lang/et300.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua300.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az300.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by300.json","r");
  }
  if(code>=500 and code<600){
    if(config.lang==5) file=SPIFFS.open("/lang/et500.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua500.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az500.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by500.json","r");
  }
  if(code>=600 and code<700){
    if(config.lang==5) file=SPIFFS.open("/lang/et600.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua600.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az600.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by600.json","r");
  }
  if(code>=700 and code<800){
    if(config.lang==5) file=SPIFFS.open("/lang/et700.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua700.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az700.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by700.json","r");
  }
  if(code>=800 and code<900){
    if(config.lang==5) file=SPIFFS.open("/lang/et800.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua800.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az800.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by800.json","r");
  }
  if(code>=900 and code<1000){
    if(config.lang==5) file=SPIFFS.open("/lang/et900.json","r");
    if(config.lang==6) file=SPIFFS.open("/lang/ua900.json","r");
    if(config.lang==7) file=SPIFFS.open("/lang/az900.json","r");
    if(config.lang==8) file=SPIFFS.open("/lang/by900.json","r");
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

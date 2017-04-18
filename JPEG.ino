void drawFSJpeg(const char *filename,int xpos,int ypos){
  fs::File jpgFile=SPIFFS.open(filename,"r");
  if(!jpgFile) return;
  JpegDec.decodeFsFile(filename);
  renderJPEG(xpos,ypos);
}

void renderJPEG(int xpos,int ypos){
  uint16_t *pImg;
  uint16_t mcu_w=JpegDec.MCUWidth;
  uint16_t mcu_h=JpegDec.MCUHeight;
  uint32_t mcu_pixels=mcu_w*mcu_h;
  while(JpegDec.read()){
    pImg=JpegDec.pImage;
    int mcu_x=JpegDec.MCUx*mcu_w+xpos;
    int mcu_y=JpegDec.MCUy*mcu_h+ypos;
    if((mcu_x+mcu_w)<=myGLCD.getDisplayXSize()&&(mcu_y+mcu_h)<=myGLCD.getDisplayYSize()){
      draw(mcu_x,mcu_y,mcu_x+mcu_w-1,mcu_y+mcu_h-1,pImg);
    }
    else if((mcu_y+mcu_h)>=240) JpegDec.abort();
  }
}

void draw(int x,int y,int sx,int sy,uint16_t *data){
  int a,b,c=0,ta,tb;
  uint16_t tmp[16][16];
  for(a=0;a<(sx-x+1);a++){
    for(b=0;b<(sy-y+1);b++){
      tmp[a][b]=data[c++];
    }
  }
  pinMode(CS,OUTPUT);
  digitalWrite(CS,LOW);
  myGLCD.setXY(x,y,sx,sy);
  for(ta=sx-x;ta>=0;ta--){
    for(tb=0;tb<(sy-y+1);tb++){
      myGLCD.LCD_Write_DATA(tmp[tb][ta]&0xff,tmp[tb][ta]>>8);
    }
  }
  pinMode(CS,OUTPUT);
  digitalWrite(CS,HIGH);  
}


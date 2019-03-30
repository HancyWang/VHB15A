#include "delay.h"
#include "lcd.h"
#include "Font.h"
#include "all.h"

uint8_t  xdata  Back_Color;

//2019.03.29
sfr P0   = 0x80;
sbit P14 = 0x90^4;
sbit P15 = 0x90^5;
sbit P16 = 0x90^6;
sbit P40 = 0xC0^0;
sbit P46 = 0xC0^6;

static uint16_t code Table_Color[]=
{
	RED,
	GREEN,
	BLUE,
	WHITE, 
	BLACK , 
	GRAY,  
	YELLOW ,
	BRIGHT_BLUE,
	AIR_BLUE,
	PURPLE,
	BLACKRED 	 
};


static void SdCmd(uint8_t data8)	 //2014-06-10 V1.04更换夏瑞的LCD屏，驱动由ILI9328变更为ILI9341
{
	LCD_RS_L;
	LCD_PORT =data8;
	LCD_CS_L;
	LCD_WR_L;
	LCD_WR_H;
	LCD_CS_H;
}


static void SdData(uint8_t data8)  //2014-06-10 V1.04更换夏瑞的LCD屏，驱动由ILI9328变更为ILI9341
{
	LCD_RS_H;
	LCD_PORT =data8;
	LCD_CS_L;
	LCD_WR_L;
	LCD_WR_H;
	LCD_CS_H;
}

void LCD_CtrlWrite_ILI9328(uint16_t i)
{
	LCD_CS_L;
	LCD_RS_H;
	LCD_PORT =i/256;
	LCD_WR_L;
	LCD_WR_H;
	LCD_PORT =i%256;
	LCD_WR_L;
	LCD_WR_H;
	LCD_RS_H;
	LCD_CS_H;
}

void   LCD_CtrlWrite_Color(uint8_t color)
{
   LCD_CtrlWrite_ILI9328(Table_Color[color]);
}

void LCD_Initial(void)
{
 
	LCD_RST_L;
	delay_ms(20); // Delay 1ms
	LCD_RST_H;
	delay_ms(800); // Delay 10ms // This delay time is necessary
	
	SdCmd(0x11);//Wake up
	delay_ms(120); //Delay 120ms
	SdCmd(0x35);SdData(0x00);//TE ON   60Hz
			
	SdCmd(0x36);
	SdData(0x00); //RTN
	SdCmd(0x21); //黑白反相	 

	SdCmd(0xb2);  //Porch setting
	SdData(0x0c);
	SdData(0x0c);
	SdData(0x00);
	SdData(0x33);
	SdData(0x33);

	SdCmd(0xb7);  //Gate Control
	SdData(0x35); //VGH=13.26V, VGL=-10.43V

	SdCmd(0xbb); //vcom setting
	SdData(0x19);

	SdCmd(0xc0); //LCM Control
	SdData(0x2c);

	SdCmd(0xc2);  //VDV and VRH Command Enable	
	SdData(0x01);

	SdCmd(0xc3); //VRH Set, VAP(GVDD) & VAN(GVCL)
	SdData(0x1d); 

	SdCmd(0xc4);  //VDV Set, 
	SdData(0x20); //VDV=0V

	SdCmd(0xc6);//Frame rate control in normal mode
	SdData(0x0f);//dot inversion & 60Hz

	SdCmd(0xd0);   //Power control 1
	SdData(0xa4);
	SdData(0xa1); //AVDD=6.8V; AVCL=-4.8V; VDDS=2.3V

	SdCmd(0xe8);
	SdData(0x83);
//--------------------------------ST7789VI gamma setting---------------------------------------//
  SdCmd(0xe0);
	SdData(0xd2);
	SdData(0x11);
	SdData(0x16);
	SdData(0x0d);
	SdData(0x0d);
	SdData(0x38);
	SdData(0x3d);
	SdData(0x44);
	SdData(0x4c);
	SdData(0x1c);
	SdData(0x15);
	SdData(0x15);
	SdData(0x1c);
	SdData(0x20);


	SdCmd(0xe1);
	SdData(0xd2);
	SdData(0x11);
	SdData(0x16);
	SdData(0x0d);
	SdData(0x0d);
	SdData(0x38);
	SdData(0x3d);
	SdData(0x44);
	SdData(0x4c);
	SdData(0x1c);
	SdData(0x15);
	SdData(0x15);
	SdData(0x1c);
	SdData(0x20);

	SdCmd(0x3a);SdData(0x55); // 8/16bit,65k colors
	SdCmd(0x29);             //Display on

	delay_ms(10);

	SdCmd(0x2a);
	SdData(0x00);
	SdData(0x00);
	SdData(0x00);
	SdData(0xEF);

	SdCmd(0x2b);
	SdData(0x00);
	SdData(0x00);
	SdData(0x01);
	SdData(0x3F);

	delay_ms(120);
	SdCmd(0x2c);
}

void  LCD_CrtlWrite_Area(uint8_t SX,uint8_t EX,uint16_t SY,uint16_t EY)
{
	   SdCmd(0x2A);
	   SdData(SX/256);
	   SdData(SX%256);
	   SdData(EX/256);
	   SdData(EX%256);
	   SdCmd(0x2B);
	   SdData(SY/256);
	   SdData(SY%256);
	   SdData(EY/256);
	   SdData(EY%256);
	   SdCmd(0x2C);	
}  


 //画矩形
void Draw_Rectangle(uint8_t Start_X, uint16_t Start_Y,uint8_t End_X, uint16_t End_Y,uint8_t color, uint8_t LineW)
{
 	   uint16_t i,j,w,h;
 	   w=End_X-Start_X;
 	   h=End_Y-Start_Y;
 	   //画第一条线
	   LCD_CrtlWrite_Area(Start_X,End_X,Start_Y,Start_Y+LineW-1);
     for (j=0; j<LineW;j++)
     {
 	     for(i=0;i<=w;i++)
 	     {
 	   	    LCD_CtrlWrite_Color(color);
 	     }
 	   }
	   LCD_CrtlWrite_Area(Start_X,End_X,End_Y-LineW+1,End_Y);
     for (j=0; j<LineW;j++)
     {
 	     for(i=0;i<=w;i++)
 	     {
 	   	      LCD_CtrlWrite_Color(color);
 	     }
 	   }
	   LCD_CrtlWrite_Area(Start_X,Start_X+LineW-1,Start_Y,End_Y);
     for (j=0; j<=h;j++)
     {
 	     for(i=0;i<LineW;i++)
 	     {
 	   	     LCD_CtrlWrite_Color(color);
 	     }
 	   }
	   LCD_CrtlWrite_Area(End_X-LineW+1,End_X,Start_Y,End_Y);
     for (j=0; j<=h;j++)
     {
 	     for(i=0;i<LineW;i++)
 	     {
 	   	    LCD_CtrlWrite_Color(color);
 	     }
 	   }
 }

 //画实体方块
void Draw_Rectangle_Real(uint8_t Start_X, uint16_t Start_Y,uint8_t End_X, uint16_t End_Y,uint8_t color)
{
 	   uint16_t i,j,w,h;
 	   w=End_X-Start_X;
 	   h=End_Y-Start_Y;
	   LCD_CrtlWrite_Area(Start_X,End_X,Start_Y,End_Y);
     for (j=0; j<=h;j++)
     {
 	     for(i=0;i<=w;i++)
 	     {
 	   	        LCD_CtrlWrite_Color(color);

 	     }
 	   }
}

//显示单色图形
//X-坐标 0-239
//Y-坐标 0-319
//H-高   2-200
//W-宽   1-100
//gp 字符地址
//NUM-图形编号
//color-颜色
static void DISP_MONO_GRAP(uint8_t  X1, uint16_t Y1,uint8_t  H, uint8_t  W, uint8_t *gp,uint8_t color)
{
	   uint16_t w,j;
	   LCD_CrtlWrite_Area(X1,X1+H-1,Y1,Y1+W-1);
	   for(w=0;w<(int)H*(int)W/8;w++)
	   {
	 	   for(j=0;j<8;j++)
	 	   {
	 	  	 if((gp[w]&(0x01<<j))!=0)
	 	        LCD_CtrlWrite_Color(color);
	 	     else
	 	   	    LCD_CtrlWrite_Color(Back_Color);
	 	    }
	 	 }
}


void DISP_TEMP_30X56(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,56,30,NUM30X56[NUM],color);
}

void DISP_TEMP_38X64(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,64,38,NUM38X64[NUM],color);
}

void DISP_CQK25X24(uint8_t  X, uint16_t Y,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,24,25,CQK25X24,color);
}

void DISP_RTD28X24(uint8_t  X, uint16_t Y,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,24,28,RTD28X24,color);
}

void DISP_JQK41X16(uint8_t  X, uint16_t Y,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,16,41,JQK41X16,color);
}



void DISP_HEAT_36X24(uint8_t  X, uint16_t Y,uint8_t color)
{
		 DISP_MONO_GRAP(X,Y,24,36,HEAT36X32,color);
}

void DISP_RH_17X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,40,17,NUM17X40[NUM],color);
}

void DISP_FNT10X24(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,24,10,FNT10X24[NUM],color);
}

void DISP_RH14X24(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,24,14,RH14X24[NUM],color);
}

void DISP_TIME_10X16(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,16,10,NUM10X16[NUM],color);
}
void DISP_ICO_32X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,40,32,ICO32X40[NUM],color);	
}

void DISP_ICO_40X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,40,40,ICO40X40[NUM],color);
}



void DISP_ICO_52X64(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,48,48,ICO48X48[NUM],color);	
}


void DISP_DHM(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,16,14,DHM14X16[NUM],color);
}  

void DISP_VHB80_56X40(uint8_t  X, uint16_t Y,uint8_t color)
{
	DISP_MONO_GRAP(X,Y,40,56,VHB8056X40,color);
}


void Disp_YouChuang(uint8_t  X, uint16_t Y,uint8_t color) //80,71
{
	DISP_MONO_GRAP(X,Y,80,71,PIC_YouChuang,color); 
}

void Disp_WuChuang(uint8_t  X, uint16_t Y,uint8_t color)   //
{
	DISP_MONO_GRAP(X,Y,80,71,PIC_WuChuang,color);
}

void DISP_VHB80_PIC(uint8_t color)
{	
  DISP_VHB80_56X40(POS_VHB80_X,POS_VHB80_Y,color);
  DISP_HEAT_36X24(POS_HEAT_X,POS_HEAT_Y,color);    //加热盘图形    
  DISP_CQK25X24(POS_CQK_X,POS_CQK_Y,color);
  Draw_Rectangle_Real(POS_XQGS_X1,POS_XQGS_Y1,POS_XQGS_X2,POS_XQGS_Y2,color);  //画吸气管上部，下部
  Draw_Rectangle_Real(POS_XQGX_X1,POS_XQGX_Y1,POS_XQGX_X2,POS_XQGX_Y2,color); 
  DISP_RTD28X24(POS_RTD_X,POS_RTD_Y,color);    
  Draw_Rectangle_Real(POS_CQGS_X1,POS_CQGS_Y1,POS_CQGS_X2,POS_CQGS_Y2,color);  //画出气管上部，下部
  Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGX_X2,POS_CQGX_Y2,color); ;  //画出气管上部，下部
  Draw_Rectangle_Real(POS_CQGX_X1,POS_CQGX_Y1,POS_CQGS_X2,POS_CQGX_Y1+1,color);  //画封口 
  DISP_JQK41X16(POS_JQK_X,POS_JQK_Y,color);	    //画进气口
}


//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
static void LCD_DrawPoint(u16 x,u16 y,uint8_t color)
{	
	SdCmd(0x2A);;	 //设置光标位置 
	SdData(x/256);	  //开始点的X坐标
	SdData(x%256);
	SdData(x/256);
	SdData(x%256);	   
	SdCmd(0x2B);
	SdData(y/256);	//开始点的Y坐标	   
	SdData(y%256);	   
	SdData(y/256);		   
	SdData(y%256);
	SdCmd(0x2C);		
	LCD_CtrlWrite_Color(color); //开始写入GRAM
}

//*	 画点的方式
//显示16*8 ASC字符  
//X座标 0-239
//Y座标 0-319
//NUM字符
//mode:叠加方式(1)不写背景色  还是非叠加方式(0)写背景色
//color字符的颜色	
 void DISP_FNT8X16(u16  X, u16 Y,u8 NUM,u8 mode,u8 color)
{
	   uint16_t  w,j;
	   uint16_t F_x,F_y;

	   F_x = X;
	   F_y = Y;	 

	  LCD_CrtlWrite_Area(X,X+15,Y,Y+7);

	   NUM=NUM-' ';//得到偏移后的值		   
	   for(w=0;w<8;w++)
	   {
	 	   for(j=0;j<8;j++)
	 	   {
	 	  	 if((asc2_1608[NUM][w*2+1]&(0x01<<j))!=0)
			 {
			 	if(mode == 0)
					LCD_CtrlWrite_Color(color); //快速写
				else
	 	        	LCD_DrawPoint(F_x,F_y,color);//开始写入GRAM
				
			 }
	 	     else
	 	   	    {
			 		if(mode == 0)
	 	   	    	LCD_CtrlWrite_Color(Back_Color);
			    }
			 F_x++;				
	 	   } 
		   for(j=0;j<8;j++)
	 	   {
	 	  	 if((asc2_1608[NUM][w*2]&(0x01<<j))!=0)
	 	      {
			 	if(mode == 0)
					LCD_CtrlWrite_Color(color); //快速写
				else
	 	        	LCD_DrawPoint(F_x,F_y,color);//开始写入GRAM
				
			 }
	 	     else
	 	   	    {
			 		if(mode == 0)
	 	   	    	LCD_CtrlWrite_Color(Back_Color);
			    }
			 F_x++;				
	 	   }
		   F_y++; 
		   F_x = X; 
	 	} 
}

//*	 画点的方式 可以显示叠加方式
//显示24*12 ASC字符  
//X座标 0-239
//Y座标 0-319
//NUM字符
//mode:叠加方式(1)不写背景色  还是非叠加方式(0)写背景色 速度快，画框方式
//color字符的颜色 
 void DISP_FNT12X24(u16 X, u16 Y,u8 NUM,u8 mode,u8 color)
{
	   uint16_t  w,j;
	   uint16_t F_x,F_y; 	   

	   F_x = X;
	   F_y = Y;

	   LCD_CrtlWrite_Area(X,X+23,Y,Y+11);

	   NUM=NUM-' ';//得到偏移后的值		   
	   for(w=0;w<12;w++)
	   {
	   	   for(j=0;j<8;j++)
	 	   {
	 	  	 if((asc2_2412[NUM][w*3+2]&(0x01<<j))!=0)
			 {
			 	if(mode == 0)
					LCD_CtrlWrite_Color(color); //快速写
				else
	 	        	LCD_DrawPoint(F_x,F_y,color);//开始写入GRAM
				
			 }
	 	     else
	 	   	    {
			 		if(mode == 0)
	 	   	    	LCD_CtrlWrite_Color(Back_Color);
			    }
				F_x++;	
				
	 	   } 
	 	   for(j=0;j<8;j++)
	 	   {
	 	  	 if((asc2_2412[NUM][w*3+1]&(0x01<<j))!=0)
	 	     {
			 	if(mode == 0)
					LCD_CtrlWrite_Color(color); //快速写
				else
	 	        	LCD_DrawPoint(F_x,F_y,color);//开始写入GRAM
				
			 }
	 	     else
	 	   	    {
			 		if(mode == 0)
	 	   	    	LCD_CtrlWrite_Color(Back_Color);
			    }
				F_x++;
				
	 	   } 
		   for(j=0;j<8;j++)
	 	   {
	 	  	 if((asc2_2412[NUM][w*3]&(0x01<<j))!=0)
	 	     {
			 	if(mode == 0)
					LCD_CtrlWrite_Color(color); //快速写
				else
	 	        	LCD_DrawPoint(F_x,F_y,color);//开始写入GRAM
				
			 }
	 	     else
	 	   	    {
			 		if(mode == 0)
	 	   	    	LCD_CtrlWrite_Color(Back_Color);
			    }
			   F_x++;

	 	   }  
		   F_y++; 
		   F_x = X; 
	 	} 
}

//在指定位置显示一个字符
//x,y:起始坐标
//size:字体大小 16/24
//num:要显示的字符:" "--->"~" 
//mode:叠加方式(1)不写背景色  还是非叠加方式(0)写背景色
//color:
void LCD_ShowChar(u16 x,u16 y,u8 size,u8 num,u8 mode,u8 color)
{  							  
    
	//设置窗口
	if(size == 16)
        	DISP_FNT8X16(x,y,num,mode,color);
		else if(size == 24)
        	DISP_FNT12X24(x,y,num,mode,color); 
}  


//显示16*8 或24*12 ASC字符
//x,y:起点坐标
//height_x	字高度，也是字号,如16表示16*8，24表示24*12
//width_y: 文字的最大长度，以免超出范围，最大是320 
//*p:字符串起始地址
//mode:叠加方式(1)不写背景色  还是非叠加方式(0)写背景色
//字符的颜色
void LCD_ShowString(u16 x, u16 y,u16 height_x,u16 width_y, u8 *p,u8 mode,u8 color)
{ 
	uint8_t size;	        
	uint8_t y0;
	y0=y;
	//size = 16;

	//LCD_CrtlWrite_Area(0,239,0,319);
	size = height_x;  //字号或高度
	height_x+=x; //
	width_y+=y;//总宽度
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(y>=width_y){y=y0;y+=size;}
        //if(x>=height_x)break;//退出
		LCD_ShowChar(x,y,size,*p,mode,color);

        y+=size/2;	
        p++;
    }  
}

//m^n函数
//返回值:m^n次方.
static u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		


//显示数字,高位为0,还是显示
//x,y:起点坐标
//size:字体大小	 16或24	 
//len:长度(即要显示的位数)
//num:数值(0~999999999);
//mode:	  7和0位使用
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
//color:颜色 
void LCD_ShowxNum(u16 x,u16 y,u8 size,u8 len,u32 num,u8 mode,u8 color)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;	 //求需要显示的最高位
		if(enshow==0&&t<(len-1))// 如果最高位为0   需要显示的长度
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x,y+(size/2)*t,size,'0',mode&0X01,color);  //为0也要显示
				else LCD_ShowChar(x,y+(size/2)*t,size,' ',mode&0X01,color);  //为0不显示
 				continue;
			}else enshow=1; //最高位不是0或	 		 	 
		}
	 	LCD_ShowChar(x,y+(size/2)*t,size,temp+'0',mode&0X01,color); 
	}
} 

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r,u8 color)
{
	int a,b;
	int di;

	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b,color);             //5
 		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-a,y0+b,color);             //1       
 		LCD_DrawPoint(x0-b,y0+a,color);             
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
  	LCD_DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 

void Refresh_Work_Mode(void)
{
	  Back_Color=WHITE18;
    if(Work_Mode==0)
    {
         Disp_WuChuang(POS_YWC_X,POS_YWC_Y,PURPLE18);
    }else if(Work_Mode==1)
    {
         Disp_YouChuang(POS_YWC_X,POS_YWC_Y,PURPLE18);
    }else//无简易模式
    {
           ;
    }
}



//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void DrawWorkWindows(void)
{
    LCD_LIGHT_CLOSE;
    //清除选择界面-----------------------------------------
		Draw_Rectangle_Real(0,0,239,319,WHITE18);//清屏
    Back_Color=WHITE18;
    Disp_WuChuang(POS_Select_WC_X,POS_Select_WC_Y,WHITE18);
    Disp_YouChuang(POS_Select_YC_X,POS_Select_YC_Y,WHITE18);
    Draw_Rectangle(75,182,162,262,WHITE18,4);//
    Draw_Rectangle(75,58,162,136,WHITE18,4);
    Refresh_Work_Mode();
    Back_Color=WHITE18;
    DISP_ICO_52X64(POS_ICO_TEMP_X,POS_ICO_TEMP_Y,0,BLACK18);
    DISP_ICO_40X40(POS_ICO_SHIDU_X,POS_ICO_TEMP_Y,0,BLACK18);
    DISP_ICO_40X40(POS_ICO_TIME_X,POS_ICO_TEMP_Y,1,BLACK18);//显示时间
    //画点
    Draw_Rectangle_Real(POS_RT_TEMP_X+1,POS_RT_TEMP_Y+67,POS_RT_TEMP_X+8,POS_RT_TEMP_Y+71,BLACK18); //画点
    //显示单位
    DISP_ICO_32X40(POS_RT_TEMP_X,POS_RT_TEMP_Y+34*3+5,ICO_DU,BLACK18);
  
    //显示湿度单位
    DISP_ICO_32X40(POS_RT_RH_X,POS_RT_RH_Y+19*2,ICO_PER,BLACK18);  
		if(data_flash.Language == Lan_English)//英语
		{
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+70,0,BLACK18);
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+84,1,BLACK18);
		}
		else if(data_flash.Language == Lan_Portuguese)//葡萄牙语
		{
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+70,2,BLACK18);
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+84,0,BLACK18);
		}
		else if(data_flash.Language == Lan_Spanish)//西班牙语
		{
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+70,1,BLACK18);
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+84,0,BLACK18);
		}
		else if(data_flash.Language == Lan_French)//法语
		{
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+70,1,BLACK18);
			DISP_RH14X24(POS_RT_RH_X,POS_RT_RH_Y+84,0,BLACK18);
		}		
     
    DISP_DHM(POS_DMH_X,POS_DMH_Y,0,BLACK18);
    DISP_DHM(POS_DMH_X,POS_DMH_Y+25,1,BLACK18);
    DISP_DHM(POS_DMH_X,POS_DMH_Y+25*2,2,BLACK18);   
    //显示运行时间

    Draw_Rectangle_Real(POS_ICO_TIME_X,POS_DMH_Y+18,POS_ICO_TIME_X+2,POS_DMH_Y+20,BLACK18);
    Draw_Rectangle_Real(POS_ICO_TIME_X+10,POS_DMH_Y+18,POS_ICO_TIME_X+12,POS_DMH_Y+20,BLACK18);

    Draw_Rectangle_Real(POS_ICO_TIME_X,POS_DMH_Y+43,POS_ICO_TIME_X+2,POS_DMH_Y+45,BLACK18);
    Draw_Rectangle_Real(POS_ICO_TIME_X+10,POS_DMH_Y+43,POS_ICO_TIME_X+12,POS_DMH_Y+45,BLACK18);
		
		DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y,2,BLACK18);
    DISP_TIME_10X16(POS_SYSDATE_X,POS_SYSDATE_Y+12,0,BLACK18);
	
    Draw_Rectangle_Real(POS_SYSDATE_X,POS_SYSDATE_Y+48,POS_SYSDATE_X+2,POS_SYSDATE_Y+50,BLACK18);

    Draw_Rectangle_Real(POS_SYSDATE_X,POS_SYSDATE_Y+76,POS_SYSDATE_X+2,POS_SYSDATE_Y+78,BLACK18);

    Draw_Rectangle_Real(POS_SYSTIME_X,POS_SYSTIME_Y+24,POS_SYSTIME_X+2,POS_SYSTIME_Y+26,BLACK18);
    Draw_Rectangle_Real(POS_SYSTIME_X+10,POS_SYSTIME_Y+24,POS_SYSTIME_X+12,POS_SYSTIME_Y+26,BLACK18);

    Draw_Rectangle_Real(POS_SYSTIME_X,POS_SYSTIME_Y+52,POS_SYSTIME_X+2,POS_SYSTIME_Y+54,BLACK18);
    Draw_Rectangle_Real(POS_SYSTIME_X+10,POS_SYSTIME_Y+52,POS_SYSTIME_X+12,POS_SYSTIME_Y+54,BLACK18);     
} 

//显示版本号
//VHB15A_EN_VM_V1.07      EN-英文国家   VM-VM默认功能及界面
void	LCD_Show_Verion(void)
{
	LCD_ShowString(60,20,16,200,"VHB15A",0,BLACK18);	//显示机种名
	
	LCD_ShowString(60,68,16,200,"---EN",0,BLACK18);	//显示版本语言		

	LCD_ShowString(60,108,16,200,"---VM",0,BLACK18);	//显示客户 

	

//显示版本号
	LCD_ShowString(40,20,16,200,VHB15A_VER,0,GRAY18); 
}


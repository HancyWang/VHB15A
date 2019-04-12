#ifndef __LCD_H
#define __LCD_H

#include "all.h" 

extern uint8_t    Back_Color;


void LCD_ILI9341V_Parameter(uint8_t data8); //2014-06-10 V1.04更换夏瑞的LCD屏，驱动由ILI9328变更为ILI9341
void LCD_ILI9341V_CMD(uint8_t data8);	
void LCD_CtrlWrite_ILI9328(uint16_t i);	
void LCD_Initial(void);      //ILI9341初始化 //2014-06-10 V1.04更换夏瑞的LCD屏，驱动由ILI9328变更为ILI9341

void Draw_Rectangle(uint8_t Start_X, uint16_t Start_Y,uint8_t End_X, uint16_t End_Y,uint8_t color, uint8_t LineW);
void LCD_CrtlWrite_Area(uint8_t SX,uint8_t EX,uint16_t SY,uint16_t EY);
void LCD_CtrlWrite_Color(uint8_t color);
void Draw_Rectangle_Real(uint8_t Start_X, uint16_t Start_Y,uint8_t End_X, uint16_t End_Y,uint8_t color);

void DISP_TEMP_30X56(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_TEMP_38X64(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
extern void DISP_RH_17X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_TIME_10X16(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void Disp_YouChuang(uint8_t  X, uint16_t Y,uint8_t color); //79,62
void Disp_WuChuang(uint8_t  X, uint16_t Y,uint8_t color); //79,62
void DISP_ICO_32X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_ICO_40X40(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_ICO_52X64(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_DHM(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_VHB80_56X40(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_HEAT_36X24(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_CQK25X24(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_RTD28X24(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_JQK41X16(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_RH14X24(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);
void DISP_VHB80_PIC(uint8_t color);                       //显示仪器图形
void DISP_FNT10X24(uint8_t  X, uint16_t Y,uint8_t NUM,uint8_t color);


void Disp_ShangBiao1(uint8_t  X, uint16_t Y,uint8_t color);
void Disp_ShangBiao2(uint8_t  X, uint16_t Y,uint8_t color);
void Disp_ShangBiao3(uint8_t  X, uint16_t Y,uint8_t color);
void Disp_ShangBiao4(uint8_t  X, uint16_t Y,uint8_t color);

void Disp_ShangBiao6(uint8_t  X, uint16_t Y,uint8_t color);
void Disp_ShangBiao7(uint8_t  X, uint16_t Y,uint8_t color);
void Disp_ShangBiao8(uint8_t  X, uint16_t Y,uint8_t color);
void DISP_FNT8X16(u16  X, u16 Y,u8 NUM,u8 mode,u8 color);
void DISP_FNT12X24(u16  X, u16 Y,u8 NUM,u8 mode,u8 color);
void LCD_ShowChar(u16 x,u16 y,u8 size,u8 num,u8 mode,u8 color); 
void LCD_ShowString(u16 x, u16 y,u16 height_x,u16 width_y,const u8 *p,u8 mode,u8 color);
void LCD_ShowxNum(u16 x,u16 y,u8 size,u8 len,u32 num,u8 mode,u8 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u8 color);//画圆

void DrawWorkWindows(void);
void Refresh_Work_Mode(void);
void  LCD_Show_Verion(void);//显示版本号 




#define LCD_RS  P15  //数据/命令切换
#define LCD_RST  P46       //复位
#define LCD_WR   P14    //写控制
#define LCD_RD   P42    //读控制
#define LCD_CS  P16

#define LCD_RS_H  	(P15=(bit)1)
#define LCD_RS_L  	(P15=(bit)0)

#define LCD_RD_H  	(P42=(bit)1)
#define LCD_RD_L  	(P42=(bit)0)

#define LCD_WR_H  	(P14=(bit)1)
#define LCD_WR_L  	(P14=(bit)0)

#define LCD_RST_H  	(P46=(bit)1)
#define LCD_RST_L  	(P46=(bit)0)

#define LCD_CS_H  	(P16=(bit)1)
#define LCD_CS_L  	(P16=(bit)0)

#define LCD_1DIR_H 	(P17=(bit)1)//高电平允许数据从A（3V）送到B（5V）
#define LCD_1DIR_L 	(P17=(bit)0)//低电平允许数据从B（5V）送到A（3V）


#define LCD_LIGHT_OPEN 		(P40=(bit)1)
#define LCD_LIGHT_CLOSE 	(P40=(bit)0)

#define	LCD_PORT  P0
#define LCD_DATA_PORT P0



#define RED       	   0x07FF    //红色
#define GREEN     	   0xF81F    //绿色
#define BLUE      	   0xFFE0    //蓝色
#define WHITE     	   0x0000    //白色
#define BLACK     	   0xFFFF    //黑色
#define GRAY     	   	 0x7BEF    //灰色  0x630c 
#define YELLOW     	   0x001F    //黄色
#define BRIGHT_BLUE    0xF98B    //青色
#define AIR_BLUE       0x81C0    //天蓝
#define PURPLE    	   0x07E0    //紫色	 
#define BLACKRED  	   0xF210    //黑红色	 0xF210 

#define  RED18           0
#define  GREEN18         1
#define  BLUE18          2
#define  WHITE18         3
#define  BLACK18         4
#define  GRAY18          5
#define  YELLOW18        6    //
#define  BRIGHT_BLUE18   7    //
#define  AIR_BLUE18      8    //
#define  PURPLE18    	 9   //
#define  BLACKRED18    	 10   //

#define  POS_Select_YC_X         79
#define  POS_Select_YC_Y         62
#define  POS_Select_WC_X         79
#define  POS_Select_WC_Y         186
#define  POS_YWC_X               130+5         //有无创收图标位置145
#define  POS_YWC_Y               230


#define  ICO_TEMP    0
#define  ICO_SHIDU   1
#define  ICO_TIME    2
#define  ICO_DU      3
#define  ICO_PER     4
#define  ICO_CQK     5

#define  POS_ICO_TEMP_X      168+5  //175,168
#define  POS_ICO_TEMP_Y      8
#define  POS_ICO_SHIDU_X     103+5
#define  POS_ICO_SHIDU_Y     8
#define  POS_ICO_TIME_X      19
#define  POS_ICO_TIME_Y      8
#define  POS_SYSTIME_X   16
#define  POS_SYSTIME_Y   216
#define  POS_SYSDATE_X   43
#define  POS_SYSDATE_Y   208
#define  POS_RT_TEMP_X       (POS_ICO_TEMP_X-4) //7
#define  POS_RT_TEMP_Y       60
#define  POS_RT_RH_X         POS_ICO_SHIDU_X 
#define  POS_RT_RH_Y         65
#define  POS_DMH_X           43
#define  POS_DMH_Y           58
#define  POS_VHB80_X    16
#define  POS_VHB80_Y    138
#define  POS_HEAT_X    POS_VHB80_X+40   //58
#define  POS_HEAT_Y    POS_VHB80_Y+14   //152
#define  POS_CQK_X     POS_HEAT_X+24    //82
#define  POS_CQK_Y     POS_HEAT_Y+19    //171
#define  POS_XQGX_X1   POS_CQK_X+15      //97
#define  POS_XQGX_Y1   POS_CQK_Y+25
#define  POS_XQGS_X1   POS_CQK_X+22
#define  POS_XQGS_Y1   POS_CQK_Y+25
#define  POS_XQGX_X2   POS_XQGX_X1+1
#define  POS_XQGX_Y2   POS_CQK_Y+93
#define  POS_XQGS_X2   POS_XQGS_X1+1
#define  POS_XQGS_Y2   POS_CQK_Y+93
#define  POS_RTD_X     POS_CQK_X
#define  POS_RTD_Y     POS_CQK_Y+93
#define  POS_CQGX_X1   POS_CQK_X
#define  POS_CQGX_Y1   POS_CQK_Y+29
#define  POS_CQGS_X1   POS_CQK_X+7
#define  POS_CQGS_Y1   POS_CQK_Y+29
#define  POS_CQGX_X2   POS_CQGX_X1+1
#define  POS_CQGX_Y2   POS_CQK_Y+93
#define  POS_CQGS_X2   POS_CQGS_X1+1
#define  POS_CQGS_Y2   POS_CQK_Y+93 
#define  POS_JQK_X     POS_CQK_X
#define  POS_JQK_Y     POS_HEAT_Y-29
#define  POS_SCREEN_X 75
#define  POS_SCREEN_Y 80

#define  POS_ALARM_COL_X     220  //报警的颜色块
#define  POS_ALARM_COL_Y     250  //

//开机选择使用上一次用户的设定或默认设定 提示位置
#define  Load_User_Pre_Set_X      192 //
#define  Load_User_Pre_Set_Y      53 //
#define  Load_Fac_Set_X      			72 //
#define  Load_Fac_Set_Y      			(Load_User_Pre_Set_Y+6) //  


#endif



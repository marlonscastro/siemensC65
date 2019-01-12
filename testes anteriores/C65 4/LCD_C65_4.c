/*
 * S6B33B2.c
 *
 * Created: 12.07.2011 17:11:09
 *  Author: Sven Fabricius
 */

#include "S6B33B2.h"
#include <string.h>
#include "LuciaConsole_8x11.h"

int fY = 0;
int fX = 0;
uint16_t Back;
uint16_t Front;

void LCD_OutByteRS0(uint8_t Data)
{
        LCD_Status_Low(LCD_RS);
        LCD_Data = Data;
        LCD_Status_Low(LCD_WRB);
        LCD_Status_High(LCD_WRB);
}

void LCD_OutByteRS1(uint8_t Data)
{
        LCD_Status_High(LCD_RS);
        LCD_Data = Data;
        LCD_Status_Low(LCD_WRB);
        LCD_Status_High(LCD_WRB);
}

void LCD_WCom( int data, ... )
{
        va_list zeiger;
        va_start(zeiger,data);
        for(int i = 1; i <= data; i++)
                LCD_OutByteRS0(va_arg(zeiger,int));
        va_end(zeiger);
}

void LCD_BData(uint8_t dataH, uint8_t dataL)
{
        LCD_OutByteRS1(dataH);
        LCD_OutByteRS1(dataL);
}      

void S6B33B2_INIT(uint16_t Color)
{
        LCD_Data_Control = 0xFF;
       
        LCD_Status_Control = (1 << LCD_CS1B) | (1 << LCD_WRB) | (1 << LCD_RDB) | (1 << LCD_RS) | (1 << LCD_RSTB);
       
        LCD_Data = 0x00;
        LCD_Status = 0x00;

        _delay_ms(1);
        LCD_Status_High(LCD_RSTB);
        _delay_ms(1);
        LCD_Status_Low(LCD_RSTB);
        _delay_ms(1);
        LCD_Status_High(LCD_RSTB);
       
       
        LCD_Status_Low(LCD_CS1B);
        LCD_Status_High(LCD_WRB);
        LCD_Status_High(LCD_RDB);
        LCD_Status_Low(LCD_RS);

        LCD_WCom(1, STANDBY_MODE_OFF);
        LCD_WCom(2, OSCILLATION_MODE_SET, OMS_OSC);
        LCD_WCom(2, DCDC_AND_AMP_ON_OFF_SET, DAAOOS_DCDC1);
        LCD_WCom(2, DCDC_AND_AMP_ON_OFF_SET, DAAOOS_DCDC1 | DAAOOS_AMP);
        LCD_WCom(2, DCDC_AND_AMP_ON_OFF_SET, DAAOOS_DCDC1 | DAAOOS_DCDC2 | DAAOOS_AMP );
        LCD_WCom(2, DCDC_AND_AMP_ON_OFF_SET, DAAOOS_DCDC1 | DAAOOS_DCDC2 | DAAOOS_DCDC3 | DAAOOS_AMP);
        LCD_WCom(2, DRIVER_OUTPUT_MODE_SET,/* 0x01 | */ DOMS_SDIR);
        LCD_WCom(2, DC_DC_SELECT, DDS_DC1_X20);
        LCD_WCom(2, DCDC_CLOCK_DIVISION_SET, DCDS_DIV1_16);
        LCD_WCom(2, TEMPERATURE_COMPENSATION_SET, TCS_05);
        LCD_WCom(2, CONTRAST_CONTROL1, 0xD0);
        LCD_WCom(2, ADDRESSING_MODE_SET, 0x00);
        LCD_WCom(2, ROW_VECTOR_MODE_SET, 0x0E);
        LCD_WCom(2, N_LINE_INVERSION_SET, 0x0D);
        LCD_WCom(2, FRAME_FREQUENCY_CONTROL, 0x00);
        LCD_WCom(2, ENTRY_MODE_SET, 0x81);
        LCD_WCom(3, X_ADDRESS_AREA_SET, 0x02, 0x81);
        LCD_WCom(3, Y_ADDRESS_AREA_SET, 0x02, 0x81);
        LCD_WCom(1, 0x61);
        LCD_WCom(1, DISPLAY_ON);
       
        for(int i = 0; i <= 16383; i++)
        {
                LCD_WData(Color);
        }              
}

void Display_Test(void)
{
        LCD_WCom(3, X_ADDRESS_AREA_SET, 0x02, 0x81);
        LCD_WCom(3, Y_ADDRESS_AREA_SET, 0x02, 0x81);

        for(int i = 0; i <= 31;i++)
        {
                for(int j = 0; j < 512; j++)
                {
                        LCD_WData(to_rgb565(255,0,0));
                }                      
                       
                for(int j = 0; j < 512; j++)
                {
                        LCD_WData(to_rgb565(0,255,0));
                }                      

                for(int j = 0; j < 512; j++)
                {
                        LCD_WData(to_rgb565(0,0,255));
                }                      
        }
}

void setFillArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
        uint8_t StartX = 2 + (129 - Height - Y - 1);
        uint8_t EndX = 2 + (129 - Y);
       
        uint8_t StartY = 2 + X;
        uint8_t EndY = 2 + (X + Width - 1);
       
        LCD_WCom(3, X_ADDRESS_AREA_SET, StartX, EndX);
        LCD_WCom(3, Y_ADDRESS_AREA_SET, StartY, EndY);
}

void fillRect(uint16_t Color, uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
        setFillArea(X, Y, Width, Height);      
        for(int i = 0; i < (Width * Height);i++)
                LCD_WData(Color);
}

void drawRect(uint16_t Color, uint8_t Line, uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
        fillRect(Color, X, Y, Width, Line); // TOP
        fillRect(Color, X, (Y + Line), Line, (Height - (Line * 2))); // LEFT
        fillRect(Color, (X + Width - Line), (Y + Line), Line, (Height - (Line * 2))); // RIGHT
        fillRect(Color, X, (Y + Height - Line), Width, Line); // BOTTOM
}

void putChar(uint16_t BackColor, uint16_t FrontColor, char C, uint8_t X, uint8_t Y)
{
        setFillArea(X, Y, 8, 11);
        uint8_t data[11];
        eeprom_read_block(data, &LuciaConsole[(uint8_t)C - 32], 11);
        for (int8_t i = 10; i > 0; i--)
        {
                uint8_t Line = data[i];
                for (int8_t j = 0; j < 8; j++)
                {
                        if (Line & 0x80)
                        {
                                LCD_WData(BackColor);
                        }
                        else
                        {
                                LCD_WData(FrontColor);
                        }
                        Line = Line << 1;
                }
        }      
}

void setXY(int X, int Y)
{
        fX = X;
        fY = Y;
}

void setFrontColor(uint16_t Color)
{
        Front = Color;
}

void setBackColor(uint16_t Color)
{
        Back = Color;
}

void putText(uint16_t BackColor, uint16_t FrontColor, char *Text, uint8_t Length, uint8_t X, uint8_t Y)
{
        for (uint8_t i = 0; i < Length; i++)
        {
                putChar(BackColor,FrontColor,Text[i],X,Y);
                X += 7;
        }      
}

void writeLn(char *Text)
{
        putText(Back, Front, Text, strlen(Text), fX, fY);
        fY+=12;
}

void clear(void)
{
        fillRect(Back,0,0,128,128);
}

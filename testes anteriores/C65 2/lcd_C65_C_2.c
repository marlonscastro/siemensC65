#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"
#include <avr/interrupt.h>
#include <stdio.h>

#define OSC 0x02 // Oscillation mode set
#define DRVOUT 0x10 // Driver output mode set
#define DCDCSEL 0x20 // DC-DC select
#define BIAS 0x22 // Bias set
#define DCDCCLK 0x24 // DCDC clock division set
#define DCDCCTL 0x26 // DCDC and AMP on/off set
#define TEMPCOM 0x28 // Temperature compensation set
#define CONT1 0x2A // Contrast control 1
#define CONT2 0x2B // Contrast control 2
#define STBYOFF 0x2C // Standby off
#define STBYON 0x2D // Standby on
#define ADRMODE 0x30 // Adressing mode set
#define ROWVEC 0x32 // Row vector mode set
#define NLINV 0x34 // N-line invertion set
#define FRAFRE 0x36 // Frame frequency control
#define ENTRY 0x40 // Entry mode set
#define XSET 0x42 // X-adress area set
#define YSET 0x43 // Y-adress area set
#define DISON 0x51 // Display on
#define DISOFF 0x50 // Display off
#define DISPATT 0x53
#define PARTDISMODE 0x55
/*
CS  PB0
RES PB1
RS  PB2
CLK PB7
DAT PB5
*/

#define CS0 PORTB &=! 0x01;
#define CS1 PORTB |=  0x01;
#define RS0 PORTB &=! 0x04;
#define RS1 PORTB |=  0x04;
#define CLK0 PORTB &=!0x80;
#define CLK1 PORTB |= 0x80;
#define SDA0 PORTB &=!0x20;
#define SDA1 PORTB |= 0x20;
#define RESET0 PORTB &=! 0x02;
#define RESET1 PORTB |=  0x02;

void shiftBits(unsigned char b){
   SPDR = b;
   while(!(SPSR & (1<<SPIF)));
}
void lcdcommand(unsigned char dta){
   CS0
   RS0
   shiftBits(dta);
   CS1
}
void lcdcommand16(unsigned char cmd, unsigned char param){
   CS0
   RS0
   shiftBits(cmd);
   shiftBits(param);
   CS1
}



void lcddata(unsigned char dta){
   CS0
   RS1
   shiftBits(dta);
   CS1
}

void lcdfill(char color){
   unsigned int i;
   CS0
   RS0
   shiftBits(XSET);
   shiftBits(0);
   shiftBits(131);
   shiftBits(YSET);
   shiftBits(0);
   shiftBits(131);
   RS1
   for(i = 0; i < (132*132); i++)
   shiftBits(color);
   CS1
}

void LCD_INIT(void){
   // Hardware reset
   CLK1
   SDA1
   RS1
   CS1
   RESET0
   _delay_us(1);
   RESET1
   _delay_us(1);
   
   lcdcommand(STBYOFF); // STANDBY off
   lcdcommand16(OSC,0x01);
   lcdcommand16(DCDCCTL,0x0F);
   lcdcommand16(DRVOUT,0x30);
   lcdcommand16(DCDCSEL,0x01);
   lcdcommand16(BIAS,0x01);
   lcdcommand16(DCDCCLK,0x01);
   lcdcommand16(TEMPCOM,0x00);
   lcdcommand16(ADRMODE,0x40);
   lcdcommand16(ROWVEC,0x01);
   lcdcommand16(NLINV,0x82);
   lcdcommand16(ENTRY,0x00);
   lcdcommand16(0x00,0x80);
   lcdcommand(DISON);
   
   lcdfill(0xAF);
   //lcdcommand(DISON); // Disp On
}

int main(void){
   DDRB = 0xFF;                        
   lcd_init();
   LCD_DISPLAY(LCDDISPLAY);
   SPCR |= (1<<SPE) |(1<<MSTR) |(1<<CPOL) |(1<<CPHA) |(1<<SPR1);
   LCD_INIT();
   while(1){
      // Codigo aqui
   }
}

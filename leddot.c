/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.3 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : ¶þÏàËÄÏß²½½øµç»úÖÆ×÷µÄ¶¨Ê±Î¹ÓãÆ÷
Version : 1.0
Date    : 2012-2-15
Author  : txz                            
Company : TC                            
Comments: 


Chip type           : ATmega8L
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <stdlib.h>
#include "ds1302.h"

#define run_state_LED PORTD.4    /*ÔËÐÐ×´Ì¬LEDÖ¸Ê¾µÆ£¬ÁÁ1ÃëÃð1Ãë*/
#define feed_light PORTC.1       /*Óã¸×ÕÕÃ÷µÆ*/  
#define alertNumber 3            /*Ã¿ÌìÎ¹Óã´ÎÊý*/

struct alertInfo{     //Î¹ÓãµÄÊ±¼äºÍÎ¹Ê³Á¿
  uchar hour;
  uchar minute;
  uchar turns;       //²½½øµç»úÐý×ª´ÎÊý£¬Ô½´óÍ¶Î¹µÄËÇÁÏÔ½¶à
  uchar light_off_hour;
  uchar light_off_minute;
};

// Declare your global variables here

struct alertInfo feed[3]={{7,20,4,7,30},{11,30,4,7,40},{22,25,4,22,35}};

bit manual_light_on=0;    //ÊÖ¶¯´ò¿ªÓã¸×ÕÕÃ÷µÆ±êÖ¾£¬1-ÊÖ¶¯£¬0-·ñ
uchar flag;
uchar curminute;   
uchar anti_shake;       //Ïû³ý¶¶¶¯


uchar moto_direct=0;  //µç»úÐý×ª·½Ïò
uchar moto_speed=90;
uchar np; 
//²½½øµç»úÔËÐÐÊý¾Ý±í 
//const uchar motortb[]={0x11,0x99,0x88,0xcc,0x44,0x66,0x22,0x33}; 
const uchar motortb[]={0x01,0x09,0x08,0x0c,0x04,0x06,0x02,0x03}; 
void delay(uchar t); 
void a_step(uchar d,uchar t);    //µç»ú×ªÒ»²½
void a_turn(uchar d,uchar t);    //µç»ú×ªÒ»È¦
void alert_compare();         //ÄÖÖÓ±È½Ï       
void key_scan();
void key_press(uchar key);

// Timer 1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
  // Reinitialize Timer 1 value
  TCNT1H=0x85;
  TCNT1L=0xED;
  // Place your code here
  run_state_LED=!run_state_LED;

}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=P State6=P State5=P State4=P State3=P State2=P State1=P State0=P 
PORTB=0xFF;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=0 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTD=0x00;
DDRD=0xFF;
//run_state_LED ÔËÐÐLEDÖ¸Ê¾µÆ

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 31.250 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x04;
TCNT1H=0x85;
TCNT1L=0xED;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x04;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei")

moto_direct=0;
flag=0;
PORTD=0x04;           
np=4; 

PORTB.2=0;
run_state_LED=1;      //ÔËÐÐÖ¸Ê¾µÆÁÁ

nowtime.nowyear=12; 
nowtime.nowmonth=2; 
nowtime.nowday=19; 
nowtime.nowhour=22; 
nowtime.nowminute=19; 
nowtime.nowsecond=30; 
nowtime.nowweek=7;

ds1302_write_time();
curminute=nowtime.nowminute;
while(1)
{

  ds1302_read_time();
 
  key_scan();
  
  alert_compare();   //ÄÖÖÓ
        

}

}


void delay(uchar t)// Ã¿²½ÑÓÊ±µÄ×Ó³ÌÐò  
{ 
 uchar i; 
 uint j; 
  for (i=0;i<t;i++) 
     for (j=0;j<1000;j++); 
}
 
void a_step(uchar d,uchar t)  //²½½øµç»ú×ßÒ»²½ d=0 Õý×ª d=1 ·´×ª  t Ô½´ó×ßµÃÔ½Âý  
{                              
  
  if (d&0x01)                   
     { 
    if (np==0) 
       np=7; 
    else np--; 
   } 
  else  
     { 
    if (np==7) 
       np=0; 
    else np++; 
   } 
  PORTD=(PORTD & 0xf0) | motortb[np]; 
    delay(t); 
}
 
void a_turn(uchar d,uchar t)// ²½½øµç»ú×ß1/4È¦  
{ 
   uchar i; 
   for (i=0;i<10;i++) 
      a_step(d,t); 
}   

void alert_compare()
{
    uchar i,j;
    for (i=0;i<3;i++) {
        //Î¹ÓãÅÐ¶Ï
        if(feed[i].hour==nowtime.nowhour && feed[i].minute==nowtime.nowminute && curminute!=nowtime.nowminute){
            //feed_light=1;       //¿ªÓã¸×ÕÕÃ÷µÆ
            for(j=0;j<feed[i].turns;j++){
                a_turn(moto_direct,moto_speed);
            }
            curminute=nowtime.nowminute;
        }
        
        //¹ØÓã¸×ÕÕÃ÷µÆÅÐ¶Ï
        if(feed[i].light_off_hour==nowtime.nowhour && feed[i].light_off_minute==nowtime.nowminute && !manual_light_on){
            //feed_light=0;    //¹Ø±ÕÓã¸×ÕÕÃ÷µÆ
        }
    }
}   

void key_scan()
{
    uchar i;
    uchar scan=0x01;
    for(i=0;i<8;i++)
    {
         if((~PINB) & scan )
         {
             key_press(i);
         }
         scan<<=1;
    }
}    

void key_press(uchar key)
{
    if(anti_shake<180)     //Ïû³ý°´¼ü¶¶¶¯
    {
        anti_shake++;
        return;
    }
    else
        anti_shake=0;
    
    switch (key) {
    case 0:
        break;
    
    case 1:
        break;
        
    case 2:
        break;
        
    case 3:
        break;
        
    case 4:
        break;
        
    case 5:
        break;
        
    case 6:
        a_turn(moto_direct,moto_speed);    //Í¶Î¹Ò»´ÎËÇÁÏ        
        break;
        
    case 7:
        feed_light=!feed_light;   //¿ª/¹ØÓã¸×ÕÕÃ÷µÆ
        
        if(feed_light)
            manual_light_on=1;
        else
            manual_light_on=0;

    } 


}
#define uchar unsigned char
#define uint unsigned int

#define ds1302_rst   7                     /*!PORTD7 DS1302ʵʱʱ�Ӹ�λ������ */ 
#define ds1302_io   6                      /*!PORTD6 DS1302ʵʱʱ������������ */    
#define ds1302_sclk  5                      /*!PORTD5 DS1302ʵʱʱ��ʱ�������� */ 

#define set_ds1302_rst_ddr()  DDRD|=1<<ds1302_rst 
#define set_ds1302_rst()        PORTD|=1<<ds1302_rst 
#define clr_ds1302_rst()        PORTD&=~(1<<ds1302_rst) 

#define set_ds1302_io_ddr()   DDRD|=1<<ds1302_io 
#define clr_ds1302_io_ddr()    DDRD&=~(1<<ds1302_io) 
#define set_ds1302_io()          PORTD|=1<<ds1302_io 
#define clr_ds1302_io()          PORTD&=~(1<<ds1302_io) 
#define in_ds1302_io()          PIND&(1<<ds1302_io) 

#define set_ds1302_sclk_ddr() DDRD|=1<<ds1302_sclk 
#define set_ds1302_sclk()        PORTD|=1<<ds1302_sclk 
#define clr_ds1302_sclk()         PORTD&=~(1<<ds1302_sclk) 

#define ds1302_sec_reg        0x80 
#define ds1302_min_reg        0x82 
#define ds1302_hr_reg           0x84 
#define ds1302_date_reg         0x86 
#define ds1302_month_reg       0x88 
#define ds1302_day_reg        0x8a 
#define ds1302_year_reg        0x8c 
#define ds1302_control_reg      0x8e 
#define ds1302_charger_reg    0x90 
#define ds1302_clkburst_reg   0xbe  

const unsigned char WeekTab[] = { 
   //���������ڱ� 
  (3 << 5) + 31,///1�� 
  (6 << 5) + 29,///2�� 
  (0 << 5) + 31,///3�� 
  (3 << 5) + 30,///4�� 
  (5 << 5) + 31,//5�� 
  (1 << 5) + 30,//6�� 
  (3 << 5) + 31,//7�� 
  (6 << 5) + 31,//8�� 
  (1 << 5) + 30,//9�� 
  (4 << 5) + 31,//10�� 
  (0 << 5) + 30,//11�� 
  (2 << 5) + 31 //12�� 
}; 

struct timechar 
{ 
  uchar nowyear; 
  uchar nowmonth; 
  uchar nowday; 
  uchar nowhour; 
  uchar nowminute; 
  uchar nowsecond; 
  uchar nowweek; 
}; 
struct timechar nowtime; //�����ʱ��    

uchar BcdToBin(uchar val) ;
uchar BinToBcd(uchar val);
void ds1302_write(uchar reg, uchar data);
uchar ds1302_read(uchar reg); 
uchar check_ds1302(void); 
void ds1302_read_time(void); 
unsigned char WeekDay20(unsigned char y, unsigned char m, unsigned char d);
void ds1302_write_time(void); 

uchar BcdToBin(uchar val) 
{ 
    val = (val >> 4) *10+(val &0x0f); ///��BCD��ת��Ϊ10������ 
    return val; ///����10������ 
} 

uchar BinToBcd(uchar val) 
{ 
    return ((val / 10) *16+val % 10); 
} 

void ds1302_writedata(uchar reg, uchar data) 
{ 
   uchar i = 0; 
   set_ds1302_io_ddr(); 
        //nop()    
     //nop()  
   clr_ds1302_rst(); 
    //nop()  
    //nop()  
   clr_ds1302_sclk(); 
    //nop()  
    //nop()  
   set_ds1302_rst(); 
    //nop()  
    //nop()  
   for (i = 8; i > 0; i--) 
   { 
      if (reg &0x01) 
      { 
          set_ds1302_io(); 
      } 
      else 
      { 
          clr_ds1302_io(); 
      } 
       //nop()  
       //nop()  
      set_ds1302_sclk(); 
       //nop()  
       //nop()  
      clr_ds1302_sclk(); 
       //nop()  
       //nop()  
      reg >>= 1; 
    } 
    for (i = 8; i > 0; i--) 
    { 
      if (data &0x01) 
      { 
          set_ds1302_io(); 
      } 
      else 
      { 
          clr_ds1302_io(); 
      } 
       //nop()  
       //nop()  
      set_ds1302_sclk(); 
       //nop()  
       //nop()  
      clr_ds1302_sclk(); 
       //nop()  
       //nop()  
      data >>= 1; 
    } 
    clr_ds1302_rst(); 
     //nop()  
     //nop()  
    clr_ds1302_io_ddr(); 
     //nop()  
     //nop()  
}
   
uchar ds1302_readdata(uchar reg) 
{ 
  uchar data = 0; 
  uchar i = 0; 
  reg += 1; ///����־ 
  set_ds1302_io_ddr(); 
   //nop()  
   //nop()  
  clr_ds1302_rst(); 
   //nop()  
   //nop()  
  clr_ds1302_sclk(); 
   //nop()  
   //nop()  
  set_ds1302_rst(); 
   //nop()  
   //nop()  
  for (i = 8; i > 0; i--) 
  { 
     if (reg &0x01) 
     { 
        set_ds1302_io(); 
     } 
     else 
     { 
        clr_ds1302_io(); 
     }  
      //nop()  
      //nop()  
     set_ds1302_sclk(); 
      //nop()  
      //nop()  
     clr_ds1302_sclk(); 
      //nop()  
      //nop()  
     reg >>= 1; 
  } 
  clr_ds1302_io_ddr(); 
   //nop()  
   //nop()  
  for (i = 8; i > 0; i--) 
  { 
    data >>= 1; 
    if (in_ds1302_io()) 
    { 
      data |= 0x80; 
    } 
     //nop()  
     //nop()  
    set_ds1302_sclk(); 
     //nop()  
     //nop()  
    clr_ds1302_sclk(); 
     //nop()  
     //nop()  
   } 
   clr_ds1302_rst(); 
    //nop()  
    //nop()  
   return (data); 
} 

uchar check_ds1302(void) 
{ 
  ds1302_writedata(ds1302_control_reg, 0x80); 
  if (ds1302_readdata(ds1302_control_reg) == 0x80) 
  { 
    return 1; 
  } 
  return 0; 
} 

void ds1302_read_time(void) 
{ 
  uchar tmphour; 
  nowtime.nowyear = BcdToBin(ds1302_readdata(ds1302_year_reg)); ///�� 
  nowtime.nowmonth = BcdToBin(ds1302_readdata(ds1302_month_reg)); ///�� 
  nowtime.nowday = BcdToBin(ds1302_readdata(ds1302_date_reg)); ///�� 
  nowtime.nowweek = BcdToBin(ds1302_readdata(ds1302_day_reg)); ///�� 
  tmphour = ds1302_readdata(ds1302_hr_reg); ///ʱ 
  nowtime.nowhour = (tmphour &0b00100000) / 32 * 20+(tmphour &0x10) / 16 * 10+(tmphour &0x0F); 
  nowtime.nowminute = BcdToBin(ds1302_readdata(ds1302_min_reg)); ///�� 
  nowtime.nowsecond = BcdToBin(ds1302_readdata(ds1302_sec_reg)); ///�� 
} 


/*!------------------------------------------------------------------------------ 
                                2000��~2099�������㷨 
-------------------------------------------------------------------------------*/ 
unsigned char WeekDay20(unsigned char y, unsigned char m, unsigned char d) 
{ 
unsigned char week, day; 
  day = WeekTab[m - 1];///�±� 
  week = day >> 5;///�������� 
  day &= 0x1f;//������ 
  if ((m < 3) && (y & 0x03)) 
      {///ƽ�� 
    if (m == 2) day--;///ƽ�������� 
    week++;///ƽ���±�+1 
  } 
  y = y + (y >> 2);//��+��/4 
  week = (week +  y + d + 2) % 7;///(����=��+��/4+�±�+2��)%7 
  return week;///�������� 
} 

void ds1302_write_time(void) 
  { 
  uchar bcdyear; 
  uchar bcdmonth; 
  uchar bcdday; 
  uchar bcdhour; 
  uchar bcdminute; 
  uchar bcdsecond; 
  uchar bcdweek; 

  bcdyear = BinToBcd(nowtime.nowyear); 
  bcdmonth = BinToBcd(nowtime.nowmonth); 
  bcdday = BinToBcd(nowtime.nowday); 
  bcdhour = BinToBcd(nowtime.nowhour); 
  bcdminute = BinToBcd(nowtime.nowminute); 
  bcdsecond = BinToBcd(nowtime.nowsecond); 
    
  nowtime.nowweek=WeekDay20(nowtime.nowyear,nowtime.nowmonth,nowtime.nowday); 
  bcdweek = BinToBcd(nowtime.nowweek); 
    
  ds1302_writedata(ds1302_control_reg, 0x00); ///�ر�д���� 
  ds1302_writedata(ds1302_sec_reg, 0x80); ///��ͣ 
  ds1302_writedata(ds1302_day_reg, bcdweek); ///��    
  ds1302_writedata(ds1302_charger_reg, 0xa9); ///������ 
  ds1302_writedata(ds1302_year_reg, bcdyear); ///�� 
  ds1302_writedata(ds1302_month_reg, bcdmonth); ///�� 
  ds1302_writedata(ds1302_date_reg, bcdday); ///�� 
  ds1302_writedata(ds1302_hr_reg, bcdhour); ///ʱ 
  ds1302_writedata(ds1302_min_reg, bcdminute); ///�� 
  ds1302_writedata(ds1302_sec_reg, bcdsecond); ///�� 
  ds1302_writedata(ds1302_control_reg, 0x80); ///��д���� 
  }
#define uchar unsigned char
#define uint unsigned int

#define ds1302_rst   7                     /*!PORTD7 DS1302实时时钟复位线引脚 */ 
#define ds1302_io   6                      /*!PORTD6 DS1302实时时钟数据线引脚 */    
#define ds1302_sclk  5                      /*!PORTD5 DS1302实时时钟时钟线引脚 */ 

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
   //闰年月星期表 
  (3 << 5) + 31,///1月 
  (6 << 5) + 29,///2月 
  (0 << 5) + 31,///3月 
  (3 << 5) + 30,///4月 
  (5 << 5) + 31,//5月 
  (1 << 5) + 30,//6月 
  (3 << 5) + 31,//7月 
  (6 << 5) + 31,//8月 
  (1 << 5) + 30,//9月 
  (4 << 5) + 31,//10月 
  (0 << 5) + 30,//11月 
  (2 << 5) + 31 //12月 
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
struct timechar nowtime; //这里存时间    

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
    val = (val >> 4) *10+(val &0x0f); ///将BCD码转换为10进制数 
    return val; ///返回10进制数 
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
  reg += 1; ///读标志 
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
  nowtime.nowyear = BcdToBin(ds1302_readdata(ds1302_year_reg)); ///年 
  nowtime.nowmonth = BcdToBin(ds1302_readdata(ds1302_month_reg)); ///月 
  nowtime.nowday = BcdToBin(ds1302_readdata(ds1302_date_reg)); ///日 
  nowtime.nowweek = BcdToBin(ds1302_readdata(ds1302_day_reg)); ///周 
  tmphour = ds1302_readdata(ds1302_hr_reg); ///时 
  nowtime.nowhour = (tmphour &0b00100000) / 32 * 20+(tmphour &0x10) / 16 * 10+(tmphour &0x0F); 
  nowtime.nowminute = BcdToBin(ds1302_readdata(ds1302_min_reg)); ///分 
  nowtime.nowsecond = BcdToBin(ds1302_readdata(ds1302_sec_reg)); ///秒 
} 


/*!------------------------------------------------------------------------------ 
                                2000年~2099年星期算法 
-------------------------------------------------------------------------------*/ 
unsigned char WeekDay20(unsigned char y, unsigned char m, unsigned char d) 
{ 
unsigned char week, day; 
  day = WeekTab[m - 1];///月表 
  week = day >> 5;///月星期数 
  day &= 0x1f;//月天数 
  if ((m < 3) && (y & 0x03)) 
      {///平年 
    if (m == 2) day--;///平年月天数 
    week++;///平年月表+1 
  } 
  y = y + (y >> 2);//年+年/4 
  week = (week +  y + d + 2) % 7;///(星期=年+年/4+月表+2日)%7 
  return week;///返回星期 
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
    
  ds1302_writedata(ds1302_control_reg, 0x00); ///关闭写保护 
  ds1302_writedata(ds1302_sec_reg, 0x80); ///暂停 
  ds1302_writedata(ds1302_day_reg, bcdweek); ///周    
  ds1302_writedata(ds1302_charger_reg, 0xa9); ///涓流充电 
  ds1302_writedata(ds1302_year_reg, bcdyear); ///年 
  ds1302_writedata(ds1302_month_reg, bcdmonth); ///月 
  ds1302_writedata(ds1302_date_reg, bcdday); ///日 
  ds1302_writedata(ds1302_hr_reg, bcdhour); ///时 
  ds1302_writedata(ds1302_min_reg, bcdminute); ///分 
  ds1302_writedata(ds1302_sec_reg, bcdsecond); ///秒 
  ds1302_writedata(ds1302_control_reg, 0x80); ///打开写保护 
  }
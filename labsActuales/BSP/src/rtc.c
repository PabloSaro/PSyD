
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10);  //conversion Bcd a Binario
#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10); //conversion Binario a Bcd
extern void isr_TICK_dummy( void );

void rtc_init( void )
{
    TICNT   = (1<<7);//deshabilita las interrupciones del rtc
    RTCALM  = 0x0;
    RTCRST  = 0x0;
        
    RTCCON  |= 1;
    
    BCDYEAR = 00010011; // año 2013
    BCDMON  = 0001;  // enero
    BCDDAY  = 0011;  // martes
    BCDDATE = 0001;  // dia 1
    BCDHOUR = 0000;  // 00h
    BCDMIN  = 0000;  // 00 min
    BCDSEC  = 0000; // 00 segs

    ALMYEAR = 0x0;
    ALMMON  = 0x0;
    ALMDAY  = 0x0;
    ALMHOUR = 0x0;
    ALMMIN  = 0x0;
    ALMSEC  = 0x0;

    RTCCON &= ~(0x1);
}

void rtc_puttime( rtc_time_t *rtc_time )
{
    RTCCON |= 0x1;
    
    BCDYEAR = BIN2BCD(rtc_time->year);
    BCDMON  = BIN2BCD(rtc_time->mon);
    BCDDAY  = BIN2BCD(rtc_time->wday);
    BCDDATE = BIN2BCD(rtc_time->mday);
    BCDHOUR = BIN2BCD(rtc_time->hour);
    BCDMIN  = BIN2BCD(rtc_time->min);
    BCDSEC  = BIN2BCD(rtc_time->sec);
        
    RTCCON &= 0x1;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
    RTCCON |= 0x1;
    
    rtc_time->year = BCD2BIN(BCDYEAR);
    rtc_time->mon  = BCD2BIN(BCDMON);
    rtc_time->mday = BCD2BIN(BCDDATE);
    rtc_time->wday = BCD2BIN(BCDDAY);
    rtc_time->hour = BCD2BIN(BCDHOUR);
    rtc_time->min  = BCD2BIN(BCDMIN);
    rtc_time->sec  = BCD2BIN(BCDSEC);
    if( ! rtc_time->sec ){
        rtc_time->year = BCD2BIN(BCDYEAR);
        rtc_time->mon  = BCD2BIN(BCDMON);
        rtc_time->mday = BCD2BIN(BCDDATE);
        rtc_time->wday = BCD2BIN(BCDDAY);
        rtc_time->hour = BCD2BIN(BCDHOUR);
        rtc_time->min  = BCD2BIN(BCDMIN);
        rtc_time->sec  = BCD2BIN(BCDSEC);
    };

    RTCCON &= ~(0X1);
}



void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = (uint32)isr;
    I_ISPC    = ~0;
    INTMSK   &= ~(1<<20);
    TICNT     |= (1<<7);
}

void rtc_close( void )
{
    TICNT     = 0;
    INTMSK   |= (1<<20);// rtc??
    pISR_TICK = (uint32)isr_TICK_dummy;
}


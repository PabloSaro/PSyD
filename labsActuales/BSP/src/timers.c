#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>

extern void isr_TIMER0_dummy( void );

static uint32 loop_ms = 0;
static uint32 loop_s = 0;

static void sw_delay_init( void );

void timers_init( void )
{
    TCFG0 = 0;
    TCFG1 = 0;

    TCNTB0 = 0;
    TCMPB0 = 0;
    TCNTB1 = 0;
    TCMPB1 = 0;
    TCNTB2 = 0;
    TCMPB2 = 0;
    TCNTB3 = 0;
    TCMPB3 = 0;
    TCNTB4 = 0;
    TCMPB4 = 0;
    TCNTB5 = 0;

    TCON = 0x2222202; // 010 0010 0010 0010 0010 0010  0000 0010
    TCON = 0;

    sw_delay_init();
}

static void sw_delay_init( void )
{
    uint32 i;
    
    timer3_start();
    for( i=1000000; i; i--);
    loop_s = ((uint64)1000000*10000)/timer3_stop();
    loop_ms = loop_s / 1000;
};

void timer3_delay_ms( uint16 n )
{
    for( ; n; n-- )
    {
        TCFG0 = (TCFG0 & ~(0xff << 8)) | (49 << 8);   // Prescaler a 50
        TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12); //Divider a 32
        //200*32/(64*10^6) = 0.000025s
        TCNTB3 = 0x28; // 0.000025s *40 = 0.001

        TCON = (TCON & ~(0xf << 16)) | (1 << 17);
        TCON = (TCON & ~(0xf << 16)) | (1 << 16);
        while( !TCNTO3 );
    }
}

void sw_delay_ms( uint16 n )
{
    uint32 i;
    
    for( i=loop_ms*n; i; i-- );
}

void timer3_delay_s( uint16 n )
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);   // Prescaler a 200
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12); //Divider a 32
    //200*32/(64*10^6) = 0.001s
    TCNTB3 = 0x2710; // 0.001s *10000 = 1

    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

void sw_delay_s( uint16 n )
{
    uint32 i;
    
    for( i=loop_s*n; i; i-- );
}

void timer3_start( void ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);    
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = 0xffff; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_stop( void )
{
    TCON &= ~(1 << 16);
    return 0xffff - TCNTO3;
}

void timer3_start_timeout( uint16 n ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);          
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);
    
    TCNTB3 = n; 
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    while( !TCNTO3 );
}

uint16 timer3_timeout( )
{
    return !TCNTO3;
}    

void timer0_open_tick( void (*isr)(void), uint16 tps )
{
    pISR_TIMER0 = (uint32)isr;
    I_ISPC      =  (1 << 13);
    INTMSK    &= ~(1<<26 | 1 << 13);

    if( tps > 0 && tps <= 10 ) {
        TCFG0 = (TCFG0 & ~(0xff)) | (49);   // Prescaler a 50
        TCFG1 = (TCFG1 & ~(0xf)) | (4); //Divider a 32
        //50*32/(64*10^6) = 0.000025s
        TCNTB0 = (40000U / tps);
    } else if( tps > 10 && tps <= 100 ) {
        TCFG0 = (TCFG0 & ~(0xff)) | (79);   // Prescaler a 80
        TCFG1 = (TCFG1 & ~(0xf)) | (0); //Divider a 2
        //80*2/(64*10^6) = 0.0000025s
        TCNTB0 = (400000U / (uint32) tps);
    } else if( tps > 100 && tps <= 1000 ) {
        TCFG0 = (TCFG0 & ~(0xff)) | (7);   // Prescaler a 8
        TCFG1 = (TCFG1 & ~(0xf)) | (0); //Divider a 2
        //8*2/(64*10^6) = 0.00000025s
        TCNTB0 = (4000000U / (uint32) tps);
    } else if ( tps > 1000 ) {
        TCFG0 = (TCFG0 & ~(0xff)) | (0);   // Prescaler a 1
        TCFG1 = (TCFG1 & ~(0xf)) | (0); //Divider a 2
        //1*2/(64*10^6) = 0.000000031s
        TCNTB0 = (32000000U / (uint32) tps);
    }

    TCON = (TCON & ~(0xf)) | (0xa);
    TCON = (TCON & ~(0xf)) | (0x9);
}

void timer0_open_ms( void (*isr)(void), uint16 ms, uint8 mode )
{
    pISR_TIMER0 = (uint32)isr;
    I_ISPC      = 0;
    INTMSK     &= ~(1<<26 | 1 << 13);

    TCFG0 = (TCFG0 & ~(0xff)) | (199); //Prescaler a 200
    TCFG1 = (TCFG1 & ~(0xf)) | (4); //divider a 32
    //200*32/(64*10^6) = 0.0001s
    TCNTB0 = 10*ms;

    TCON =  (TCON & ~(0xf)) | mode | 0x2;
    TCON =  (TCON & ~(0xf)) | mode | 0x1;
}

void timer0_close( void )//falta
{
    TCNTB0 = 0;
    TCMPB0 = 0;

    TCON = (TCON & ~(0xf)) | 0x2;
    TCON = TCON & ~(0xf);
    
    INTMSK |= (1<<13);
    pISR_TIMER0 = (uint32)isr_TIMER0_dummy;
}

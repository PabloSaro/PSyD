#include <s3c44b0x.h>
#include "systemLab3.h"    

static void port_init( void )
{
    PDATA = ~0;
    PCONA = 0xFE;

    PDATB = ~0;
    PCONB = 0x14F;

    PDATC = ~0;
    PCONC = 0x5FF555FF;
    PUPC  = 0x94FB;

    PDATD = ~0;
    PCOND = 0xAAAA;
    PUPD  = 0xFF;
    
    PDATE = ~0;
    PCONE = 0x255A9;
    PUPE  = 0x1FB;

    PDATF = ~0;
    PCONF = 0x251A;
    PUPF  = 0x74;
    
    PDATG = ~0;
    PCONG = 0xF5FF;
    PUPG  = 0x30;

    SPUCR = 0x7;
    
    EXTINT = 0x22000220;
}

void sys_init( void )
{
    port_init();    
}



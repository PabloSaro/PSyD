
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <iis.h>
#include <dma.h>

static void isr_bdma0( void ) __attribute__ ((interrupt ("IRQ")));

static uint8 flag;
static uint8 iomode;
static uint8 play;

void iis_init( uint8 mode )
{
    iomode = mode;

    if( mode == IIS_POLLING )
    {
        IISPSR  = (7<<4) | 7;
        IISMOD  = (1<<3) | 1 | (3 << 6);
        IISFCON = (3<<8);
        IISCON  = 3 ;
    }
    if( mode == IIS_DMA )
    {
        IISPSR  = (7<<4) | 7;
    	IISMOD  = (1<<3) | 1;
        IISFCON = 0;
        IISCON  = 0;
        bdma0_init();
        bdma0_open( isr_bdma0 );
        flag = OFF;
        play = OFF;
    }
}

static void isr_bdma0( void )
{
	if(!(BDICNT0 & (1<<21) )){
		IISCON &= ~1;
	}

    flag = OFF;
    play = OFF;
    I_ISPC = BIT_BDMA0; 
}


inline void iis_putSample( int16 ch0, int16 ch1 )
{

    while( ((IISFCON & (0xf0))>>4) > 6 );
    IISFIF = ch0;
    IISFIF = ch1;
}


inline void iis_getSample( int16 *ch0, int16 *ch1 )
{
    while((IISFCON & 0xf) < 2 );
    *ch0 = IISFIF;
    *ch1 = IISFIF;

}






void iis_play( int16 *buffer, uint32 length, uint8 loop )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING ){
        for( i=0; i<length/2; )
        {
   ch1 = buffer[i++];
   ch2 = buffer[i++];
   iis_putSample( ch1, ch2 );
        }
    }
    else if( iomode == IIS_DMA ){
    BDISRC0 = (1 << 30) | (1 << 28) | (uint32) buffer;
    BDIDES0 = (1 << 30) | (3 << 28) | (uint32) &IISFIF;
    BDCON0 = 0;
    BDICNT0 = (1 << 30) | (1 << 26) | (3 << 22) | (1 << 20) | length;
    if(loop == 1){
    	BDICNT0 |= (1<<21);
    }
    IISMOD = (8 << 4) | 9;
    IISFCON = (0xA << 8) | (IISFCON & 0xff);
    IISCON = (2 << 4) |7;
    flag = ON;
    play = ON;
    }

}



void iis_rec( int16 *buffer, uint32 length )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING ){
        for( i=0; i<length/2; )
        {
        iis_getSample( &ch1, &ch2 );
        buffer[i++] = ch1;
        buffer[i++] = ch2;

        }
    }
    else if( iomode == IIS_DMA )
    {
        while( flag != OFF ){
        BDISRC0  = (1 << 30) | (3 << 28) | (uint32) &IISFIF;
        BDIDES0  = (2 << 30) | (1 << 28) | (uint32) buffer;      
        BDCON0   = 0;
        BDICNT0  = (1 << 30) | (1 << 26) | (3 << 22) | (1 << 20) | length;

        IISMOD  = (1<<3) | 1 |(1<<6);
        IISFCON = (5 << 8) | (IISFCON & 0xff);
        IISCON  = (1 << 4) | (1 << 3) | 3;
        flag = ON;
        }
    }
}



void iis_pause( void )
{
    IISCON &= ~1;
    play = OFF;
}


void iis_continue( void )
{

	IISCON |= 1 ;
	play = ON;
}

uint8 iis_status( void )
{
    return play;
}

void iis_playwawFile( uint8 *fileAddr )
{
    uint32 size;

    while ( !(fileAddr[0] == 'd' && fileAddr[1] == 'a' && fileAddr[2] == 't' && fileAddr[3] == 'a') )
        fileAddr++;
    fileAddr += 4;

    size = (uint32) fileAddr[0];
    size += (uint32) fileAddr[1] << 8;
    size += (uint32) fileAddr[2] << 16;
    size += (uint32) fileAddr[3] << 24;
    fileAddr += 4;

    iis_play( (int16 *)fileAddr, size, OFF );
}



#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if(((1<<6) & PDATG) == 0)
        return PB_LEFT;
    else if(((1<<7) & PDATG) == 0)
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{
	uint8 status;
	if(scancode == PB_LEFT ){
		if(PDATG & (1 << 6))
		status = 0;
		else
		status = 1;
	}
	if(scancode == PB_RIGHT){
			if(PDATG & (1 << 7))
			status = 0;
			else
			status = 1;
	}
	return status;
}

void pb_wait_keydown( uint8 scancode )
{
	while( pb_status(scancode) == 0);
}

void pb_wait_keyup( uint8 scancode )
{
	pb_wait_keydown( scancode );
	while( pb_status(scancode) == 1);
}

void pb_wait_any_keydown( void )
{
	while( pb_scan() == PB_FAILURE);
	//sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_any_keyup( void )
{
	pb_wait_any_keydown();
	 while(  pb_scan() != PB_FAILURE );
	 //sw_delay_ms( PB_KEYUP_DELAY );
}

uint8 pb_getchar( void )
{
    uint8 scancode;

    while( pb_scan() == PB_FAILURE);
    sw_delay_ms( PB_KEYDOWN_DELAY );
    scancode = pb_scan();
    while( pb_scan() != PB_FAILURE );
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( pb_scan() == PB_FAILURE);
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( pb_scan() != PB_FAILURE );
    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )//dudo
{

	 uint8 scancode;

	timer3_start_timeout( ms );
	while( pb_scan() == PB_FAILURE && !timer3_timeout());
	  scancode = pb_scan();
	  return scancode;

}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32)isr;
    EXTINTPND = 0;
    I_ISPC    = (1<<21);
    INTMSK   &= ~(1 << 26 | 1 << 21);
}

void pbs_close( void )
{
    INTMSK  |= (1 << 21);
    pISR_PB  = (uint32)isr_PB_dummy ;
}

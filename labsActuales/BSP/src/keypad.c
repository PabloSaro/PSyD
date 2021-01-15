
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

void keypad_init( void )
{
	timers_init();
};

uint8 keypad_scan( void )//test
{
    uint8 aux;

    aux = *( KEYPAD_ADDR + 0x1c );
    if( (aux & 0x0f) != 0x0f )
    {
        if( (aux & 0x8) == 0 )
            return KEYPAD_KEY0;
        else if( (aux & 0x4) == 0 )
            return KEYPAD_KEY1;
        else if( (aux & 0x2) == 0 )
            return KEYPAD_KEY2;
        else if( (aux & 0x1) == 0 )
            return KEYPAD_KEY3;
    }
    aux = *( KEYPAD_ADDR + 0xfb );
    if( (aux & 0x0f) != 0x0f )
    {
        if( (aux & 0x7) == 0x7)
            return KEYPAD_KEY4;
        else if( (aux & 0xb) == 0xb )
            return KEYPAD_KEY5;
        else if( (aux & 0xd) ==0xd)
            return KEYPAD_KEY6;
        else if( (aux & 0xe) ==0xe)
            return KEYPAD_KEY7;
    }
    aux = *( KEYPAD_ADDR + 0x16 );
    if( (aux & 0x0f) != 0x0f )
    {
    	if( (aux & 0x7) == 0x7)
            return KEYPAD_KEY8;
    	 else if( (aux & 0xb) == 0xb )
            return KEYPAD_KEY9;
    	 else if( (aux & 0xd) ==0xd)
            return KEYPAD_KEYA;
    	 else if( (aux & 0xe) ==0xe)
            return KEYPAD_KEYB;
    }
    aux = *( KEYPAD_ADDR + 0x0e );
    if( (aux & 0x0f) != 0x0f )
       {
    	if( (aux & 0x7) == 0x7)
             return KEYPAD_KEYC;
         else if( (aux & 0xb) == 0xb )
             return KEYPAD_KEYD;
         else if( (aux & 0xd) ==0xd)
              return KEYPAD_KEYE;
         else if( (aux & 0xe) ==0xe)
              return KEYPAD_KEYF;
        }
    return KEYPAD_FAILURE;
}

uint8 keypad_status( uint8 scancode )//bien
{
	uint8 status;
    if(keypad_scan()== scancode){
    	status = 1;
    }
    else{
    	status=0;
    }
    return status;
}

void keypad_wait_keydown( uint8 scancode )//bien
{
     while( 1 ) 
     {
        while( keypad_scan()!= scancode );
        sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
        if ( scancode == keypad_scan() )
            return;
        while( keypad_scan()!= KEYPAD_FAILURE );
        sw_delay_ms( KEYPAD_KEYUP_DELAY );
    }         
}

void keypad_wait_keyup( uint8 scancode )//bien
{
	keypad_wait_keydown(scancode);
	while( keypad_scan()!= KEYPAD_FAILURE );


}

void keypad_wait_any_keydown( void )
{
	while( keypad_scan() == KEYPAD_FAILURE);
}

void keypad_wait_any_keyup( void )
{
	keypad_wait_any_keydown();
	while( keypad_scan() == KEYPAD_FAILURE);
}

uint8 keypad_getchar( void )
{
	uint8 scancode;
		keypad_wait_any_keydown();
	    scancode = keypad_scan();
	    while( keypad_scan() != KEYPAD_FAILURE );
	    sw_delay_ms( KEYPAD_KEYUP_DELAY );
	    return scancode;

}

uint8 keypad_getchartime( uint16 *ms )
{
    uint8 scancode;

    while( keypad_scan() == KEYPAD_FAILURE);
    timer3_start();
    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );

    scancode = keypad_getchar();

    while( keypad_scan() != KEYPAD_FAILURE );
    *ms = timer3_stop() / 10;
    sw_delay_ms( KEYPAD_KEYUP_DELAY );

    return scancode;

}

uint8 keypad_timeout_getchar( uint16 ms )//la ultima
{
	 uint8 scancode;

	timer3_start_timeout( ms );
	while((!(timer3_timeout())) || keypad_scan() == KEYPAD_FAILURE);
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	scancode = keypad_getchar();

	return scancode;
}

void keypad_open( void (*isr)(void) )
{
    pISR_KEYPAD   = (uint32)isr;
    EXTINTPND = ~0;
    I_ISPC    = (1<<24);
    INTMSK   &= ~(1 << 26 | 1 << 24);
}

void keypad_close( void )
{
    INTMSK  |= (1 << 24);
    pISR_KEYPAD  = (uint32)isr_KEYPAD_dummy ;
}


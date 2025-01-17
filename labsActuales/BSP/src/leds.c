
#include <s3c44b0x.h>
#include <leds.h>

uint8 status;

void leds_init( void )
{
	PDATB |= (3<<9);
}

void led_on( uint8 led )
{
    if(led == 1)
    PDATB &= ~(1<<9);
    else if(led == 2)
    PDATB &= ~(1<<10);

}

void led_off( uint8 led )
{
	if(led == 1)
	PDATB |= (1<<9);
	else if(led == 2)
	PDATB |= (1<<10);
}

void led_toggle( uint8 led )
{
	if(led == 1)
	PDATB ^= (1<<9);
	else if(led == 2)
	PDATB ^= (1<<10);

}

uint8 led_status( uint8 led )
{
	if(led == 1){
		if(PDATB & (1 << 9))
		status = 0;
		else
		status = 1;
	}
	if(led == 2){
			if(PDATB & (1 << 10))
			status = 0;
			else
			status = 1;
	}
	return status;
}

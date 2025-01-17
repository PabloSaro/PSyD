#include <s3c44b0x.h>
#include <s3cev40.h>
#include <system.h>
#include <timers.h>
#include <rtc.h>
#include <leds.h>
#include <segs.h>
#include <keypad.h>
#include <uart.h>
#include "eventGenerator.h"

/*******************************************************************/
/*******************************************************************/

#define MAXEVENTS (256)    // N�mero m�ximo de eventos encolables

static volatile struct {
	event_t buffer[MAXEVENTS];
	uint16  size;
	uint16  head;
	uint16  tail;
} efifo;

static void efifo_enqueue( event_t *pevent );
static void efifo_dequeue( event_t *pevent );
static boolean efifo_is_empty( void );
static boolean efifo_is_full( void );

/*******************************************************************/

boolean any_event( void )
{
	return !efifo_is_empty();
}

void get_event( event_t *pevent )
{
	efifo_dequeue( pevent );
}

/*******************************************************************/

static void efifo_init( void )
{
	efifo.head = 0;
	efifo.tail = 0;
	efifo.size = 0;
}

static void efifo_enqueue( event_t *pevent )
{
	efifo.buffer[efifo.tail].time = pevent->time;
	efifo.buffer[efifo.tail].id   = pevent->id;
	efifo.buffer[efifo.tail].arg  = pevent->arg;
    efifo.tail++;
	if( efifo.tail == MAXEVENTS )
		efifo.tail = 0;
	INT_DISABLE;
	efifo.size++;
	INT_ENABLE;
}

static void efifo_dequeue( event_t *pevent )
{
	pevent->time = efifo.buffer[efifo.head].time;
	pevent->id   = efifo.buffer[efifo.head].id;
	pevent->arg  = efifo.buffer[efifo.head].arg;
    efifo.head++;
	if( efifo.head == MAXEVENTS )
		efifo.head = 0;
	INT_DISABLE;
	efifo.size--;
	INT_ENABLE;
}

static boolean efifo_is_empty( void )
{
	return (efifo.size == 0);
}

static boolean efifo_is_full( void )
{
	return (efifo.size == MAXEVENTS-1);
}

/*******************************************************************/
/*******************************************************************/

static boolean arg_needed;
static boolean clients_waiting;

static void keypad_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void keypad_up_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_up_isr( void ) __attribute__ ((interrupt ("IRQ")));

/*******************************************************************/

void eventGenerator_init( void )
{
    keypad_init();
    timers_init();
    leds_init();
    segs_init();
    efifo_init();

    arg_needed = FALSE;
    clients_waiting = FALSE;

    EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);	   // Falling edge tiggered
    keypad_open( keypad_down_isr );
};

/*******************************************************************/

/*
** Se ejecuta cuando se presiona el keypad, programando el T0 para que interrumpa pasado el tiempo de rebote de presion
*/
static void keypad_down_isr( void )
{
	timer0_open_ms( timer0_down_isr, KEYPAD_KEYDOWN_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;				       // Enmascara la interrupci�n por pulsaci�n del keypad
	I_ISPC	  = BIT_KEYPAD;				       // Borra el bit de interrupci�n pendiente
}

/*
** Se ejecuta una vez transcurrido el tiempo de rebote por presion, encolando un evento en funci�n de la tecla pulsada
*/
static void timer0_down_isr( void )
{
	uint8 aux;

    static event_t event;

    aux = keypad_scan();
    if( aux == KEYPAD_FAILURE )
    	segs_off();
    else
    {
		segs_putchar( aux );

		if( !arg_needed )
			switch( aux )
			{
				case 0x0:
				case 0x1:
				case 0x2:
				case 0x3:
				case 0x4:
				case 0x5:
				case 0x6:
				case 0x7:
				case 0x8:
				case 0x9:
				case 0xb:
					event.id = CAJALIBRE;
					event.arg = aux;
					rtc_gettime( &event.time );
					if( !efifo_is_full() )
						efifo_enqueue( &event );
					break;
				case 0xa:
					event.id  = ABRECAJA;
					arg_needed = TRUE;
					break;
				case 0xc:
					event.id = CIERRACAJA;
					arg_needed = TRUE;
					break;
				case 0xe:
					if( !clients_waiting ) {
						event.id = CLIENTEESPERA;
						event.arg = 0;
						rtc_gettime( &event.time );
						led_toggle( LEFT_LED );
						led_toggle( RIGHT_LED );
						if( !efifo_is_full() )
							efifo_enqueue( &event );
						clients_waiting = TRUE;
					} else {
						event.id = SINCLIENTES;
						event.arg = 0;
						rtc_gettime( &event.time );
						led_toggle( LEFT_LED );
						led_toggle( RIGHT_LED );
						if( !efifo_is_full() )
							efifo_enqueue( &event );
						clients_waiting = FALSE;
					}
					break;
				default:
					break;
			}
		else
			switch( aux )
			{
				case 0x0:
				case 0x1:
				case 0x2:
				case 0x3:
				case 0x4:
				case 0x5:
				case 0x6:
				case 0x7:
				case 0x8:
				case 0x9:
					event.arg = aux;
					rtc_gettime( &event.time );
					if( !efifo_is_full() )
						efifo_enqueue( &event );
					arg_needed = FALSE;
					break;
				default:
					break;
					//Printear error;;
					uart0_puts( "\n\n Insercion erronea del evento  \n" );

			}
    }

	EXTINT = (EXTINT & ~(0xf<<4)) | (4<<4);	   // Rising edge triggered
	keypad_open( keypad_up_isr );
	I_ISPC = BIT_TIMER0;				       // Borra el bit de interrupci�n pendiente
}

/*
** Se ejecuta cuando se depresiona el keypad, programando el T0 para que interrumpa pasado el tiempo de rebote de depresion
*/
static void keypad_up_isr( void )
{
	timer0_open_ms( timer0_up_isr, KEYPAD_KEYUP_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;				       // Enmascara la interrupci�n por pulsaci�n del keypad
	I_ISPC	  = BIT_KEYPAD;				       // Borra el bit de interrupci�n pendiente
}

/*
** Se ejecuta una vez transcurrido el tiempo de rebote por depresion
*/
static void timer0_up_isr( void )
{
	EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);    // Falling edge triggered
	keypad_open( keypad_down_isr );
	I_ISPC = BIT_TIMER0;				       // Borra el bit de interrupci�n pendiente
}

/*******************************************************************/



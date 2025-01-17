#include <system.h>
#include <uart.h>
#include <rtc.h>
#include <keypad.h>
#include <common_types.h>
#include "eventGenerator.h"
#include <lcd.h>
#include <iis.h>
#include <uda1341ts.h>
#include <leds.h>
#include <timers.h>

/************************************/

//void isr_tick( void ) __attribute__ ((interrupt ("IRQ")));

/**********************************/


#define  GRAB_0     	((int16 *) 0x0c400000)
#define  GRAB_1      	((int16 *) 0x0c40f2f0)
#define  GRAB_2     	((int16 *) 0x0c4163be)
#define  GRAB_3  	    ((int16 *) 0x0c4217e0)
#define  GRAB_4     	((int16 *) 0x0c42ce82)
#define  GRAB_5      	((int16 *) 0x0c438a50)
#define  GRAB_6       	((int16 *) 0x0c44781e)
#define  GRAB_7       	((int16 *) 0x0c451aec)
#define  GRAB_8      	((int16 *) 0x0c45bdba)
#define  GRAB_9       	((int16 *) 0x0c464788)
#define  ABRIRCAJA 	    ((int16 *) 0x0c470356)
#define  CERRARCAJA     ((int16 *) 0x0c49ad24)
#define  PASEPOR      	((int16 *) 0x0c4bf8c6)
//////////////////////////////////////////////
#define GRAB_0_SIZE 	(33300)
#define GRAB_1_SIZE 	(28878)
#define GRAB_2_SIZE 	(28878)
#define GRAB_3_SIZE 	(46114)
#define GRAB_4_SIZE 	(46754)
#define GRAB_5_SIZE 	(48078)
#define GRAB_6_SIZE 	(40878)
#define GRAB_7_SIZE     (41678)
#define GRAB_8_SIZE     (35278)
#define GRAB_9_SIZE     (48078)
#define ABRIRCAJA_SIZE  (137678)
#define CERRARCAJA_SIZE (150434)
#define PASEPOR_SIZE    (110478)


/////////////////////////////////////////////

/***************************************/
#define OFERTAS1   ((uint8 *)0x0c210000)
#define OFERTAS2   ((uint8 *)0x0c220000)
#define OFERTAS3   ((uint8 *)0x0c230000)
#define OFERTAS4   ((uint8 *)0x0c240000)
#define OFERTAS5   ((uint8 *)0x0c250000)
#define OFERTAS6   ((uint8 *)0x0c260000)
//#define MAPA       ((uint8 *)0x0c270000)
/***************************************/


int8 publicidad = 10;
int numCajas = 10;
int TIEMPOCAJA = 3;
int8  publi;
static boolean actualizar = 1;



void audios(uint8 x);//un switch para elegir el numero del audio
void pintar_Menu(); //pinta un menu
void pintar_Cajas(char Cajas []);//pinta estado de las cajas
void mensaje(int8 num, uint8 x);//informade por el lcd de la accion que se ha realizado
int cajafree(char Cajas []);//funcion auxiliar que si no hay accion y hay clientes revisa si hay cajas libres


int main( void )
{

	//int caja [numCajas] ;

	rtc_time_t rtc_time;
    event_t event;
	sys_init();
	uart0_init();
	rtc_init();
	lcd_init();
	uda1341ts_init();

	/////////////////////////////////////////////////////

	//lcd_putWallpaper( OFERTAS1 );
	iis_init( IIS_POLLING );
	uda1341ts_setvol( VOL_MED );

	///////////////////////////////////////////////////////


	/***************************************************/
	uint8 caja,cont=0;
	char Cajas[] ={'C','C','C','C','C','C','C','C','C','C'};
	publi=publicidad;


	
	pintar_Menu();




    /************************************/



	uart0_puts( "\nIntroduzca fecha\n" );
	uart0_puts( "  - Dia: " );
	rtc_time.mday = (uint8) uart0_getint();
	uart0_puts( "  - Mes: " );
	rtc_time.mon = (uint8) uart0_getint();
	uart0_puts( "  - A�o (2 digitos): " );
	rtc_time.year = (uint8) uart0_getint();
	uart0_puts( "Introduzca hora\n" );
	uart0_puts( "  - Hora: " );
	rtc_time.hour = (uint8) uart0_getint();
	uart0_puts( "  - Minuto: " );
	rtc_time.min = (uint8) uart0_getint();
	uart0_puts( "  - Segundo: " );
	rtc_time.sec = (uint8) uart0_getint();

	rtc_puttime( &rtc_time );
	pintar_Cajas(Cajas);
    /************************************/

	eventGenerator_init();                              // Arranca el generador de eventos

    while( 1 )
    {


        if( any_event() )                               // Comprueba si hay alg�n evento por procesar
        {
        	//publi = 10;
            get_event( &event );                        // Desencola un evento

            uart0_puts( "\n(" );                         // Procesa el evento
            uart0_putint( event.time.mday );
            uart0_putchar( '/' );
            uart0_putint( event.time.mon );
            uart0_putchar( '/' );
            uart0_putint( event.time.year );
            uart0_putchar( ' ' );
            uart0_putint( event.time.hour );
            uart0_putchar( ':' );
            uart0_putint( event.time.min );
            uart0_putchar( ':' );
            uart0_putint( event.time.sec );
            uart0_puts( ")" );
            switch( event.id )
            {
                case ABRECAJA:
                    uart0_puts( " Abre la caja n�mero " );
                    uart0_putint( event.arg );
                    if (Cajas[event.arg] == 'C'){
                    	Cajas[event.arg] = 'O';
                    	mensaje(2, event.arg);
                    	actualizar = 1;
                    	iis_play( ABRIRCAJA, ABRIRCAJA_SIZE, OFF );
                    	audios(event.arg);
                    }
                    else{
                      	uart0_puts( "\n la caja no estaba cerrada  " );
                    }

                    break;
                case CIERRACAJA:
                    uart0_puts( " Cierra la caja n�mero " );
                    uart0_putint( event.arg );
                    if (Cajas[event.arg] == 'O'){
                    	Cajas[event.arg] = 'C';
                    	mensaje(1, event.arg);
                    	actualizar = 1;
                    	iis_play( CERRARCAJA, CERRARCAJA_SIZE, OFF );
                    	audios(event.arg);
                    }
                    else{
                    	uart0_puts( "\n la caja no estaba abierta  " );
                    }

                    break;
                case CAJALIBRE:
                    uart0_puts( " Est� libre la caja n�mero " );
                    uart0_putint( event.arg );
                    if( Cajas[event.arg] == 'X'){
                    	Cajas[event.arg] = 'O';
                    	mensaje(3, event.arg);
                    	actualizar = 1;
                    }
                    else{
                    	 uart0_puts( "\n la caja no estaba ocupada  " );
                    }
                    break;
                case CLIENTEESPERA:
                    uart0_puts( "\n Al menos hay un cliente esperando \n" );
                    /////////////////////
                   caja = cajafree(Cajas);
                	if(caja != 11){
                	Cajas[caja] = 'X';
                	mensaje(0, caja);
                	 uart0_puts( "\n se ocupa una caja " );
                	actualizar = 1;
                	iis_play( PASEPOR, PASEPOR_SIZE, OFF );
                	audios(caja);
                	}
                	///////////////////////
                    break;
                case SINCLIENTES:
                    uart0_puts( " No hay clientes esperando" );
                    break;
            }
            uart0_putchar( '\n' );
        }
        else
        {
        	caja = cajafree(Cajas);
        	if(led_status(1) && caja != 11 ){
        		 Cajas[caja] = 'X';
        		 mensaje(0, caja);
        		 uart0_puts( "\n se ocupa una caja \n" );
        		 actualizar = 1;
             	iis_play( PASEPOR, PASEPOR_SIZE, OFF );
             	audios(caja);
        	}

        }
        if(actualizar == 1){
        	sw_delay_s(1);
        	actualizar = 0;
        	pintar_Cajas(Cajas);
        	publi = publicidad;
        }
        else{
            publi--;
            if(publi <= 0)
               {
            	lcd_clear();
            	lcd_on();
                cont = ((cont+1) %6);
                switch( cont ){
        			case 0x0:lcd_putWallpaper( OFERTAS1 );break;
        			case 0x1:lcd_putWallpaper( OFERTAS2 );break;
        			case 0x2:lcd_putWallpaper( OFERTAS3 );break;
        			case 0x3:lcd_putWallpaper( OFERTAS4 );break;
        			case 0x4:lcd_putWallpaper( OFERTAS5 );break;
        			case 0x5:lcd_putWallpaper( OFERTAS6 );break;
                	}
                }
              }

        sw_delay_s(2);
    }
    return 0;
}


void audios(uint8 x){
	switch( x ){
	        			case 0x0:	 iis_play( GRAB_0, GRAB_0_SIZE, OFF );break;
	        			case 0x1:	 iis_play( GRAB_1, GRAB_1_SIZE, OFF );break;
	        			case 0x2:	 iis_play( GRAB_2, GRAB_2_SIZE, OFF );break;
	        			case 0x3:	 iis_play( GRAB_3, GRAB_3_SIZE, OFF );break;
	        			case 0x4:	 iis_play( GRAB_4, GRAB_4_SIZE, OFF );break;
	        			case 0x5:	 iis_play( GRAB_5, GRAB_5_SIZE, OFF );break;
	        			case 0x6:	 iis_play( GRAB_6, GRAB_6_SIZE, OFF );break;
	        			case 0x7:	 iis_play( GRAB_7, GRAB_7_SIZE, OFF );break;
	        			case 0x8:	 iis_play( GRAB_8, GRAB_8_SIZE, OFF );break;
	        			case 0x9:	 iis_play( GRAB_9, GRAB_9_SIZE, OFF );break;
	                	}
}

int cajafree(char Cajas []){
	int i,x=11;
	for(i=0;i < numCajas;i++){
		if(Cajas[i] == 'O'){
			x = i;
		}
		}
		return x;
}
void pintar_Menu( void ){
	lcd_clear();
	lcd_on();
    lcd_draw_box( 10, 10, 310, 230, BLACK, 5 );
    lcd_puts_x2( 85, 20 , BLACK, "FILA UNICA" );
    lcd_puts( 20, 50, BLACK , "Programa que controla una fila unica" );
    lcd_puts( 20, 66, BLACK , "los clientes podran ir a 10 cajas. " );
    lcd_puts( 20, 82, BLACK , "las cajas tienen 3 estados. " );
    lcd_puts( 40, 98, BLACK , "Abierta" );
    lcd_puts( 140, 98, BLACK , "Cerrada" );
    lcd_puts( 230, 98, BLACK , "Ocupada" );
    lcd_putchar_x2( 56, 114, BLACK , 'O' );
    lcd_putchar_x2( 156, 114, BLACK , 'C' );
    lcd_putchar_x2( 246, 114, BLACK , 'X' );
    lcd_puts( 51, 146, BLACK , "a+n");
    lcd_puts( 151, 146, BLACK , "c+n");
    lcd_puts( 241, 146, BLACK , "b+n");
    lcd_puts( 110, 162, BLACK , "0<= n <= 9" );
    lcd_puts( 20, 178, BLACK , "LA E ACTIVA Y DESACTIVA CLIENTES" );
    lcd_puts( 20, 199, DARKGRAY , "Programa realizado por:" );
    lcd_puts( 20, 215, DARKGRAY , "Pablo Saro y Gonzalo Fernandez" );
}

void mensaje(int8 num, uint8 x){
	lcd_clear();
	lcd_on();
	lcd_draw_box( 10, 10, 310, 230, BLACK, 5 );
	switch( num ){
	 case 0x0:	 lcd_puts_x2( 115, 20 , BLACK, "PASE A" );break;
	 case 0x1:	 lcd_puts_x2( 100, 20 , BLACK, "SE CIERRA" );break;
	 case 0x2:	 lcd_puts_x2( 110, 20 , BLACK, "SE ABRE" );break;
	 case 0x3:	  lcd_puts_x2( 100, 20 , BLACK, "SE LIBERA" );break;
	}

	lcd_puts_x2( 110, 52 , BLACK, "LA CAJA" );
	lcd_draw_box( 130, 95, 185, 140, BLACK, 5 );
	lcd_putint_x2( 152, 104 , BLACK, x );

}



void pintar_Cajas(char Cajas []){
    lcd_clear();
    lcd_on();

    uint8 x =30, y = 50 ;
    uint8 i=0;

    lcd_draw_box( 10, 10, 310, 230, BLACK, 5 );
    lcd_puts( 75, 20, DARKGRAY , "ESTADO DE LAS CAJAS" );


    for(;i < numCajas;i++){
    	if (i == 5){
    		 x = 30;
    		 y += 90;
    	}
    	lcd_putint( x+15, y, BLACK, i );
    	lcd_draw_box( x, y+20, x+36, y+60, BLACK, 3 );
    	lcd_putchar_x2( x+12, y+25 , BLACK, Cajas[i] );
    	x += 55;
    	}
	}



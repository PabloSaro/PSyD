/*-------------------------------------------------------------------
**
**  Fichero:
**    eventGenerator.h  4/9/2018
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de las funciones
**    necesarias para generar eventos en tiempo real que reproduzcan
**    el comportamiento de clientes y cajas
**
**  Notas de diseño:
**    - Este generador hace uso en exclusividad de los siguientes
**      recursos: SEGS, LEDS, KEYPAD y TIMER0, Además usa el RTC
**    - Lo hace invocando las funciones siguientes del BSP:
**        segs_init, segs_off, segs_putchar
**        leds_init, led_toggle
**        keypad_init, keypad_open, keypad_scan,
**        timers_init, timer0_open_ms,
**        rtc_gettime
**
**-----------------------------------------------------------------*/

#ifndef __REALTIMEMODEL_H__
#define __REALTIMEMODEL_H__

#include <common_types.h>
#include <rtc.h>

/* Tipos de eventos */

#define ABRECAJA        (1)    // Abre una caja
#define CIERRACAJA      (2)    // Cierra una caja
#define CAJALIBRE       (3)    // El cliente ha abandonado la caja
#define CLIENTEESPERA   (4)    // Hay clientes en espera
#define SINCLIENTES     (5)    // No hay clientes en espera





/* Definición del tipo de datos evento */

typedef struct {
    rtc_time_t time;   // Marca de tiempo del evento
    uint8      id;     // Tipo de evento
    uint8      arg;    // Origen del evento (aplica solamente a cajas)
} event_t;

/*
** Inicializa el generador de eventos en tiempo real
**
** Tras ejecutar esta funcion, la pulsación de teclas del keypad generaran
** eventos que se encolan en una FIFO interna al generador. Estos son:
**   - Tecla 1-9: Genera un evento de tipo CAJALIBRE y arg=tecla
**   - Tecla A + Tecla 1-9: Genera un evento de tipo ABRECAJA y arg=tecla
**   - Tecla C + Tecla 1-9: Genera un evento de tipo CIERRACAJA y arg=tecla
**   - Tecla E: Genera alternativamente eventos de tipo CLIENTEESPERA y de
**              tipo SINCLIENTES
**
** La presencia de clientes en espera se muestra encencidendo leds.
** La última tecla pulsada se muestra en el display 7-segs. Si hay un error
** de lectura del keypad el display se apaga.
*/
void eventGenerator_init( void );

/*
** Indica si hay eventos en la FIFO por procesar
*/
boolean any_event( void );

/*
** Desencola un evento
*/
void get_event( event_t *pevent );

#endif


#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
    UFCON0  = 0x1;
    UMCON0  = 0x0;
    ULCON0  = 0x3;
    UBRDIV0 = 0x22;
    UCON0   = 0x5;
}

void uart0_putchar( char ch )
{
	//modificada
	while((UFSTAT0 & (1<<9)));
    UTXH0 = ch;
}        

char uart0_getchar( void )
{	//modificada
	while(!(UFSTAT0 & (0xf)));
    return URXH0;
}

void uart0_puts( char *s )
{	//modificada()
	int i=0;
	while(s[i]!= '\0'){
		uart0_putchar(s[i]);
		i++;
	}
}


void uart0_putint( int32 i )
{
	char buf[8 + 1];
	char *p = buf + 8;
	uint8 c;
	int pos=0;
	if (i < 0){
		pos = -1;
		i = - i;
	}
	*p = '\0';

	do {
		c = i % 10;
		*--p = '0' + c;
		i = i /10;
	} while( i );
	if (pos == -1){
		*--p= '-';
	}
	uart0_puts( p );
}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'A' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{	//modificada
	int i=0;
	s[i] = uart0_getchar();
	while(s[i] != '\n'){
		i++;
		s[i] = uart0_getchar();
	}
	s[i]= '\0';
}

int32 uart0_getint( void )
//Espera la recepción de una cadena de dígitos decimales que
//interpreta como un entero con signo que devuelve
//modificada
{
	char numero[256];
	int32 acumulado = 0, i = 0, signo = 0;
	uart0_gets(numero);
	if(numero[i] == '-'){
		signo = -1;
		i++;
	}
	while(numero[i] != '\0'){
		acumulado = acumulado * 10  + (numero[i] - '0');
		i++;
	}
	if (signo == -1 ){
		acumulado = - acumulado;
	}

	return acumulado;

}
/*

	char buf [8+1];
	int32 num;
	char *p = buf[8];
	char aux;
	int i = 0;

	do{
	aux = *--p - 0;
	if (aux != '-' && aux != '/0')
	num += aux * pow(10,i);// returned = returned * 10;
	i++;
	}while(*p);

	if(buf[0]=='-'){
		num = -num;
	}
	return num;

*/


uint32 uart0_gethex( void )
//Espera la recepción de una cadena de dígitos hexadecimales que
//interpreta como un entero sin signo que devuelve
//modificada
{
	char num[256];
	uint32 acumulado = 0;
	int i = 0, provisional=0;
	uart0_gets(num);
	while(num[i] != '\0'){
		if(( '0' <= num[i]) && (num[i] <= '9')){
			provisional = num[i] - '0';
			acumulado = acumulado * 16 + provisional;
		}
		else if(( 'A' <= num[i]) && (num[i] <= 'F')){
			provisional = num[i] - '7';
			acumulado = acumulado * 16 + provisional;
		}
		else if(( 'a' <= num[i]) && (num[i] <= 'f')){
			provisional = num[i] - 'a' + 10;
			acumulado = acumulado * 16 + provisional;
		}
		i++;
	}
	return acumulado;
}
	/*
	char numero[255];
	int32 acumulado = 0, i = 0, signo = 0;
	uart0_gets(numero);
	while(numero[i] != '\0'){
		if(numero[i] == '-'){
			signo = -1;
		}
		acumulado = acumulado * 10  + (numero[i] - '0');
		i++;
	}
	if (signo == -1 ){
		acumulado = - acumulado;
	}

	return acumulado;
	*/




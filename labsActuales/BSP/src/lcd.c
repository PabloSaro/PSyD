
#include <s3c44b0x.h>
#include <lcd.h>

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;

void lcd_init( void )
{      
    DITHMODE = 0x12210;
    DP1_2    = 0xA5A5;
    DP4_7    = 0xBA5DA65;
    DP3_5    = 0xA5A5F;
    DP2_3    = 0xD6B;
    DP5_7    = 0xEB7B5ED;
    DP3_4    = 0x7DBE;
    DP4_5    = 0x7EBDF ;
    DP6_7    = 0x7FDFBFE;
    
    REDLUT   = 0x0;
    GREENLUT = 0x0;
    BLUELUT  = 0x0;

    LCDCON1  = 0x1C020;
    LCDCON2  = 0x13CEF;
    LCDCON3  = 0x0;

    LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
    LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
    LCDSADDR3 = 0x50;
    
    lcd_off();

}

void lcd_on( void )
{
    LCDCON1 |= 1;
    state = 1;
}

void lcd_off( void )
{
    LCDCON1 &= ~(1<<0);
    state = 0;
}

uint8 lcd_status( void )
{
    return state;
}

void lcd_clear( void )
{
	int i=0;
    for(;i< (LCD_WIDTH * LCD_HEIGHT /2);i++){
     lcd_buffer[i] = 0;
    }
}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    
    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
    uint8 byte, bit,aux;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    byte = lcd_buffer[i];
    aux = (byte >> bit);
    aux &= 0xF << bit;
    return aux;
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{	//hay que llamar a putpixel, pero no se si esta bien asi
    //uint16 left, right;
    //uint8 byte;
    uint16 i,j;
    i = xleft;
//    left = xleft/2 + y*(LCD_WIDTH/2);
//    right =  xright/2 + y*(LCD_WIDTH/2);

    for(j=y;j<y+width; j++){
    	i = xleft;
    	//right = right +(j*(LCD_WIDTH/2));
    	for(; i<xright;i++){
    		//lcd_buffer[i] = color |(color << 4);
    		lcd_putpixel(i,j,color);
    	}
    }
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{	//hay que llamar a putpixel, pero no se si esta bien asi

  //  uint16 left, right;
    //uint8 byte;
    uint16 i,j;

    i = x;

 //   left = x/2 + yup*(LCD_WIDTH/2);
 //   right =  x/2 + yup*(LCD_WIDTH/2)+(width /2);

    for(j=yup;j<ydown; j++){
    	i = x;
    	for(; i<(x+width);i++){
    		lcd_putpixel(i,j,color);//rellenaria los 2 pixels???
    	}

    }
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
	lcd_draw_hline(xleft,xright,yup,color,width);
	lcd_draw_hline(xleft,xright + width,ydown,color,width);
	lcd_draw_vline(yup, ydown, xleft, color, width);
	lcd_draw_vline(yup, ydown, xright, color, width);
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )                    
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	int i =0;
    while(s[i] != '\0'){
    	lcd_putchar(x,y,color,s[i]);
    	x += 8;
    	i++;
    }
}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
	/*
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + i/2;//de int 32 a font que esta en 16???
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )
            if( bitmap[line] & (0x80 >> row) )//probablemente haya que cambiar esta direccion
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
     */


	char buf[20 + 1];
	char *p = buf + 20;
	uint8 c;
	if (i < 0){
		lcd_putchar(x,y,color,'-');
		x=x+8;
		i= i * -1;
	}
	*p = '\0';

	do {
		c = i % 10;
		*--p = '0' + c;
		i = i /10;
	} while( i );
	lcd_puts(x, y, color, p);
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
{
	/*
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + i/2;//de int 32 a font que esta en 16???
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )
            if( bitmap[line] & (0x80 >> row) )//esta direccion tambien
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
                */
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
	lcd_puts(x, y, color, p);

}

void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )
            if( bitmap[line] & (0x80 >> row) ){
                lcd_putpixel( x+row*2, y+line*2, color );
    			lcd_putpixel( x+row*2+1, y+line*2, color );
    			lcd_putpixel( x+row*2, y+line*2+1, color );
    			lcd_putpixel( x+row*2+1, y+line*2+1, color );
            }
            else{
                lcd_putpixel( x+row*2, y+line*2, WHITE );
				lcd_putpixel( x+row*2+1, y+line*2, WHITE );
				lcd_putpixel( x+row*2, y+line*2+1, WHITE );
				lcd_putpixel( x+row*2+1, y+line*2+1, WHITE );
            }
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
	int i =0;
    while(s[i] != '\0'){
    	lcd_putchar_x2(x,y,color,s[i]);
    	x += 16;
    	i++;
    }
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
	char buf[20 + 1];
	char *p = buf + 20;
	uint8 c;
	if (i < 0){
		lcd_putchar_x2(x,y,color,'-');
		x=x+8;
		i= i * -1;
	}
	*p = '\0';

	do {
		c = i % 10;
		*--p = '0' + c;
		i = i /10;
	} while( i );
	lcd_puts_x2(x, y, color, p);

}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
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
	lcd_puts_x2(x, y, color, p);

}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;
    
    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )                                                                       
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}

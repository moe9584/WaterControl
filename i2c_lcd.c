/*****************************************************************************
 
 i2c_lcd.c - LCD over I2C library 
		Designed for HD44870 based LCDs with I2C expander PCF8574X
		on Atmels AVR MCUs
 
 Copyright (C) 2006 Nico Eichelmann and Thomas Eichelmann
               2014 clean up by Falk Brunner
			   2014 modifiy PIN-Configuration, dual use LCD, I2C-adress by Christoph Moser
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 
 You can contact the authors at info@computerheld.de
 
*****************************************************************************/

/*
	Version 0.11
	Requires I2C-Library from Peter Fleury http://jump.to/fleury
	
	See i2clcd.h for description and example.
*/

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h> 
#include <util/delay.h>
#include "i2c_lcd.h"
#include "i2cmaster.h"

// global variable für light control

static uint8_t lightOn=0;

//-	Display initialization sequence

void i2c_lcd_clear(uint8_t Dev_ID) {
	i2c_lcd_command(I2C_LCD_CLEAR, Dev_ID);
	_delay_ms(2);
}


void i2c_lcd_init(uint8_t Dev_ID) {
    
    i2c_lcd_light(false, Dev_ID);
    	
	_delay_ms(15);		        //-	Wait for more than 15ms after VDD rises to 4.5V
	i2c_lcd_write(CMD_D1 | CMD_D0, Dev_ID);	//-	Set interface to 8-bit
	_delay_ms(5);			    //-	Wait for more than 4.1ms
	i2c_lcd_write(CMD_D1 | CMD_D0, Dev_ID);	//-	Set interface to 8-bit
	_delay_ms(0.1);		        //-	Wait for more than 100us	
	i2c_lcd_write(CMD_D1 | CMD_D0, Dev_ID);	//-	Set interface to 8-bit
	i2c_lcd_write(CMD_D1, Dev_ID);		    //-	Set interface to 4-bit

	//- From now on in 4-bit-Mode
	i2c_lcd_command(I2C_LCD_LINE_MODE | I2C_LCD_5X7, Dev_ID);
	i2c_lcd_command(I2C_LCD_DISPLAYON | I2C_LCD_CURSOROFF | I2C_LCD_BLINKINGOFF, Dev_ID);
	i2c_lcd_command(I2C_LCD_CLEAR, Dev_ID);
    _delay_ms(2);
	i2c_lcd_command(I2C_LCD_INCREASE | I2C_LCD_DISPLAYSHIFTOFF, Dev_ID);
}

//-	Write data to i2c

void i2c_lcd_write_i2c(uint8_t value, uint8_t Dev_ID) {
	i2c_start_wait(Dev_ID+I2C_WRITE);
	i2c_write(value);
	i2c_stop();
}

//-	Write nibble to display with pulse of enable bit
// map pinout between PCF8574 and LCD

void i2c_lcd_write(uint8_t value, uint8_t Dev_ID) {
    uint8_t data_out=0;
    
    // map data to LCD pinout
    if (value & CMD_D0) data_out |= I2C_LCD_D4;
    if (value & CMD_D1) data_out |= I2C_LCD_D5;
    if (value & CMD_D2) data_out |= I2C_LCD_D6;
    if (value & CMD_D3) data_out |= I2C_LCD_D7;
    if (value & CMD_RS) data_out |= I2C_LCD_RS;
    if (value & CMD_RW) data_out |= I2C_LCD_RW;
    if (!lightOn) data_out |= I2C_LCD_LIGHT_N;

	i2c_lcd_write_i2c(data_out | I2C_LCD_E, Dev_ID);		//-	Set new data and enable to high
	i2c_lcd_write_i2c(data_out, Dev_ID);	            //-	Set enable to low
}

//-	Read data from i2c

uint8_t i2c_lcd_read_i2c(uint8_t Dev_ID) {
	uint8_t lcddata = 0x00;

	i2c_start_wait(Dev_ID+I2C_READ);
	lcddata = i2c_readNak();
	i2c_stop();
	return lcddata;
}

//-	Read data from display over i2c (lower nibble contains LCD data)

uint8_t i2c_lcd_read(bool mode, uint8_t Dev_ID) {
	uint8_t lcddata, data;

	if(mode == I2C_LCD_DATA) {
		lcddata = (I2C_LCD_E | I2C_LCD_RS | I2C_LCD_RW | I2C_LCD_D4 | I2C_LCD_D5 | I2C_LCD_D6 | I2C_LCD_D7);
	} else {
		lcddata = (I2C_LCD_E | I2C_LCD_RW | I2C_LCD_D4 | I2C_LCD_D5 | I2C_LCD_D6 | I2C_LCD_D7);
	}

    if (!lightOn) lcddata |= I2C_LCD_LIGHT_N;
    i2c_lcd_write_i2c(lcddata, Dev_ID);
	lcddata = i2c_lcd_read_i2c(Dev_ID);

    data=0;
    // map data from LCD pinout to internal positions
    if (lcddata & I2C_LCD_D4) data |= CMD_D0;
    if (lcddata & I2C_LCD_D5) data |= CMD_D1;
    if (lcddata & I2C_LCD_D6) data |= CMD_D2;
    if (lcddata & I2C_LCD_D7) data |= CMD_D3;
 
    lcddata=0;   
    if (!lightOn) lcddata |= I2C_LCD_LIGHT_N;
	i2c_lcd_write_i2c(lcddata, Dev_ID);

	return data;
}

//-	Read one complete byte via i2c from display

uint8_t i2c_lcd_getbyte(bool mode, uint8_t Dev_ID)  {
	uint8_t hi, lo;

	hi = i2c_lcd_read(mode, Dev_ID);
	lo = i2c_lcd_read(mode, Dev_ID);
	return (hi << 4) + (lo & 0x0F);
}

//-	Issue a command to the display (use the defined commands above)

void i2c_lcd_command(uint8_t command, uint8_t Dev_ID) {

    i2c_lcd_write((command >> 4), Dev_ID);
	i2c_lcd_write((command & 0x0F), Dev_ID);	
}

//-	Print string to cursor position

void i2c_lcd_print(char *string, uint8_t Dev_ID) {

	while(*string)	{
		i2c_lcd_putchar(*string++, Dev_ID);
	}
}

//-	Print string from flash to cursor position

void i2c_lcd_print_P(PGM_P string, uint8_t Dev_ID) {
    uint8_t c;

	while((c=pgm_read_byte(string++)))	{
		i2c_lcd_putchar(c, Dev_ID);
	}
}

//-	Put char to atctual cursor position

void i2c_lcd_putchar(char lcddata, uint8_t Dev_ID) {

	i2c_lcd_write((lcddata >> 4) | CMD_RS, Dev_ID);
	i2c_lcd_write((lcddata & 0x0F) | CMD_RS, Dev_ID);
}

//-	Put char to position

bool i2c_lcd_putcharlc(uint8_t line, uint8_t col, char value, uint8_t Dev_ID) {

	if(!i2c_lcd_gotolc(line, col, Dev_ID)) return false;
	i2c_lcd_putchar(value, Dev_ID);
	
	return true;
}

//-	Print string to position (If string is longer than LCD_COLS overwrite first chars)(line, row, string)

bool i2c_lcd_printlc(uint8_t line, uint8_t col, char *string, uint8_t Dev_ID) {

    if (!i2c_lcd_gotolc(line, col, Dev_ID)) return false;
    	
	while(*string) {
		i2c_lcd_putchar(*string++, Dev_ID);
		col++;
		if(col > I2C_LCD_COLS) {
			col = 1;
            i2c_lcd_gotolc(line, col, Dev_ID);
		}
	}
	return true;
}

//-	Print string from flash to position
//  (If string is longer than LCD_COLS overwrite first chars)(line, row, string)

bool i2c_lcd_printlc_P(uint8_t line, uint8_t col, char *string, uint8_t Dev_ID) {
    char c;
    
    if (!i2c_lcd_gotolc(line, col, Dev_ID)) return false;
    	
	while((c=pgm_read_byte(string++))) {
		i2c_lcd_putchar(c, Dev_ID);
		col++;
		if(col > I2C_LCD_COLS) {
			col = 1;
            i2c_lcd_gotolc(line, col, Dev_ID);
		}
	}
	return true;
}

//-	Print string to position (If string is longer than LCD_COLS continue in next line)

bool i2c_lcd_printlcc(uint8_t line, uint8_t col, char *string, uint8_t Dev_ID) {	
    
    if (!i2c_lcd_gotolc(line, col, Dev_ID)) return false;
    	
	while(*string) {
		i2c_lcd_putchar(*string++, Dev_ID);
		col++;
		if(col > I2C_LCD_COLS) {
			line++;
			col = 1; 
    		if(line > I2C_LCD_LINES) {
			    line = 1;
		    }
            i2c_lcd_gotolc(line, col, Dev_ID);
		}
	}
	return true;
}

//-	Print string from flash to position
// (If string is longer than LCD_COLS continue in next line)

bool i2c_lcd_printlcc_P(uint8_t line, uint8_t col, char *string, uint8_t Dev_ID) {	
    char c;
      
    if (!i2c_lcd_gotolc(line, col, Dev_ID)) return false;
    	
	while((c=pgm_read_byte(string++))) {
		i2c_lcd_putchar(c, Dev_ID);
		col++;
		if(col > I2C_LCD_COLS) {
			line++;
			col = 1; 
    		if(line > I2C_LCD_LINES) {
			    line = 1;
		    }
            i2c_lcd_gotolc(line, col, Dev_ID);
		}
	}
	return true;
}

//-	Go to position (line, column)

bool i2c_lcd_gotolc(uint8_t line, uint8_t col, uint8_t Dev_ID) {
	uint8_t lcddata=0;
	
	if( (line > I2C_LCD_LINES) ||
	    (col > I2C_LCD_COLS) ||
	    ((line == 0) || (col == 0)) ) return false;
	
    switch (line) {
        case 1: lcddata = I2C_LCD_LINE1; break;
        case 2: lcddata = I2C_LCD_LINE2; break;
        case 3: lcddata = I2C_LCD_LINE3; break;
        case 4: lcddata = I2C_LCD_LINE4; break;
    }
    lcddata |= 0x80;
    lcddata += (col-1);
	i2c_lcd_command(lcddata, Dev_ID);
	return true;	
}

//-	Go to nextline (if next line > (LCD_LINES-1) return false)

bool i2c_lcd_nextline(uint8_t Dev_ID) {
	uint8_t line, col;

	i2c_lcd_getlc(&line, &col, Dev_ID);
	if (!i2c_lcd_gotolc(line + 1, 1, Dev_ID)) {
        return false;
    } else {
        return true;
    }
}

//-	Get line and row (target byte for line, target byte for row)

bool i2c_lcd_getlc(uint8_t *line, uint8_t *col, uint8_t Dev_ID) {
	uint8_t lcddata;

	lcddata = i2c_lcd_getbyte(I2C_LCD_ADDRESS, Dev_ID);
	if (lcddata & (1 << 7)) return false;       // LCD busy

    if (lcddata >= I2C_LCD_LINE1 && lcddata < (I2C_LCD_LINE1+I2C_LCD_COLS)) {
        *line = 1;
        *col = lcddata - I2C_LCD_LINE1 + 1;
        return true;
    } else if (lcddata >= I2C_LCD_LINE2 && lcddata < (I2C_LCD_LINE2+I2C_LCD_COLS)) {
        *line = 2;
        *col = lcddata - I2C_LCD_LINE2 + 1;
        return true;
    } else if (lcddata >= I2C_LCD_LINE3 && lcddata < (I2C_LCD_LINE3+I2C_LCD_COLS)) {
        *line = 3;
        *col = lcddata - I2C_LCD_LINE3 + 1;
        return true;
    } else if (lcddata >= I2C_LCD_LINE4 && lcddata < (I2C_LCD_LINE4+I2C_LCD_COLS)) {
        *line = 4;
        *col = lcddata - I2C_LCD_LINE4 + 1;
        return true;
    }

	return false;
}

// turn light on/off

void i2c_lcd_light(bool light, uint8_t Dev_ID) {

    if (light) {
        lightOn = 1;
	    i2c_lcd_write_i2c(I2C_LCD_LIGHT_ON, Dev_ID);
    } else {
        lightOn = 0;
    	i2c_lcd_write_i2c(I2C_LCD_LIGHT_OFF, Dev_ID);
    }
}

//-	Check if busy

bool i2c_lcd_busy(uint8_t Dev_ID) {
	uint8_t state;
    
    state = i2c_lcd_getbyte(I2C_LCD_ADDRESS, Dev_ID);
	if (state & (1 << 7)) {
        return true;
    } else {
        return false;
    }
}

/*************************************************************************
Title:		LCD-library
Author:		Christoph Moser <moserchristoph@gmx.at>
File:		lcd-routines.h, v1.0, 2014/09/12
Software:	WinAVR-20100110 ; AVR-GCC 4.3.3 ; avr-libc 1.6.7
Hardware: 	any AVR, tested	ADC0-Port/PIN
Usage:		see Doxygen manual
*************************************************************************/

#ifndef LCD_ROUTINES_H
	#define LCD_ROUTINES_H

/** 
 *  @defgroup moe_LCD LCD Library
 *  @code #include <lcd-routines.h> @endcode
 * 
 *  @brief LCD-library to control a LCD in 4bit-mode
 *
 *	LCD Pins are restricted to one PORT
 *	DataPins must be placed on consecutive pins
 *
 *  @note Based on http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 *  @author Christoph Moser moserch@gmx.at
 *  @version 1.0
 *	Added doxygen comments
 *	@note Tested controllers
 * 	- ATMEGA8
 *	- ATMEGA128
 */
 
 	#ifndef F_CPU
		#define F_CPU 4000000
		#warning F_cPU!
	#endif
 
 /**@{*/
 
/*
** constants and macros
*/

/* @brief Minimum Prescaler of ADC
 *  @param  F_CPU  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *	@see adc_init
 */




 
////////////////////////////////////////////////////////////////////////////////
// Pinbelegung für das LCD, an verwendete Pins anpassen
// Alle LCD Pins müssen an einem Port angeschlossen sein und die 4
// Datenleitungen müssen auf aufeinanderfolgenden Pins liegen
 
//  LCD DB4-DB7 <-->  PORTD Bit PB2-PB5
#define LCD_PORT      PORTB
#define LCD_DDR       DDRB
#define LCD_DB        PB2
 
//  LCD RS      <-->  PORTD Bit PB0     (RS: 1=Data, 0=Command)
#define LCD_RS        PB0
 
//  LCD EN      <-->  PORTD Bit PB1     (EN: 1-Impuls für Daten)
#define LCD_EN        PB1
 
////////////////////////////////////////////////////////////////////////////////
// LCD Ausführungszeiten (MS=Millisekunden, US=Mikrosekunden)
 
#define LCD_BOOTUP_MS           15
#define LCD_ENABLE_US           20
#define LCD_WRITEDATA_US        46
#define LCD_COMMAND_US          42
 
#define LCD_SOFT_RESET_MS1      5
#define LCD_SOFT_RESET_MS2      1
#define LCD_SOFT_RESET_MS3      1
#define LCD_SET_4BITMODE_MS     5
 
#define LCD_CLEAR_DISPLAY_MS    2
#define LCD_CURSOR_HOME_MS      2
 
////////////////////////////////////////////////////////////////////////////////
// Zeilendefinitionen des verwendeten LCD
// Die Einträge hier sollten für ein LCD mit einer Zeilenlänge von 16 Zeichen passen
// Bei anderen Zeilenlängen müssen diese Einträge angepasst werden
 
#define LCD_DDADR_LINE1         0x00
#define LCD_DDADR_LINE2         0x40
#define LCD_DDADR_LINE3         0x14
#define LCD_DDADR_LINE4         0x54



/**
 *	@brief   Initialize LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/
void lcd_init( void );

/**
 *	@brief   Clear LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/
 
////////////////////////////////////////////////////////////////////////////////
// LCD löschen
void lcd_clear( void );


/**
 *	@brief   Initialize LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/

////////////////////////////////////////////////////////////////////////////////
// Cursor in die 1. Zeile, 0-te Spalte
void lcd_home( void );
 
 
 /**
 *	@brief   Initialize LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/
 
////////////////////////////////////////////////////////////////////////////////
// Cursor an eine beliebige Position 
void lcd_setcursor( uint8_t spalte, uint8_t zeile );


/**
 *	@brief   Sends one data byte to LCD
 *
 *	Routine sets one data byte to the actual cursor position
 *	Sets the RS-Pin high -> Data byte
 * 	Transfer 4 data bits
 *	
 *	@param   data	single data byte
 * 	@return  none
*/
void lcd_data( uint8_t data );
 
 
 /**
 *	@brief   Initialize LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/
 
////////////////////////////////////////////////////////////////////////////////
// Ausgabe eines Strings an der aktuellen Cursorposition 
void lcd_string( const char *data );



/**
 *	@brief   Initialize LCD-Display
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/

////////////////////////////////////////////////////////////////////////////////
// Ausgabe eines Strings an Position row und Zeile line
void lcd_string_p( const char *data, unsigned char row,unsigned char line);
 
 
 
 /**
 *	@brief   Write a char to 
 *
 *	Routine sets the initial conditions to initialize a LCD-display
 *	Activates 4bit-Mode for LCD-display
 *	Set all connected Ports from AVR to output
 *	This routine should be called due init-procedure before any data is set to LCD
 *	
 *	@param   none 
 * 	@return  none
*/
 
////////////////////////////////////////////////////////////////////////////////
// Definition eines benutzerdefinierten Sonderzeichens.
// data muss auf ein Array[5] mit den Spaltencodes des zu definierenden Zeichens
// zeigen
void lcd_generatechar( uint8_t code, const uint8_t *data );
 
 
 
 /**
 *	@brief   Sends a comment to LCD
 *
 *	
 *	Sets the RS-Pin low -> Command
 * 	Transfer 4 data bits
 *	
 *	@param   data 
 * 	@return  none
*/
void lcd_command( uint8_t data );

/**
 *	@brief   Convert a long integer to a char
 *	
 *	@param target
 *	@param value 
 * 	@return long2ascii	Converted char
*/
char long2ascii(char *target, unsigned long value);

////////////////////////////////////////////////////////////////////////////////
// LCD Befehle und Argumente.
// Zur Verwendung in lcd_command
 
// Clear Display -------------- 0b00000001
#define LCD_CLEAR_DISPLAY       0x01
 
// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02
 
// Set Entry Mode ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04
 
#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01
 
// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08
 
#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01
 
// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10
 
#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x04
 
// Set Function --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20
 
#define LCD_FUNCTION_4BIT       0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_2LINE      0x08
#define LCD_FUNCTION_5X7        0x00
#define LCD_FUNCTION_5X10       0x04
 
#define LCD_SOFT_RESET          0x30
 
// Set CG RAM Address --------- 0b01xxxxxx  (Character Generator RAM)
#define LCD_SET_CGADR           0x40
 
#define LCD_GC_CHAR0            0
#define LCD_GC_CHAR1            1
#define LCD_GC_CHAR2            2
#define LCD_GC_CHAR3            3
#define LCD_GC_CHAR4            4
#define LCD_GC_CHAR5            5
#define LCD_GC_CHAR6            6
#define LCD_GC_CHAR7            7
 
// Set DD RAM Address --------- 0b1xxxxxxx  (Display Data RAM)
#define LCD_SET_DDADR           0x80
  /**@}*/
#endif 
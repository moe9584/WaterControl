/*************************************************************************
Title:		My Routines
Author:		Christoph Moser <moserchristoph@gmx.at>
File:		my-routines.h, v1.0, 2014/09/08
Software:	WinAVR-20100110 ; AVR-GCC 4.3.3 ; avr-libc 1.6.7
Hardware: 	every AVR		-		-
Description: 	Routines to easier handle of LCD and UART 
Usage:			
*************************************************************************/

#ifndef MY_ROUTINES_H
	#define MY_ROUTINES_H

/** 
 *  @defgroup Common My-Routines
 *  @code #include <my-routines.h> @endcode
 *  @code #include <lcd-routines.h> @endcode
 *  @code #include <uart.h> @endcode
 * 
 *  @brief Routines for easier use
 *
 *  This library can be used to convert values to a string for LCD-UART 
 *
 *  @note Based on http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Analoge_Ein-_und_Ausgabe
 *  @author Christoph Moser moserch@gmx.at
 *  @version 1.0
 *	Update for common use
 */
 
 /**@{*/
 
/**
 *	@brief   Convert a char to a string
 * 
 *	Routine converts a unsigned 8-bit number to a string
 *	8-bit number:	0..2^8-1
 *					0..255
 *
 *  @param number Character
 * 	@param str	String with converted character
 * 	@param digits	Number of digits
 * 	@param zeros	Leading zeros
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
*/
	void mystring(char number, char* str);
	void my_string(char number, char* str, uint8_t digits, uint8_t zeros);
	

/**
 *	@brief   Convert a signed 32-bit value to a string
 * 
 *	Routine converts a signed 32-bit number to a string
 *	32-bit number:	-2^31..2^31-1
 *				-2147483648..+2147483647
 *
 *  @param number 	Signed 32-bit number
 * 	@param string	String with converted number
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
*/
	void my_itoa(int32_t number, char* string);
 
 /**
 *	@brief   Convert a signed 64-bit value to a string
 * 
 *	Routine converts a signed 32-bit number to a string
 *	64-bit number:	-2^63..2^63-1
 *				-9223372036854775808..+9223372036854775808
 *
 *  @param number 	Signed 64-bit number
 * 	@param string	String with converted number
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
*/
	void my_lltoa(int64_t number, char* string);
 
/**
 *	@brief   Round a defined position of a string
 *	
 *	Routine rounds a string after a defined position
 *	The string could be a signed or unsigned
 *	Signed strings:		my_round(my_string+1, 5)
 *	Unsigned strings:	my_roung(my_string,5)
 *	Digit:	1	second digit from left side of string
 *	Digit:	9	Last digti from left side of string
 *
 *  @param string 	Signed or unsigned string
 * 	@param digit Digit where a round should be applied
 * 	@return  none
 *	@warning For signed strings the first char has to be skipped, see describtion.
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik
*/
void my_round(char* string, uint8_t digit);

/**
 *	@brief   Routine sends a string with a defined format to LCD
 * 
 *	Routine sends a unformatted signed or unsigned string to LCD
 *	String generated with Fixed point arithmetic needs a adjustment
 *	to set the start point, the comma and the decimal places
 *	Leading zeros are printed if start<comma
 *  start:	1..11 for signed strings
 *	start:	0..10 for unsigned strings
 *	comma:	comma >= start
 *
 *  @param string 	Signed or unsigned string
 * 	@param start	digit where string should start
 *	@param comma	digit where a virtual komma is placed
 *	@param frac		number of decimal places
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
 *	@see my_print_UART
 *	@see lcd_data
*/
void my_print_LCD(char* string, uint8_t start, uint8_t comma, uint8_t frac);

/**
 *	@brief   Routine formats a signed string
 * 
 *	Routine sends a unformatted signed or unsigned string to UART
 *	String generated with Fixed point arithmetic needs a adjustment
 *	to set the start point, the comma and the decimal places
 *	Leading zeros are printed if start<comma
 *  start:	0..11 for signed strings
 *	comma:	comma >= start
 *	fraction: number of digits
 *
 *  @param string 	Signed or signed string
 * 	@param start	digit where string should start
 *	@param comma	digit where a virtual komma is placed
 *	@param frac		number of decimal places
 *	@param str_new	Formated signed string
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
 *	@see my_print_LCD
 *	@see uart_putc
*/
void my_print_str(char* string, uint8_t start, uint8_t comma, uint8_t frac, uint8_t flag, char* str_new);

/**
 *	@brief   Routine prints a string with a defined format to UART
 * 
 *	Routine sends a unformatted signed or unsigned string to UART
 *	String generated with Fixed point arithmetic needs a adjustment
 *	to set the start point, the comma and the decimal places
 *	Leading zeros are printed if start<comma
 *  start:	1..11 for signed strings
 *	start:	0..10 for unsigned strings
 *	comma:	comma >= start
 *
 *  @param string 	Signed or unsigned string
 * 	@param start	digit where string should start
 *	@param comma	digit where a virtual komma is placed
 *	@param frac		number of decimal places
 * 	@return  none
 *	@see http://www.mikrocontroller.net/articles/Festkommaarithmetik 
 *	@see my_print_LCD
 *	@see uart_putc
*/
void my_print_UART(char* string, uint8_t start, uint8_t comma, uint8_t frac); 

void nr_str(char* string, char* str_new, uint8_t nr_new, uint8_t pos_start, uint8_t pos_end);

uint8_t SIM900_AT_CSQ(char* sig_qual, char*csq_str, uint8_t gsm_rssi);

void SIM900_AT_CREG(signed char gsm_reg, char* gsm_return);

void SIM900_AT_CMTI(signed char sms_nr, char* sms_str, char* gsm_return);

void SIM900_AT_CMGR(char* sms_phone_nr, char* gsm_return);

void SIM900_SMS_Status(char alarm, char* str_AL, char* str_vgrid, char* str_vbatt, char* str_alarm_time, char* str_time, char* sms_msg_status);

extern unsigned char my_wait(unsigned char time, unsigned char sec);

void my_print_time(unsigned char hour, unsigned char min, unsigned char sec, char* str_time, char mode);

/**@}*/

#endif

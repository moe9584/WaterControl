
/**@mainpage SmartFarmMini
 *
 * Multi-purpose Shield v1
 *
 * @section sec1 Features 
 *
 * ATMEGA328 - Fuses lF:0xF7, hF:0xD9, e-F:0x07
 *
 * @section sec2 Pinout of Controller
 * @code - Arduino Uno Mapping (A- Analog in, D Digital)
 * LABEL	ATMEGA	PORT	Shield
 *	D0		2		PD0		RXD				UART-RXD
 *	D1		3		PD1		TXD				UART-TXD
 *	RESET	1		PC6		RESET						ISP-6pin, only use as output
 *	AREF	21				
 *			9		PB6		XTAL1
 *			10		PB7		XTAL2
 *	D2		4		PD2		SW1				
 *	D3		5		PD3		SW2			
 *	D4		6		PD4		DHT11 - control
 *	D5		11		PD5		Buzzer
 *	D6		12	 	PD6		IR-Receiver
 *	D7		13		PD7		Extension pins (GND/5V/D7)	
 *	D8		14		PB0		Extension pins (GND/5V/D8)	
 *	D9		15		PB1		RGB-Led - red
 *	D10		16		PB2		RGB-Led - green
 *	D11		17		PB3		RGB-Led - Blue
 *	D12		18		PB4		Led4
 *	D13		19		PB5		Led3
 *	A0		23		PC0		Poti
 *	A1		24		PC1		LDR			
 *	A2		25		PC2		LM35D					
 *	A3		26		PC3		Extension pins (GND/5V/A3)			
 *	A4		27		PC4		SDA
 *	A5		28		PC5		SCL
 * @endcode
*/
#include <stdbool.h>
#include <stdint.h> 
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h> // itoa
#include <ctype.h> // isdigit
//#include <avr/eeprom.h>
#include "i2c_lcd.h"
#include "i2cmaster.h"
#include "uart.h"
#include "adc-init.h"
#include "my-routines.h"
#include "lcd-routines.h" // all pins must be on one port, support i2c
#include <avr/wdt.h> /*Watchdog timer handling*/


/** constants and macros */
#define I2C_DEV_ID1 0b01000000	// i2c-ID of PCF8574; 	Adress:0100&A2&A1&A0&0; Vdd= 5V, Vss = GND
//#define I2C_DEV_ID1 0b01110000	// i2c-ID of PCF8574T; 	Adress:0111&A2&A1&A0&0; Vdd= 5V, Vss = GND
#define UART_BAUD_RATE 19200// 19200 baud
#define UART_MAXSTRLEN 70


/**@{*/
/* Global variable declaration */
volatile int tc;		// Laufvariable
volatile uint8_t uart_str_complete = 0;     // 1 .. String komplett empfangen
volatile uint8_t uart_str_count = 0;
unsigned char msec = 0;
unsigned char sec = 0;
unsigned char min = 0;
unsigned char hour = 0;
unsigned int day = 0;
char str_msec[4];
char str_sec[3];
char str_min[3];
char str_hour[3];
char str_keys[2];
char str_time[9]; // 9-> hh:mm:ss 13->hh:mm:ss:mss

char uart_string[UART_MAXSTRLEN + 1] = "";
uint8_t i; 
uint8_t j;
uint8_t k;

unsigned char ret;

char buffer[12];
char buffer_new[12];
volatile uint16_t d = 0;

//int32_t e = 0;
//int32_t flow = 0;
//int64_t l_flow = 0;
//int64_t total_flow = 0;
char l_buffer[22];
char l_buffer_new[22];

// ADC-channel
char adc_channel_start = 1; // start with ADC-nr1
char adc_channel_max = 1; // Maximum number of ADC channels
uint32_t adc_results[4]; // results of ADC measurements (ADC0-ADC3)
char adc_res_avg_max = 1; // number of ADC values to create mean value
uint8_t adc_channel; // current ADC_channel
char adc_res_avg; // current ADC run of adc_res_avg_max
uint32_t adc_temp; // Temporary storage register
volatile uint8_t adc_update = 0; // 1...Flag that ADC-result is finished
char adc_restart; // Restart ADC
char adc_string[12];
char adc_run; // indicates that the ADC is in operation
char adc_eval[12]; // string including commas

// Flow-meter
int32_t total_flow;
char int0_update;
char flow_string[12];
char flow_eval[12];

// Flags
char flag_sec = 0;
char flag_min = 0;
char flag_hour = 0;
char flag_day = 0;
char update_lcd = 0;
char update_uart= 0;

/*
** constant definitions
*/


/* EEPROM variable declaration */
 

/* Prototypes */
ISR(TIMER1_OVF_vect)
{	
	TCNT1 = 59286 ; // 1sec = 3036; 
	tc++; // 65536 - 16e6 / 256 = 3036
	// 100 ms counter
}

ISR(INT0_vect)
{
	d++;
	int0_update = 1;
}
ISR(ADC_vect) // Interrupt subroutine for ADC conversion complete
{
	adc_update = 1;
} 

int main(void)
{
	
	adc_init_i(1,1); // AVCC as reference/Enable ADC-interrupt
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei(); // Interrupt based UART-Liberary
	uart_puts("\nUART ready\n");
	
	/* External Interrupt 0 */
	
	//EICRA falling edge generate interrupt request ISC01=1, ISC00=0
	
	//DDRD  &= ~(1 << DDD3); // set as input
	
	PORTD |= (1 << PD3); // internal pullup activated
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC01);
	

	
	/* Timer/Counter 1 */	
	TIMSK1 |= _BV(TOIE1); 		// aktivate overflowinterrupts of timer1 
	TCCR1B |= (1<<CS12);		// prescaler = 256
	TCNT1 = 0xFFFF; 			// FFFF for start 
	
	
	/* LCD-Display */
	DDRD  |= (1 << DDD7); // Set as PIN output
	PORTD |= (1 << PD7); // SET output LOW or deactivate internal Pullup
	_delay_ms(500);
	
	lcd_init();
	lcd_string_p("LCD-ready",0,1);
	_delay_ms(500);
	//PORTD &= ~(1 << PD7); // Light off SET output LOW or deactivate internal Pullup
	lcd_clear();
	lcd_string_p("Time:",0,1); // row/column

	// ADC
	adc_channel = adc_channel_max; // first startup of ADC
	adc_update = 1; // first jump into the ADC routine 
	adc_res_avg = adc_res_avg_max; // Never reached -> Intialisation
	adc_restart = 0;
	adc_run = 0;
	lcd_string_p("bar",13,1); // row/column

	// Flow-meter
	total_flow = 0;
	int0_update = 1;

	// Set Initial values for first output
	mystring(msec,str_msec);
	mystring(sec,str_sec);
	mystring(min,str_min);
	mystring(hour,str_hour);
	
	// Initialize string constants
	// Time-string
	//	h h : m m : s s \0
	//	0 1 2 3 4 5 6 7 8
	str_time[0]='0';
	str_time[1]='0';
	str_time[2]=':';
	str_time[3]='0';
	str_time[4]='0';
	str_time[5]=':';
	str_time[6]='0';
	str_time[7]='0';
	str_time[8]='\0';
	uart_puts(str_time);
	
	while (1)
	{
	/* 0 - Flow Counter*/	
		if (int0_update==1) {
			total_flow = total_flow+d;
			d = 0;
			int0_update=0;
			my_itoa(total_flow,flow_eval);
			//my_round(adc_string,7);
			//my_print_str(adc_string, 0, 11, 0, 1, flow_eval);
			
		}
	/* 1 - Time routine */
		if(!(tc==0)) // one  100msec is gone
		{
			msec= msec+1;
			tc = 0;
			if (msec>=10) { // Every second loop
				msec =0;
				sec = sec+1;
				flag_sec = 1;
				update_lcd = 1;
				update_uart = 1;
				if (sec>=60) { // every minute
					sec = 0;
					min = min+1;
					flag_min = 1;
					if (min>=60) { // every hour
						min = 0;
						hour = hour+1;
						flag_hour = 1;
						if (hour>=24) { // every day
							hour = 0;
							flag_day = 1;
						}	
					}	
				}
			} // 1s loop
		} // 100ms loop - End time update routine	
		
		
	// Do now the not timecritical work
		if(flag_sec==1) {
			mystring(sec,str_sec);
			for(i=0; i<2; i++) {
				str_time[i+6]=str_sec[i];
			}
			adc_restart =1;
			flag_sec = 0;
		}
		if(flag_min==1) {
			mystring(min,str_min);
			for(i=0; i<2; i++) {
				str_time[i+3]=str_min[i];
			}
		
			flag_min = 0;
		}
		if(flag_hour==1) {
			mystring(hour,str_hour);
			for(i=0; i<2; i++) {
				str_time[i]=str_hour[i];
			}
	
			flag_hour = 0;
		}
		if(flag_day==1) {
				
			flag_day = 0;
		}
		
		// Start ADC if needed
		if (adc_restart==1){ // Restart of first ADC channel ADC
			adc_restart = 0;
			adc_read_i(adc_channel); // Start ADC
			adc_run = 1;
		}
		if (adc_run==0) { // Intermediate Start of ADC
			adc_read_i(adc_channel);
			adc_run = 1;
		}
		
		// Check if ADC conversion is finished
		if (adc_update==1) {
			if(adc_res_avg<adc_res_avg_max) {
				if (adc_res_avg>0) {
					adc_results[adc_channel] = adc_results[adc_channel]+ ADCW;
				}
				else {
					adc_results[adc_channel] = ADCW;
				}
			adc_res_avg = adc_res_avg+1;
			}
			else { // Initial condition
				adc_res_avg = 0;
				adc_channel = adc_channel_start;
				adc_restart = 1; // Start the ADC
			}
			if (adc_res_avg == adc_res_avg_max) {
				adc_res_avg = 0; // Clear index of average value
				adc_results[adc_channel] = adc_results[adc_channel]/adc_res_avg_max;
				adc_temp = adc_results[adc_channel]*48876;
				
				my_itoa(adc_temp,adc_string);
				
				//uart_puts(adc_string);
				//uart_puts(" ");
				my_round(adc_string,7);
				my_print_str(adc_string, 3, 4, 1, 1, adc_eval);

				

				//lcd_setcursor(16,adc_channel+1);
				//my_print_LCD(string, 3, 5, 1);
				//my_print_LCD(string, 3, 4, 3);

				adc_channel = adc_channel+1; // Next channel
				if(adc_channel>=(adc_channel_start+adc_channel_max)) {
					adc_channel = adc_channel_start;
					adc_run = 2;
				}
				else {
					adc_run = 0;
				}
				adc_update = 0; // clear flag
				
			}
		}
		// Update the lcd-screen
		if(update_uart==1) {	
			//UART-outputs
			uart_puts(adc_eval);
			uart_puts(" ");
			uart_puts(flow_eval);
			uart_puts("\n");
			uart_puts(str_time);
			
			update_uart=0;
		}
		if(update_lcd==1) {	
			//LCD-outputs
			lcd_string_p(adc_eval,8,1);
			//my_print_LCD(adc_string, 3, 4, 2);
			lcd_string_p(flow_eval,0,2);
			update_lcd=0;
		}
	}
	return 0;
}

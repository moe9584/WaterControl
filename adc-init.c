/*************************************************************************
Title:		ADC-library
Author:		Christoph Moser <moserchristoph@gmx.at>
File:		adc-init.c, v1.0, 2013/06/25
Software:	WinAVR-20100110 ; AVR-GCC 4.3.3 ; avr-libc 1.6.7
Hardware: 	built-in UART	  Tested	ADC0-Port/PIN
			ATINY2313			-		-
			ATMEGA8				OK		PC0/23 - PC5/28
			ATMEGA328			-		-
			ATMEGA32			-		-
			ATMEGA644			-		-
			ATMEGA128			OK		PF0/61 - PF7/54
			ATMEGA2560			-		-
Description:	ADC-Library with variable reference voltage and automatic
				adjust of ADC-Clock (ADPS-Pins) for system Clock 1 - 20 MHz
Usage:			AREF - Voltage reference for adc: 1->5V ; 0->2,56V
*************************************************************************/
	#include <avr/io.h>	
	#include "adc-init.h"

/*************************************************************************
Function: adc_init()
Purpose:  Initialize ADC and set prescaler
Input:    reference voltage
Returns:  none
**************************************************************************/
void adc_init(uint8_t aref) 
{
  uint16_t result;
if(aref==1){
	ADMUX = (0<<REFS1) | (1<<REFS0);      // AVcc als Referenz benutzen
  }
else {
	ADMUX = (1<<REFS1) | (1<<REFS0);      // interne Referenzspannung nutzen
	}
  
  // Bit ADFR ("free running") in ADCSRA steht beim Einschalten
  // schon auf 0, also single conversion

// Set prescaler value  
if(TF_MIN<=16){
	ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);     // Frequenzvorteiler
}
else if(TF_MIN<=32){
	ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
}
else if(TF_MIN<=64){
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);     // Frequenzvorteiler
}
else { // 128 
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
}

  ADCSRA |= (1<<ADEN);                  // ADC aktivieren
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
 
  ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
  while (ADCSRA & (1<<ADSC) ) {}        // auf Abschluss der Konvertierung warten
  /* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
     Wandlung nicht übernommen. */
  result = ADCW;

}/* adc_init */
 
/*************************************************************************
Function: adc_read()
Purpose:  Read a single ADC-value
Input:    Port number
Returns:  ADC-value
**************************************************************************/
uint32_t adc_read( uint8_t channel )
{
  // Kanal waehlen, ohne andere Bits zu beeinflußen
  ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
  ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
  while (ADCSRA & (1<<ADSC) ) {}  // auf Abschluss der Konvertierung warten
  
  return ADCW;                    // ADC auslesen und zurückgeben
  
}/* adc_read */

 /*************************************************************************
Function: adc_read_avg()
Purpose:  Reads several single ADC-values
Input:    Port number, number of readings
Returns:  Mean ADC-value
**************************************************************************/
uint32_t adc_read_avg( uint8_t channel, uint8_t average )
{
  uint32_t result = 0;
 
  for (uint8_t i = 0; i < average; i++ )
    result += adc_read( channel );
 
  return (uint32_t)( result / average );
  
}/* adc_read_avg */

/*************************************************************************
Function: adc_init_i()
Purpose:  Initialize ADC and set prescaler
Input:    reference voltage interrupt or not, single conversion/free running
Returns:  none
**************************************************************************/
void adc_init_i(uint8_t adc_ref, uint8_t adc_int) 
{
	uint16_t result;
// reference voltage
	if(adc_ref==1){
		ADMUX = (0<<REFS1) | (1<<REFS0);      // AVcc als Referenz benutzen
	}
	else {
		ADMUX = (1<<REFS1) | (1<<REFS0);      // interne Referenzspannung nutzen
	}
  
	// Bit ADFR ("free running") in ADCSRA steht beim Einschalten
	// schon auf 0, also single conversion
	
	/*// free running mode -> not available on atmega328 adate...
	if(adc_fr==1) {
		ADCSRA = (1<<ADFR); // free running
		ADATE=1 and ADCSRB=000 with atmega328
	}
	else {
		ADCSRA = (0<<ADFR); // single conversion
	}*/
	
	// Set prescaler value  
	if(TF_MIN<=16){
		ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);     // Frequenzvorteiler
	}
	else if(TF_MIN<=32){
		ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
	}
	else if(TF_MIN<=64){
		ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);     // Frequenzvorteiler
	}
	else { // 128 
		ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
	}

	ADCSRA |= (1<<ADEN);                  // ADC aktivieren
 
  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
 
	ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung 
	while (ADCSRA & (1<<ADSC) ) {}        // auf Abschluss der Konvertierung warten
  /* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
     Wandlung nicht übernommen. */
	result = ADCW;

// Activate interrupt if necessary

	// ADC conversion complete interrupt
	if (adc_int==1) {
		ADCSRA |= (1<<ADIE);
	}
	else {
		ADCSRA |= (0<<ADIE);
	}
}/* adc_init */

/*************************************************************************
Function: adc_read()
Purpose:  Read a single ADC-value
Input:    Port number
Returns:  ADC-value
**************************************************************************/
void adc_read_i( uint8_t channel )
{
  // Kanal waehlen, ohne andere Bits zu beeinflußen
  ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
  ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
  //while (ADCSRA & (1<<ADSC) ) {}  // auf Abschluss der Konvertierung warten
  
  //return ADCW;                    // ADC auslesen und zurückgeben
  
}/* adc_read */


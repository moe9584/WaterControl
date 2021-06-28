/*************************************************************************
Title:		ADC-library
Author:		Christoph Moser <moserchristoph@gmx.at>
File:		adc-init.h, v1.0, 2013/09/01
Software:	WinAVR-20100110 ; AVR-GCC 4.3.3 ; avr-libc 1.6.7
Hardware: 	built-in UART	  Tested	ADC0-Port/PIN
			ATINY2313			-		-
			ATMEGA8				OK		PC0/23 - PC5/28
			ATMEGA328			-		-
			ATMEGA32			-		-
			ATMEGA644			-		-
			ATMEGA128			OK		PF0/61 - PF7/54
			ATMEGA2560			-		-
Usage:			AREF - Voltage reference for adc: 1->5V ; 0->2,56V


Voltage divider:
ADC: 	bit		voltage
		0		0V
		1023	5V = V_ref
						U_ADC = Vcc*(R2/(R1+R2)
     Vcc ----------+                
                   |                
                  ---               
                  | | R1          
                  | |               
                  ---               
                   |               
                   +------- PC0/ADC
                   |                
				  ---
				  | | R2
				  | |
				  ---
                   |                
      GND ---------+                

*************************************************************************/

#ifndef ADC_INIT_H
	#define ADC_INIT_H

/** 
 *  @defgroup moserch_adc ADC Library
 *  @code #include <adc-init.h> @endcode
 * 
 *  @brief ADC library with automatic Clock setting.
 *
 *  This library can be used to read in analog data to any AVR  
 *  ADC-Library with variable reference voltage and automatic
 *  adjust of ADC-Clock (ADPS-Pins) for system Clock 1 - 20 MHz
 *  AREF - Voltage reference are set via the initialize process of ADC
 *
 *  @note Based on http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Analoge_Ein-_und_Ausgabe
 *  @author Christoph Moser moserch@gmx.at
 *  @version 2.0
 *	Added doxygen comments
 *  @version 1.0
 *	Update for common use
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

/** @brief Minimum Prescaler of ADC
 *  @param  F_CPU  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *	@see adc_init
 */
	#define TF_MIN (F_CPU/200000)	// Minimum Clock of ADC
/** @brief Maximum Prescaler of ADC
 *  @param  F_CPU  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *	@see adc_init
 */
	#define TF_MAX (F_CPU/50000)     // Maximum Clock of ADC
	#if (F_CPU<800000)
		#warning Check ADC-Prescaler!
	# endif

/**
*	@brief   Initialize ADC and set voltage reference of ADC
*
*	Routine sets the inital conditions of ADC for a single conversion.
*	Prescaler for the ADC-clock  is set according the system clock.
*	ADC-clock should be between 50 kHz and 200 kHz
*
*   @param   aref reference voltage of ADC
*           - \b 0 Vref = 2,56V
*           - \b 1 Vref = 5V    
* 	@return  none
*	@note Prescaler calculation is valid for system clocks greater than 800 kHz    
*/
	void adc_init(uint8_t aref);
	void adc_init_i(uint8_t adc_ref, uint8_t adc_int);

/**
 *  @brief   Reads single ADC-value and return value
 *
 *  A single ADC-value is read in from 
 * 	a defined (channel) ADC-port and returned back
 *
 *  @param   channel Define ADC-port
 *  @return  adc_read Return value of ADC
 */
	uint32_t adc_read( uint8_t channel );
	
	void adc_read_i( uint8_t channel );

/**
 *  @brief   Reads several ADC-values and return mean value
 *
 *  A defined number(average) of single ADC-values are read in from 
 * 	a defined (channel) ADC-port.
 *	A mean value is calulated and given back.
 *
 *  @param   channel Define ADC-port
 *	@param	 average Number of readings for mean value
 *  @return  adc_read_avg Return value of ADC
*/
	uint32_t adc_read_avg( uint8_t channel, uint8_t average );

/**@}*/

#endif

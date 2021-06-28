/*************************************************************************
Title:		ADC-library
Author:		Christoph Moser <moserchristoph@gmx.at>
File:		my-routines.c, v1.0, 2014/09/09
Software:	WinAVR-20100110 ; AVR-GCC 4.3.3 ; avr-libc 1.6.7
Hardware: 	any-AVR
Description:	ADC-Library with variable reference voltage and automatic
				adjust of ADC-Clock (ADPS-Pins) for system Clock 1 - 20 MHz
Usage:			AREF - Voltage reference for adc: 1->5V ; 0->2,56V
*************************************************************************/
	#include <avr/io.h>
	#include <stdlib.h>
	#include <string.h>
	#include "my-routines.h"
	#include "lcd-routines.h"
	#include "uart.h"
/*************************************************************************
Function: my_string()
Purpose:  Convert a char to a string
Input:    8-bit Number, string
Returns:  none
**************************************************************************/
void mystring( char number, char* str) 
{	/* Einfache Stringerzeugung für hh:mm:ss
	digits...2 => for hour, min, sec
	zeros...0 no zeros replaced, space inserted; 1 => Leading zeros; 2 => At least one zero at the end
	*/
	int8_t i;                           // schleifenzähler

	str[2]='\0';                       // String Terminator
	for(i=1; i>=0; i--) {
		str[i]=(number % 10) +'0';         // Modulo rechnen, dann den ASCII-Code von '0' addieren
		number /= 10;
	}
}

void my_string( char number, char* str, uint8_t digits, uint8_t zeros) 
{	/* Einfache Stringerzeugung für hh:mm:ss
	digits...2 => for hour, min, sec
	zeros...0 no zeros replaced, space inserted; 1 => Leading zeros; 2 => At least one zero at the end
	*/
	int8_t i;                           // schleifenzähler
	uint8_t flag;

  str[digits]='\0';                       // String Terminator
  for(i=(digits-1); i>=0; i--) {
    str[i]=(number % 10) +'0';         // Modulo rechnen, dann den ASCII-Code von '0' addieren
    number /= 10;
  }
  // Leading zeros
	if (zeros>0) {
		flag=0;
		for (i=0; i<digits; i++) {  
			if ((str[i] =='0')&&(flag==0)) {
				str[i]=' ';
			}
			else {
				flag=1;
			}
		}
		if ((flag==0)&&(zeros==2)) {
			str[digits-1] ='0';
		}
	}
}
/*************************************************************************
Function: my_itoa()
Purpose:  Convert a signed 32-bit value to a string
Input:    signed 32-bit number, string
Returns:  none
**************************************************************************/
void my_itoa(int32_t number, char* string) {
  uint8_t i;
 
  string[11]='\0';                  // String Terminator
  if( number < 0 ) {                  // ist die Zahl negativ?
    string[0] = '-';              
    number = -number;
  }
  else string[0] = ' ';             // Zahl ist positiv
 
  for(i=10; i>=1; i--) {
    string[i]=(number % 10) +'0';     // Modulo rechnen, dann den ASCII-Code von '0' addieren
    number /= 10;
  }
}

/*************************************************************************
Function: my_lltoa()
Purpose:  Convert a signed 64-bit value to a string
Input:    signed 64-bit number, string
Returns:  none
**************************************************************************/
void my_lltoa(int64_t number, char* string) {
  uint8_t i;
 
  string[21]='\0';                  // String Terminator
  if( number < 0 ) {                  // ist die Zahl negativ?
    string[0] = '-';              
    number = -number;
  }
  else string[0] = ' ';             // Zahl ist positiv
 
  for(i=20; i>=1; i--) {
    string[i]=(number % 10) +'0';     // Modulo rechnen, dann den ASCII-Code von '0' addieren
    number /= 10;
  }
}

/*************************************************************************
Function: my_round()
Purpose:  Round a defined position of a string
Input:    signed or unsigned string, digit
Returns:  none
**************************************************************************/ 
void my_round(char* string, uint8_t digit) {
  uint8_t i;
  //Funktionsaufruf : my_round(my_string+1, 5);
 
  if (string[digit]>='5') {         // Aufrunden?
    for(i=(digit-1); i>=0; i--) {
      string[i] += 1;               // Aufrunden
      if (string[i]<='9')
        break;                      // kein Übertrag, schleife verlassen
      else
        string[i]='0';              // Übertrag und Überlauf
    }
  }
 
  for(i=digit; i<12; i++) string[i] ='0';   // gerundete Stellen auf Null setzen
}

/*************************************************************************
Function: my_print_LCD()
Purpose:  Routine sends a string with a defined format to LCD
Input:    signed or unsigned string, start, comma, frac
Returns:  none
**************************************************************************/ 
void my_print_LCD(char* string, uint8_t start, uint8_t comma, uint8_t frac) {
 /* Funktionsaufruf: 	char my_string[12]="-0034567891\0";
						my_print_LCD(my_string, 2, 5, 2); 
						LCD: 
					-----------
						ADC: 528
						val = 528*48876 = 25806528
						my_itoa(val,str[12])
						my_round(str+1,4) => 25810000 = "00025810000\0"
						my_print_lcd(str,3,4,3) => 2.581 
						*/
  uint8_t i;            // Zähler
  uint8_t flag=0;       // Merker für führende Nullen
 
  // Vorzeichen ausgeben  
  if (string[0]=='-') lcd_data('-'); 
  
  else lcd_data(' ');
 
  // Vorkommastellen ohne führende Nullen ausgeben
  for(i=start; i<comma; i++) {
    if (flag==1 || string[i]!='0') {
      lcd_data(string[i]);
      flag = 1;
    }
    else {
	if (i==(comma-1)) lcd_data('0');	// Null vor dem Komma
	else lcd_data(' ');					// führende Nullen weg
	}         // Leerzeichen
  }
	if (frac!=0) {		// Wenn komma=0 dann sprung
	
		lcd_data('.');                // Komma ausgeben
		// Nachkommastellen ausgeben
		for(; i<(comma+frac); i++) lcd_data(string[i]);
	}
}
/*************************************************************************
Function: my_print_i2c_LCD()
Purpose:  Routine sends a string with a defined format to i2c-LCD
Input:    signed or unsigned string, start, comma, frac, flag, address
Returns:  none
**************************************************************************/ 
void my_print_str(char* string, uint8_t start, uint8_t comma, uint8_t frac, uint8_t flag, char* str_new) {
/* Funktionsaufruf: 	char my_print_str[12]=" 000500040\0";

						Position: 0    1    2    3    4    5    6    7    8    9   10   11
						char string[12]={' ', '0', '0', '0', '5', '0', '0', '0', '0', '4', '0','\0'};
						my_print_LCD(my_string, 4, 5, 2);
						start = 4... start at position 4 (4th place)
						comma = 5... comma between 4 and 5
						frac = 2.. two digits after comma
						flag = 1...leading zeros, 0... no leading zeros
						Result: " 5.00" */
						
    uint8_t i;          // Counter old string
	uint8_t j;			// Counter new string
	char sep = '.';	// Comma separator

    j = 0;
    /* Sign of string*/

	if ((string[j]=='-')&&(start!=0)) {
		// Negative sign
		str_new[j] = '-';
	}
	else {
		// Positive sign
		str_new[j] = string[j];
	}
	j++;

    /* Start loop after sign
     0123456789
     _000500040
    start = 4
    comma = 5
    frac = 2 */
	if (comma!=0) {
		for(i=start; i<=comma; i++) {
			if (i==comma) {
				// last position before comma
				if (i==1) {
					str_new[j] = '0';
					j++;
					if (frac!=0) {
						str_new[j]= sep;
						j++;
					}
				}
				// More than one rank before comma
				else {
					if(frac!=0) {
						str_new[j]=sep;
					}
					j++;
				}
			}
			else {
				// Values before comma
				if ((string[i]=='0')&&(flag==0)) {
					if (i!=(comma-1)) {
						str_new[j]=' ';
					}
					else {
						str_new[j]='0';
					}
					j++;
				}
				else {
					str_new[j] = string[i];
					j++;
					flag = 1;
				}
				
			}         // Leerzeichen
		}
	}
	else { // Unsigned Integer value
		for(i=start;i<=10;i++) {
            if ((string[i]=='0')&&(flag==0)&&(i<10) ) {
                str_new[j]=' ';
                j++;
            }
			else if ((string[i]=='0')&&(flag==0)) {
                str_new[j]='0';
                j++;
            }
			
            else {
                str_new[j] = string[i];
                j++;
                flag = 1;
            }
		}
	}
	if (frac!=0) {		// Wenn komma=0 dann sprung

		//str_new[i] = ',';                // Komma ausgeben
		// Nachkommastellen ausgeben
		for(i=comma; i<(comma+frac); i++) {
                str_new[j]=string[i];
                j++;
		}
	}
	str_new[j] = '\0';
}
/*************************************************************************
Function: my_print_UART()
Purpose:  Routine sends a string with a defined format to UART
Input:    signed or unsigned string, start, comma, frac
Returns:  none
**************************************************************************/ 
void my_print_UART(char* string, uint8_t start, uint8_t comma, uint8_t frac) {
 /* Funktionsaufruf: 	char my_string[12]="0001640000\0";
						my_print_UART(my_string, 3,4,2); 
						result: 0,16
						start...start at string position 3
						comma...comma is present between position 3 and position 4
						frac... number of decimals after comma */
  uint8_t i;            // Zähler
  uint8_t flag=0;       // Merker für führende Nullen
 
  // Vorzeichen ausgeben  
  if (string[0]=='-') uart_putc('-');
  else uart_putc(' ');
 
  // Vorkommastellen ohne führende Nullen ausgeben
  for(i=start; i<comma; i++) {
    if (flag==1 || string[i]!='0') {
      uart_putc(string[i]);
      flag = 1;
    }
    else {
	if (i==(comma-1)) uart_putc('0');	// Null vor dem Komma
	else uart_putc(' ');					// führende Nullen weg
	}         // Leerzeichen
  }
	if (frac!=0) {		// Wenn komma=0 dann sprung
	
		uart_putc('.');                // Komma ausgeben
		// Nachkommastellen ausgeben
		for(; i<(comma+frac); i++) uart_putc(string[i]);
	}
}
/*************************************************************************
Function: nr_str()
Purpose:  Routine sends a string with a defined format to UART
Input:    signed or unsigned string, start, comma, frac
Returns:  none
**************************************************************************/ 
void nr_str(char* string, char* str_new, uint8_t nr_new, uint8_t start_pos, uint8_t end_pos) {
 /* Funktionsaufruf: 	void nr_str[12]="+CSQ: 23,0\0";
						nr_str(gsm_return, str_new, nr_new, 7,8)
						result: 23 => nr_new / str_new
						pos_start...start at string position 7
						pos_end...end at string position 8 */
						
	//DEC:	43	44	45	46	48	49	50	51	52	53	54	55	56	57
	//ASC:	+	,	-	.	0	1	2	3	4	5	6	7	8	9
						
	uint8_t i,j;
	int16_t temp;
   //      Position:    0    1    2    3    4    5    6    7    8    9    10
    //char string[10]={'+', 'C', 'S', 'Q', ':', ' ', '2', '3', ',', '0', '\0'};
	
    j = 0;
    for (i=start_pos; i<=end_pos; i++) {
         str_new[j]=string[i];
         j++;
    }
    str_new[j]='\0';
    temp = atoi(str_new);
	nr_new = (uint8_t)temp;
}

uint8_t SIM900_AT_CSQ(char* sig_qual, char* csq_str, uint8_t gsm_rssi) {
	/*
		https://www.lte-anbieter.info/Bilder/technik/empfang/asu-2g.png
		
		Value	RSSI	Strength	
		[-]		[dBm]	[%]
		0		-113	0
		1		-111	3
		2		-109	6	
		3		-107	10	
		4		-105	13
		5		-103	16
		6		-101	19
		7		-99		23
		8		-97		26
		9		-95		29
		10		-93		32
		11		-91		35
		12		-89		39
		13		-87		42
		14		-85		45
		15		-83		48
		16		-81		52
		17		-79		55
		18		-77		58
		19		-75		61
		20		-73		65
		21		-71		68
		22		-69		71
		23		-67		74
		24		-65		77
		25		-63		81
		26		-61		84
		27		-59		87
		28		-57		90
		29		-55		94
		30		-53		97
		31		-51		100
		*/
  uint8_t i=0;            // Zähler
  uint8_t j=0;
  uint8_t start=0;
  uint8_t end=0;
  int8_t value=0;
  sig_qual[0] = '\0';
  uint8_t sig_nr=0;
  
// Extract number
    for(i=5; i<=10; i++) {
		if (csq_str[i]==' ') {
			start=i+1;
		}
		if (csq_str[i]==',') {
			end = i-1;
			break;
		}
	}
	j=0;
	for(i=start; i<=end; i++) {
		sig_qual[j]=csq_str[i];
         j++;
	}
    sig_qual[j] = '\0';
	
// Change string to number
	value=atoi(sig_qual);
	gsm_rssi = value;
	
// Convert to signal strength
	switch (value) {
		case 0:
			strcpy( sig_qual, "0" );
			sig_nr = 0;
			break;
		case 1:
			strcpy( sig_qual, "3" );
			sig_nr = 1;
			break;
		case 2:
			strcpy( sig_qual, "6" );
			sig_nr = 1;
			break;
		case 3:
			strcpy( sig_qual, "10" );
			sig_nr = 1;
			break;
		case 4: 
			strcpy( sig_qual, "13" );
			sig_nr = 1;
			break;
		case 5: 
			strcpy( sig_qual, "16" );
			sig_nr = 1;
			break;
		case 6: 
			strcpy( sig_qual, "19" );
			sig_nr = 1;
			break;
		case 7: 
			strcpy( sig_qual, "23" );
			sig_nr = 2;
			break;
		case 8: 
			strcpy( sig_qual, "26" );
			sig_nr = 2;
			break;
		case 9: 
			strcpy( sig_qual, "29" );
			sig_nr = 2;
			break;
		case 10: 
			strcpy( sig_qual, "32" );
			sig_nr = 2;
			break;
		case 11: 
			strcpy( sig_qual, "35" );
			sig_nr = 2;
			break;
		case 12: 
			strcpy( sig_qual, "39" );
			sig_nr = 2;
			break;
		case 13: 
			strcpy( sig_qual, "42" );
			sig_nr = 3;
			break;
		case 14: 
			strcpy( sig_qual, "45" );
			sig_nr = 3;
			break;
		case 15: 
			strcpy( sig_qual, "48" );
			sig_nr = 3;
			break;
		case 16: 
			strcpy( sig_qual, "52" );
			sig_nr = 3;
			break;
		case 17: 
			strcpy( sig_qual, "55" );
			sig_nr = 3;
			break;
		case 18: 
			strcpy( sig_qual, "58" );
			sig_nr = 3;
			break;
		case 19: 
			strcpy( sig_qual, "61" );
			sig_nr = 4;
			break;
		case 20: 
			strcpy( sig_qual, "65" );
			sig_nr = 4;
			break;
		case 21: 
			strcpy( sig_qual, "68" );
			sig_nr = 4;	
			break;		
		case 22: 
			strcpy( sig_qual, "71" );
			sig_nr = 4;
			break;
		case 23: 
			strcpy( sig_qual, "74" );
			sig_nr = 4;
			break;
		case 24: 
			strcpy( sig_qual, "77" );
			sig_nr = 4;
			break;
		case 25: 
			strcpy( sig_qual, "81" );
			sig_nr = 4;
			break;
		case 26: 
			strcpy( sig_qual, "84" );
			sig_nr = 4;
			break;
		case 27: 
			strcpy( sig_qual, "87" );
			sig_nr = 5;
			break;
		case 28: 
			strcpy( sig_qual, "90" );
			sig_nr = 5;
			break;
		case 29: 
			strcpy( sig_qual, "94" );
			sig_nr = 5;
			break;
		case 30: 
			strcpy( sig_qual, "97" );
			sig_nr = 5;
			break;
		case 31: 
			strcpy( sig_qual, "100" );
			sig_nr = 5;
			break;
	}
	return sig_nr;
}


void SIM900_AT_CREG(signed char gsm_reg, char* gsm_return) {
  uint8_t i=0;            // Zähler
  uint8_t start=0;
  char reg_str[2];
  int8_t val;

// Extract number
    for(i=8; i<=10; i++) {
		if (gsm_return[i]==',') {
			start=i+1;
			break;
		}
	}
	reg_str[0]=gsm_return[start];
	reg_str[1]='\0';
	
// Change string to number
	val=atoi(reg_str);
	gsm_reg = val;
}

unsigned char my_wait(unsigned char time, unsigned char sec) {
	unsigned char res;
	if(time+sec>60) {
		res=(time+sec)-60;
	}
	else {
		if (time+sec==60) {
			res=0;
		}
		else {
			res = time+sec;
		}
	}
	return res;
}
void my_print_time(unsigned char hour, unsigned char min,unsigned char sec, char* str_time,char mode) {
/* 	mode=0 => Print "hh:mm:ss"
	mode=1 => Print "hh:mm"
*/
	uint8_t i;
	char str[2];
		
	// Hour
	mystring(hour,str);				
	for(i=0; i<2; i++) {
		str_time[i]=str[i];
	}
	// Minute
	str_time[2]=':';
	mystring(min,str);			
	for(i=0; i<2; i++) {
		str_time[i+3]=str[i];
	}
	// Second
	if (mode==0) {
		str_time[5]=':';
		mystring(sec,str);
		for(i=0; i<2; i++) {
			str_time[i+6]=str[i];
		}
	}
	else {
		str_time[5] = '\0';
	}
}
void SIM900_AT_CMTI(signed char sms_nr, char* sms_str, char* gsm_return) {
  int8_t val;
// Extract number, start with 12, check if number greater than 9
	if (gsm_return[14]!='\0') {
		sms_str[0] = gsm_return[12];
		sms_str[1] = gsm_return[13];
		sms_str[2] = gsm_return[14];
		sms_str[3] = '\0';
		}
	else if (gsm_return[13]!='\0') {
		sms_str[0] = gsm_return[12];
		sms_str[1] = gsm_return[13];
		sms_str[2] = '\0';
		}
	else {
		sms_str[0] = gsm_return[12];
		sms_str[1] = '\0';
		}
	
// Change string to number
	val=atoi(sms_str);
	sms_nr = val;
}
void SIM900_AT_CMGR(char* sms_phone_nr, char* gsm_return) {
  uint8_t i=0;            // Zähler
  uint8_t j=0;
  uint8_t start=0;
// +CMGR: "REC UNREAD","+436644233412","","19/12/27,12:00:21+04"<\r><\n>
	// Start of number
    for(i=10; i<=40; i++) {
		if (gsm_return[i]=='+') {
			start=i;
			break;
		}
	}
	// Start Copy String
	j=0;
	for(i=start; i<=100; i++) {
		if (gsm_return[i]==0x22) {
			sms_phone_nr[j]='\0';
			break;
		}
		else {
			sms_phone_nr[j]= gsm_return[i];
		}
		j=j+1;
	}
}
void SIM900_SMS_Status(char alarm, char* str_AL, char*str_vgrid, char* str_vbatt, char* str_alarm_time, char* str_time, char* sms_msg_status) {
	//char str_alarm[2];
	//int8_t i;
	//uint8_t AL_nr;						// schleifenzähler
	char str_batt[12];
	char str_grid[12];

	if (alarm==0) {
	
	
		my_print_str(str_vbatt, 4, 6, 1, 0,str_batt);
		my_print_str(str_vgrid, 4, 6, 1, 0,str_grid);
		strcpy(sms_msg_status, "sim900 bereit");
		strcat(sms_msg_status, " @ ");
		strcat(sms_msg_status, str_time);
		strcat(sms_msg_status, "; Bat: ");
		strcat(sms_msg_status, str_batt);
		strcat(sms_msg_status, "V; Netz: ");
		strcat(sms_msg_status, str_grid);
		strcat(sms_msg_status, "V");
	}
	else {
		//strcpy(sms_msg_status, "sim900 ALARM");
		
		/*AL_nr = alarm;
		str_alarm[1]='\0';                       // String Terminator
		for(i=1; i>=0; i--) {
			str_alarm[i]=(AL_nr % 10) +'0';         // Modulo rechnen, dann den ASCII-Code von '0' addieren
			AL_nr /= 10;
		}*/
	
		strcpy(sms_msg_status, "sim900 ALARM Eingang: ");
		strcat(sms_msg_status, str_AL);
		strcat(sms_msg_status, " um ");
		strcat(sms_msg_status, str_alarm_time);
	}
}
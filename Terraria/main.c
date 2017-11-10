/* Name: main.c
 * Author: <Mig og dig>
 * Copyright: <MINE!! Mine alone!>
 * License: <what licence>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string>
#include "displays.h"

void initialize(){
	DDRC = 0xFF; //alle GPIO på port D sat til outputs, dette er vores output tal
	DDRD = 0b00010000; // PC5+Pc4 sat til input
	DDRB = 0x00;
}

volatile int choices[4] = {zero,two,three,four};

/*
eksempel på hvordan dette ændres:
choices[0] = A;
*/

/*
genstande der skal integreres:

Humidity sensor (B1)
Mist maker (B0)
Blæser (2x) (B2, B3)
Krystal (B6, B7)

*/

int main(void)
{
	initialize();

	while(1)
	{ //for eeeeeevigt!!
		//-------------------------------------------
		//humidity sensor:

		//-------------------------------------------
		//Mist maker:

		//-------------------------------------------
		//Blæsere:

		//-------------------------------------------
		//Krystal:

		//-------------------------------------------
		//displays der skifter og viser tal/bogstaver:
		for(int i=0;i<4;i++)
		{
			DDRC = displays[i];
			PORTD = NUMBERS[choices[i]];
			_delay_ms(1);
		}
		//--------------------------------------------
		//Knapper og interface:
		
		//--------------------------------------------
	}
}

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
	DDRD = 0xFF; //alle GPIO på port D sat til outputs, dette er vores output tal
	DDRC = 0b00010000; // PC5+Pc4 sat til input
	DDRB = 0x0F;
}

volatile int choices[4] = {zero,two,three,four};

/*
eksempel på hvordan dette ændres:
choices[0] = A;
*/

int main(void)
{
	initialize();

	while(1)
	{ //for eeeeeevigt!!

		//-------------------------------------------
		//displays der skifter og viser tal/bogstaver
		for(int i=0;i<4;i++)
		{
			DDRC = displays[i];
			PORTD = NUMBERS[choices[i]];
			_delay_ms(1);
		}
		//--------------------------------------------
	}
}

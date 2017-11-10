/* Name: main.c
 * Author: <Mig og dig>
 * Copyright: <MINE!! Mine alone!>
 * License: <what licence>
 */

#include <avr/io.h>
#include <util/delay.h>
#include "displays.h"

void initialize(){
	DDRD = 0xFF; //alle GPIO på port D sat til outputs, dette er vores output tal
	DDRC = 0b00010000; // PC5+Pc4 sat til input
	//DDRB = 0x00;
}

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
	volatile int choices[4] = {NUMBERS[0],NUMBERS[0],NUMBERS[0],NUMBERS[0]};
	/*
	eksempel på hvordan dette ændres:
	choices[0] = A;
	*/

	volatile int k=0;
	volatile int n=1;

	while(1)
	{
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
		for(int i=0;i<2;i++)
		{
			DDRC = DISPLAYS[i];
			PORTD = choices[i];
			k++;
			_delay_ms(1);
		}
		if(k>1000)
		{
			if(n<=9)
			{
				choices[0]=choices[1];
				choices[1]=NUMBERS[n];
				n++;
			}
			else if(n>9)
			{
				choices[0]=choices[1];
				choices[1]=ALPHABET[n-10];
				n++;
			}
			if(n>35)
			{
				n=0;
			}
			k=0;
		}
		/*
		for(int i=0;i<26;i++)
		{
			DDRC = displays[i];
			PORTD = NUMBERS[choices[i]];
			_delay_ms(1);
		}
		*/
		//--------------------------------------------
		//Knapper og interface:

		//--------------------------------------------
	}
}

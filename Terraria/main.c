/* Name: main.c
 * Author: <not Steen Grøntved>
 * Copyright: <MINE!! Mine alone!>
 * License: <what licence>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string>

void initialize(){
	DDRD = 0xFF; //alle GPIO på port D sat til outputs, dette er vores output tal
	DDRC = 0b00010000; // PC5+Pc4 sat til input
	DDRB = 0x0F;
} //prototype

#define zero 0b00111111
#define one 0b00000110
#define two 0b01011011
#define three 0b01001111
#define four 0b01100110
#define five 0b01101101
#define six 0b01111101
#define seven 0b00000111
#define eight 0b01111111
#define nine 0b01100111

/*

Sådan skrives det:
 ob --hgfedcba--
 som giver:

*  aaaaa
* f     b
* f     b
* f     b
*  ggggg
* e     c
* e     c
* e     c
*  ddddd  hh
*/

volatile int numbers[10] = {zero,one,two,three,four,five,six,seven,eight,nine};

#define fourth 0b00000111
#define third 0b00001011
#define second 0b00001101
#define first 0b00001110

volatile int displays[4] = {first,second,third,fourth};

volatile int choices[4] = {zero,two,three,four};

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
			PORTD = numbers[choices[i]];
			_delay_ms(1);
		}
		//--------------------------------------------
	}
}

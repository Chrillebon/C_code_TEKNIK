//Bibliotek over tal/bogstaver

//Af Christian Mikkelstrup
//Public domain

/*
Sådan skrives det:
 0b --hgfedcba--
 som giver:

*  aaaaaa
* f      b
* f      b
* f      b
*  gggggg
* e      c
* e      c
* e      c
*  dddddd  hh
*/
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define A 0b01110111
#define B 0b01111100
#define C 0b00111001
#define D 0b01011110
#define E 0b01111001
#define F 0b01110001
#define G 0b00111101
#define H 0b01110100
#define I 0b00110000
#define J 0b00011110
#define K 0b01110101
#define L 0b00111000
#define M 0b00010101
#define N 0b00110111
#define O 0b00111111
#define P 0b01110011
#define Q 0b01100111
#define R 0b00110011
#define S 0b01101101
#define T 0b01111000
#define U 0b00111110
#define V 0b00011100
#define W 0b00101010
#define X 0b01110110
#define Y 0b01101110
#define Z 0b01011011

int NUMBERS[10] = {
		0b00111111,
		0b00000110,
		0b01011011,
		0b01001111,
		0b01100110,
		0b01101101,
		0b01111101,
		0b00000111,
		0b01111111,
		0b01100111
};

int SIGN[3] = {
		0b10000000,
		0b01000000,
		0b00010000
};

int ALPHABET[26] = {
	  A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

//noter til displays:
//sæt 0. pin til jord i første display osv.
//gem dette som DDRX = position
//Hvor X er en gruppe (A, B, C, D) og position er en af de 4:

#define DISPLAY_FIRST 0b00010000
#define DISPLAY_SECOND 0b00001000
#define DISPLAY_THIRD 0b00000100
#define DISPLAY_FOURTH 0b00000010

int DISPLAYS[4] = {DISPLAY_FIRST, DISPLAY_SECOND, DISPLAY_THIRD, DISPLAY_FOURTH};

//Til personlig brug i vores kode:
char *SoonToBe;
//Sætter alle i SoonToBe til at være ingenting i displayet.
int choices[4];
/*
eksempel på hvordan dette ændres:
choices[0] = A;
*/

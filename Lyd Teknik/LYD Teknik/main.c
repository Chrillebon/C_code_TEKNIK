/* Name: main.c for LYD
 * Author: Christian Mikkelstrup
 * Copyright: Use of code has to be accepted through the author
 * License: Use for inspiration, and use common sense
 */

#include <avr/io.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include "displays.h"
#include <avr/interrupt.h>

#define C2 1000/65.41
#define Cs2 1000/69.30
#define D2 1000/73.41
#define Ds2 1000/77.78
#define E2 1000/82.41
#define F2 1000/87.31
#define Fs2 1000/92.50
#define G2 1000/98.00
#define Gs2 1000/103.83
#define A2 1000/110.00
#define As2 1000/116.54
#define B2 1000/123.47

#define C3 1000/130.81
#define Cs3 1000/138.59
#define D3 1000/146.83
#define Ds3 1000/155.56
#define E3 1000/164.81
#define F3 1000/174.61
#define Fs3 1000/185.00
#define G3 1000/196.00
#define Gs3 1000/207.65
#define A3 1000/220.00
#define As3 1000/233.08
#define B3 1000/246.94

#define C4 1000/261.63
#define Cs4 1000/277.18
#define D4 1000/293.67
#define Ds4 1000/311.13
#define E4 1000/329.63
#define F4 1000/349.23
#define Fs4 1000/369.99
#define G4 1000/392.00
#define Gs4 1000/415.30
#define A4 1000/440.00
#define As4 1000/466.16
#define B4 1000/493.88

#define C5 1000/523.25
#define Cs5 1000/554.37
#define D5 1000/587.33
#define Ds5 1000/622.25
#define E5 1000/659.26
#define F5 1000/698.46
#define Fs5 1000/739.99
#define G5 1000/783.99
#define Gs5 1000/830.61
#define A5 1000/880.00
#define As5 1000/932.33
#define B5 1000/987.77

const int nodelib[] =
{
	C2, Cs2, D2, Ds2, E2, F2, Fs2, G2, Gs2, A2, As2, B2,
	C3, Cs3, D3, Ds3, E3, F3, Fs3, G3, Gs3, A3, As3, B3,
	C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
	C5, Cs5, D5, Ds5, E5, F5, Fs5, G5, Gs5, A5, As5, B5
};

const char* nodewrite[] =
{
	"C2", "Cs2", "D2", "Ds2", "E2", "F2", "Fs2", "G2", "Gs2", "A2", "As2", "B2",
	"C3", "Cs3", "D3", "Ds3", "E3", "F3", "Fs3", "G3", "Gs3", "A3", "As3", "B3",
	"C4", "Cs4", "D4", "Ds4", "E4", "F4", "Fs4", "G4", "Gs4", "A4", "As4", "B4",
	"C5", "Cs5", "D5", "Ds5", "E5", "F5", "Fs5", "G5", "Gs5", "A5", "As5", "B5",
	"NONE"
};

volatile int Global_Time = 0;
volatile int t = 0;
volatile int this = 0;
volatile int globsize = 0;
volatile float tone = 1.0;
volatile int on = 0;
volatile int status = 0;
volatile int data1 = 0;
volatile int data2 = 0;
volatile int no_sound = 0;
volatile int write = 48;

void initialize(){
	DDRC = 0xFF; //alle GPIO på port D sat til outputs, dette er vores output tal
	DDRB = 0x00; //Test af knap
	DDRD = 0b00111110;

	//Klar til MIDI:
	UBRRH = 0; //MSB for denne udregning \/
	UBRRL = 31; //16K/(16*31250) -1 = 31
	UCSRB = (1<<RXCIE) | (1<<RXEN); //Flag for interupt af data | USART reciever
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); //For at vi skriver til UCSRC | Sætter bitstørrelsen til 8

	//Klar til timer:
	TCCR0 |= (1<<CS02) | (1<<CS00); // sæt prescaler til 1024
	TCNT0 = 0; // sætter timeren til 0*/
	TIMSK |= (1<<TOIE0); // t

	sei(); //global interrupts
}

ISR(TIMER0_OVF_vect)//overflow interrupt funktion:
{
	//Gør intet nu
}

ISR(USART_RXC_vect) //Interrupt når der er data klar i buffer
{
	if(UDR >> 7)
	{
		if(status >= 130) //skal gerne være 144, man trykker på noget
		{
			no_sound = 0;
			data1 = 0;
			data2 = 0;
		}
		status = UDR;
		UDR = 0; //forsøger at rydde op i UDR
		data1 = 0;
		data2 = 0;
	}
	else if(data1 == 0) //anden byte
	{
		if(status >= 130) //skal gerne være 144
		{
			data1 = UDR;
			if(UDR < 36 | UDR > 83)//Fastsætter hvad der skal vises på displayet
			{
				write = 48;
			}
			write = UDR-36;
		}
		else if(UDR == data1)
		{
			no_sound = 1;
		}
	}
	else //tredje byte
	{
		if(status == 144)
		{
			data2 = UDR;
		}
	}
}

int miditone(int midi)
{
	if(midi < 36 | midi > 83)
	{
		return 0;
	}
	return nodelib[midi-36];
}

void rensinput() // renser input, så vi kan bruge det
{
	for(int i = 0; i < globsize; i++)
	{
		//mellemrum bliver oversat
		if(SoonToBe[i] == ' ')
		{
			SoonToBe[i] = 0b00000000;
		}
		//bindestrej bliver oversat
		if(SoonToBe[i] == '@')
		{
			SoonToBe[i] = 0b01000000;
		}
		//indsæt flere her hvis behov:
		if(SoonToBe[i] == ',')
		{
			SoonToBe[i] = 0b00010000;
		}
	}
}

int givemechar(char tegn) //Brugt i forbindelse med display
{
	if(tegn == 44){ return 16; }
	if(tegn<64)
	{
		return NUMBERS[tegn-48];
	}
	if(tegn==64) return 64;
	return ALPHABET[tegn-65];
}

void display(int t) //Oversættelse fra hvad man skal læse til hvad der står på displayet
{
	//løkker der går gennem de fire cifre på displayet
	for(int i = 0; i < 4; i++)
	{
		//sætter dette ciffer til at være hvad der står i variablen
		//Så at det ser ud somom at det bevæger sig.
		char local = SoonToBe[t+i];
		if(t+i > globsize+3)
		{
			char local = SoonToBe[t-globsize+i-4];
			choices[i] = givemechar(local);

		}
		else if(local <= 43 || local >= 100)
		{
			choices[i] = 0b00000000;
		}
		else
		{
			char local = SoonToBe[t+i];
			choices[i] = givemechar(local);
		}
	}
}

void Light_It_Up(int i)
{
	rensinput(); //2*globsize
	PORTC = PORTC | 0b01111111;
	PORTD = PORTD & 0b11100001;
	PORTD = PORTD | DISPLAYS[i];
	PORTC = PORTC & ((~ choices[i]) | 0b10000000);
	//Timere der tæller opad
}

int size(char Input[])
{
	int size=0;
	for(int i=0;i<100;i++)
	{
		char tegn = Input[i];
		if(tegn>0)
		{
			size++;
		}
		else
		{
			return size;
		}
	}
	return 0;
}

volatile float what = 0.01;


int main(void)
{
	initialize();

	while(1)
	{
		char SoonSoonToBe[5] = nodewrite[write]; //Skriver hvilken tone det er, eller NONE;
		SoonToBe = SoonSoonToBe;
		globsize = 4;//size(SoonToBe);
		/*
		Udregning af delay cycles:

		Udregning af delay:
		timer count = delay / (clock time * prescaler)

		Hvor længe kan jeg bruge 256 prescaler:
		250 * (256 * 1/16 000 000) = 4
		Dette er alle toner i oktav 3 eller lavere.
		*/

		if(no_sound) //Hvis den ikke skal spille noget lyd
		{
			PORTD = PORTD & 0b11011111;
			TCNT0 = 0;
			on = 1;
		}
		if(TCNT0 >= (miditone(data1)/2)/(1024*1/16000))//(delay/2)/(pre*1/16000) , deler med 2, da delayet er for både tænd og sluk
		{
			if(on)
			{
				PORTD = PORTD | 0b00100000;
				TCNT0 = 0;
				on = 0;
			}
			else
			{
				PORTD = PORTD & 0b11011111;
				TCNT0 = 0;
				on = 1;
			}
		}

		if(miditone(data1) == 0) //Skal heller ikke spille lyd
		{
			PORTD = PORTD & 0b11011111;
			TCNT0 = 0;
			on = 1;
		}

		display(t); //Gør klar til hvad der skal stå på display

		Light_It_Up(this); //Tænder rent faktisk lyset i displays

		if(Global_Time > 1.5*1000) //rykker bogstaver og tal på display efter tid
		{
			if(globsize>4) t++;
			Global_Time = 0;
		}

		//Går igennem alle mulige bogstaver/tal til der er blankt display, og starter igen.
		if(t > globsize+3)
		{
			t = 0;
		}

		//Tæller display op
		dis++;
		if(dis>=4)
		{
			this = 0;
		}
	}
	return 0;
}

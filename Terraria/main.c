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
	DDRB = 0x00;
	DDRA = 0x00; //alle knapperne
}

/*
genstande der skal integreres:

Humidity sensor (B1)
Mist maker (B0)
Blæser (2x) (B2, B3)
Krystal (B6, B7)
Knappper (8 stk. A0-7) - lys, fugt, luft, status, reset, op, indst., ned

*/

// renser input, så vi kan bruge det
void rensinput()
{
	for(int i = 0; i < strlen(SoonToBe); i++)
	{
		//mellemrum bliver oversat
		if(SoonToBe[i] == ' ')
		{
			SoonToBe[i] = 0b00000000;
		}
		//bindestrej bliver oversat
		if(SoonToBe[i] == '-')
		{
			SoonToBe[i] = 0b01000000;
		}
		//indsæt flere her hvis behov:
	}
}

//Oversættelse fra hvad man skal læse til hvad der står på displayet
void display(int t)
{
	int v = 0;
	//løkker der går gennem de fire cifre på displayet
	for(int i = 0; i < 3; i++)
	{
		//sætter dette ciffer til at være hvad der står i variablen
		//Så at det ser ud somom at det bevæger sig.
		choices[i] = SoonToBe[t+i];
	}
}

int main(void)
{
	initialize();

	volatile int Global_Time=0;
	volatile int Global_Time_Keeper = 0;
	volatile int Time_Days = 0;
	volatile int n=1;
	volatile int t = 0;
	volatile int Time = 0;

	//Status til knapper:
	volatile int Status_Status = 0;
	volatile int Status_Light = 0;
	volatile int Status_Humid = 0;
	volatile int Status_Air = 0;
	volatile int Status_Setting = 0;

	volatile int Size_Status = 0;
	//Dage siden start
	volatile int Size_Light = 16;
	//Timer 0-24
	volatile int Size_Humid = 50;
	//Procent 0-100
	volatile int Size_Air = 0;
	//Størrelse fra 0-10


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
			display(t);
			DDRC = DISPLAYS[i];
			PORTD = choices[i];
			Time++;
			Global_Time++;
			Time_Days++;
			_delay_ms(1);
		}
		//test af bibliotek-----------------------------------
		/*if(Global_Time>1000)
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
			Global_Time=0;
		}*/
		//--------------------------------------------
		//Knapper og interface:

		//Hvad alle knapper gør (0-7):

		if(PINA == 0b00000001 && Status_Status != 1)
		{
			//Lys
			Status_Light = 1;
			Status_Humid = 0;
			Status_Air = 0;
			Time = 0;
		}
		if(PINA == 0b00000010 && Status_Status != 1)
		{
			//Fugt
			Status_Humid = 1;
			Status_Air = 0;
			Status_Light = 0;
			Time = 0;
		}
		if(PINA == 0b00000100 && Status_Status != 1)
		{
			//Luft
			Status_Air = 1;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
		}
		if(PINA == 0b00001000)
		{
			//Status
			if(Status_Status == 2)
			{
				Size_Status = 0;
			}
			else
			{
				Status_Status = 1;
				Status_Air = 0;
				Status_Humid = 0;
				Status_Light = 0;
			}
			Time = 0;
		}
		if(PINA == 0b00010000)
		{
			//Reset
			Status_Status = 0;
			Status_Air = 0;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
			//sætter alle størrelser til standard:
			Size_Status = 0;
			//Uger siden start
			Size_Light = 16;
			//Timer 0-24
			Size_Humid = 50;
			//Procent 0-100
			Size_Air = 0;
			//Størrelse fra 0-10
		}
		if(PINA == 0b00100000)
		{
			//Op
			if(Status_Air == 2)
			{
				Size_Air += 1;
			}
			if(Status_Humid == 2)
			{
				Size_Humid += 5;
			}
			if(Status_Light == 2)
			{
				Size_Light += 1;
			}
			Time = 0;
		}
		if(PINA == 0b01000000)
		{
			//Indstillinger
			Status_Setting = 1;
			Time = 0;
		}
		if(PINA == 0b10000000)
		{
			//Ned
			if(Status_Air == 2)
			{
				Size_Air -= 1;
			}
			if(Status_Humid == 2)
			{
				Size_Humid -= 5;
			}
			if(Status_Light == 2)
			{
				Size_Light -= 1;
			}
			Time = 0;
		}
		if(Status_Status)
		{
			//Display status
			memset(SoonToBe,0x00,strlen(SoonToBe));
			SoonToBe = "HOEST OM " << 8*7-Size_Status << " DAGE";
			Time = 0;
		}
		//undersøger om man forsøger at ændre en værdi
		if(Status_Setting)
		{
			//finder hvilken værdi man vil ændre
			//Ændrer herefter deres værdier
			if(Status_Air)
			{
				Status_Air = 2;
				Status_Humid = 0;
				Status_Light = 0;
			}
			if(Status_Humid)
			{
				Status_Humid = 2;
				Status_Air = 0;
				Status_Light = 0;
			}
			if(Status_Light)
			{
				Status_Light = 2;
				Status_Air = 0;
				Status_Humid = 0;
			}
			Time = 0;
		}
		else
		{
			if(Status_Air)
			{
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "LUFT - "<< Size_Air << " UD AF "<<NUMBERS[1]<<NUMBERS[0];
			}
			if(Status_Humid)
			{
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "FUGT - "<< Size_Humid << " PROCENT";
			}
			if(Status_Light)
			{
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "LYS - "<<Size_Light<< " TIMER OM DAGEN";
			}
			rensinput();
		}

		//Slut på hvad knapperne gør

		//Sørger for, at ingen værdier kan være uden for "range"
		if(Size_Air < 0)
		{
			Size_Air = 0;
		}
		if(Size_Air > 10)
		{
			Size_Air = 10;
		}
		if(Size_Humid < 0)
		{
			Size_Humid = 0;
		}
		if(Size_Humid > 100)
		{
			Size_Humid = 100;
		}
		if(Size_Light < 0)
		{
			Size_Light = 0;
		}
		if(Size_Light > 24)
		{
			Size_Light = 24;
		}

		//dage tæller op:
		if(Time_Days >= 1000*60*24)
		{
			Size_Status++;
			Time_Days = 0;
		}

		//Hvis det er tid til at høste:

		if(8*7-Size_Status <= 0)
		{
			Status_Status = 2;
		}

		//counters her nedenunder:

		//Hvis man har været IDLE for længe resetter den knapper man har trykket på:
		if(Time > 1000*7.5)
		{
			if(Status_Status == 2)
			{
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "HOEST NU  -  TRYK PÅ STATUS ELLER RESET NAAR DETTE ER GJORT  -  HOEST NU";
				rensinput();
			}
			else
			{
				Status_Status = 0;
				memset(SoonToBe,0x00,strlen(SoonToBe));
			}
			Status_Air = 0;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
		}

		//counter der holder styr på hvor langt bogstaverne i den tekst man skal skrive er kommet
		//Alle rykker en plads hver 0.75 sek.
		if(Global_Time_Keeper > 0.75*1000)
		{
			t++;
			Global_Time_Keeper = 0;
		}

		//Går igennem alle mulige bogstaver/tal til der er blankt display, og starter igen.
		if(t > strlen(SoonToBe)+3)
		{
			t = 0;
		}

		//Slut på interface og knapper
		//--------------------------------------------
	}
}

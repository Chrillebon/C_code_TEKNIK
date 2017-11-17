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

//Sørger for, at ingen værdier kan være uden for "range"
void range()
{
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
}

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
	volatile long Time_Days = 0;
	volatile int n=1;
	volatile int t = 0;
	volatile int Time = 0;

	//Status til knapper:
	volatile int Status_Status = 0;
	volatile int Status_Light = 0;
	volatile int Status_Humid = 0;
	volatile int Status_Air = 0;
	volatile int Status_Setting = 0;
	volatile int Status_Reset = 0;

	volatile int Size_Status = 0;
	//Dage siden start
	volatile int Size_Light = 16;
	//Timer 0-24
	volatile int Size_Humid = 50;
	//Procent 0-100 - hvad vi vil have at fugtigheden er
	volatile int Size_Humid_Sensor = 0;
	//Procent 0-100 - hvad vi måler at luftfugtigheden skal være
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

		for(int i=0;i<3;i++)
		{
			display(t);
			DDRC = DISPLAYS[i];
			PORTD = choices[i];
			//Timere der tæller opad
			Time++;
			Global_Time++;
			Time_Days++;
			_delay_ms(1);
		}
		//--------------------------------------------
		//Knapper og interface:

		//Hvad alle knapper gør (0-7):

		if(PINA == 0b00000001)
		{
			//Lys
			Status_Light = 1;
			Status_Humid = 0;
			Status_Air = 0;
			Time = 0;
		}
		if(PINA == 0b00000010)
		{
			//Fugt
			Status_Humid = 1;
			Status_Air = 0;
			Status_Light = 0;
			Time = 0;
		}
		if(PINA == 0b00000100)
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
			//Denne aktiverer kun, hvis det er tid til at høste
			if(Status_Status == 2)
			{
				Size_Status = 0;
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "HOEST FAERDIG - HOEST IGEN OM 8 UGER";
				rensinput();
			}
			//alle andre gange:
			else
			{
				Status_Status = 1;
				Status_Air = 0;
				Status_Humid = 0;
				Status_Light = 0;
			}
			Time = 0;
		}

		//Det næste er ikke en knap, men hører med til "reset" knappen.
		//Sørger for, at der går tid fra man trykker på reset første gang til at man kan hard resette.
		if(Status_Status == 1 && Status_Reset == 1)
		{
			//forsinker i 1/3 af et sekund, så man forhåbentligt ved hvad man laver/har fået en advarsel
			_delay_ms(333);
		}
		if(PINA == 0b00010000)
		{
			//Reset
			if(Status_Status == 1 && Status_Reset == 0)
			{
				Status_Reset = 1;
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "TRYK IGEN FOR HARD RESET";
				rensinput();
			}
			//Nu har man trykket to gange på reset under indstillinger og den er blevet hard resat:
			if(Status_Status == 1 && Status_Reset == 1)
			{
				Size_Status = 0;
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "HARD RESET FAERDIG";
				rensinput();
			}
			else
			{
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "RESET FAERDIG";
				rensinput();
			}
			//Hvis det er tid til at høste
			if(Status_Status == 2)
			{
				Time_Days = 0;
				Size_Status = 0;
				//Sørger for, at status ikke aktiverer længere nede:
				Status_Status = 0;
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = "HOEST FAERDIG - HOEST IGEN OM 8 UGER";
				rensinput();
			}
			//i alle andre tilfælde:
			else
			{
				Status_Status = 0;
				Status_Air = 0;
				Status_Humid = 0;
				Status_Light = 0;
				Global_Time=0;
				Global_Time_Keeper = 0;
				t = 0;
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
			Time = 0;
		}
		if(PINA == 0b00100000)
		{
			//Op
			if(Status_Air == 2)
			{
				//Justerer størrelsen af variablen
				Size_Air += 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Air << " - LUFT UD AF "<<NUMBERS[1]<<NUMBERS[0];
				rensinput();
			}
			if(Status_Humid == 2)
			{
				//Justerer størrelsen af variablen
				Size_Humid += 5;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Humid << " PROCENT FUGT";
				rensinput();
			}
			if(Status_Light == 2)
			{
				//Justerer variablen
				Size_Light += 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på skærmen
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Light<< " TIMER LYS OM DAGEN";
				rensinput();
			}
			Time = 0;
		}
		if(PINA == 0b01000000)
		{
			//Indstillinger
			//registrerer at man trykker
			Status_Setting = 1;
			Status_Status = 0;
			Status_Air = 0;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
			memset(SoonToBe,0x00,strlen(SoonToBe));
			SoonToBe = "HVAD VIL DU JUSTERE";
			rensinput();
		}
		if(PINA == 0b10000000)
		{
			//Ned
			if(Status_Air == 2)
			{
				//Justerer størrelsen af variablen
				Size_Air -= 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Air << " - LUFT UD AF "<<NUMBERS[1]<<NUMBERS[0];
				rensinput();
			}
			if(Status_Humid == 2)
			{
				//Justerer størrelsen af variablen
				Size_Humid -= 5;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Humid << " PROCENT FUGT";
				rensinput();
			}
			if(Status_Light == 2)
			{
				//Justerer variablen
				Size_Light -= 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på skærmen
				memset(SoonToBe,0x00,strlen(SoonToBe));
				SoonToBe = Size_Light<< " TIMER LYS OM DAGEN";
				rensinput();
			}
			Time = 0;
		}
		//Den virkende del af Status_Status:
		if(Status_Status)
		{
			//Display status
			memset(SoonToBe,0x00,strlen(SoonToBe));
			SoonToBe = "HOEST OM " << 8*7-Size_Status << " DAGE";
			rensinput();
			Time = 0;
		}
		//undersøger om man forsøger at ændre en værdi
		if(Status_Setting)
		{
			//finder hvilken værdi man vil ændre
			//Tillader herefter at man kan ændre dem med "op" og "ned"
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
			//Hvad displayet skal vise efter man har trykket på knapperne:
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
			Global_Time=0;
			Global_Time_Keeper = 0;
			t = 0;
			Status_Air = 0;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
		}

		//dage tæller op:
		if(Time_Days >= 1000*60*60*24)
		{
			Size_Status++;
			Time_Days = 0;
		}

		//counter der holder styr på hvor langt bogstaverne i den tekst man skal skrive er kommet
		//Alle rykker en plads hver 0.5 sek.
		if(Global_Time_Keeper > 0.5*1000)
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

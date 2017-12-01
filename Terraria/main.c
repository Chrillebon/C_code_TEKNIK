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
	DDRC = 0xFF; //alle C porte sat til 1, så at jeg kan styre 4*7 segment med lav vs høj
	DDRB = 0x01; //alle slukkede på nær mist maker
	//DDRA = 0x00; //alle knapperne
	//Setup til at måle fugtigheden:
	ADMUX = (1<<REFS0);
	ADCSRB = (1<<ADEN)|(1<<ADPS2)|(1<<ASPS1)|(1<<ASPS0);
}

/*
genstande der skal integreres:

Mist maker (B0)
Humidity sensor (B1)
Blæser (2x) (B2, B3)
2x knapper (B4,B5) lys, fugt
Krystal (B6, B7)
A0: humid sensor.
A1: Blæsere.
A2-A7: 6x Knapper (luft, status, reset, op, indst., ned)

*/

uint16_t adc_read(uint8_t INPUT)
{
	//dette skal være port A, da kun disse har analog til digital input...
	INPUT = 0b00000111;
	ADMUX = (ADMUX & 0xf8) | INPUT);

	ADCSRA |= (1<<ADSC);

	while(ADCSRA & (1<<ADSC));

	return ADC;
}

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
	for(int i = 0; i < globsize; i++)
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

//Bruges i næste funktion
//Oversætter char "værdi" til tal/bogstav vi kan bruge
//eller som "displays.h" forstår
//kig under ascii table, hvis det ikke ellers giver mening.

int givemechar(char tegn)
{
	if(tegn<65)
	{
		return NUMBERS[tegn-48];
	}
	return ALPHABET[tegn-65];
}

//Oversættelse fra hvad man skal læse til hvad der står på displayet

void display(int t)
{
	//løkker der går gennem de fire cifre på displayet
	for(int i = 0; i < 4; i++)
	{
		//sætter dette ciffer til at være hvad der står i variablen
		//Så at det ser ud somom at det bevæger sig.
		//her lokal variabel, fordi, den er åndsvag - vi brugte 2 timer på at finde det...
		char local = SoonToBe[t+i];
		//Når vi er løbet tør for hvad vi vil skrive, skal der være en overgang.
		if(t+i > globsize+3)
		{
			//Tager tegnet i starten, når den er færdig i slutningen:
			char local = SoonToBe[t-globsize+i-4];
			//værdi "local" bliver til index som i "displays.h" og givemechar() bliver til værdi vi kan bruge.
			choices[i] = givemechar(local);

		}
		//Her skal der ikke komme støj i overgangen:
		else if(local <= 47 || local >= 100)
		{
			choices[i] = 0b00000000;
		}
		//Dette er hvad der sker normalt:
		else
		{
			char local = SoonToBe[t+i];
			//værdi "local" bliver til index som i "displays.h" og givemechar() bliver til værdi vi kan bruge.
			choices[i] = givemechar(local);
		}
	}
}

//Finder størrelsen af vores array og gemmer det i globsize (længere nede)

int size(char Input[])
{
	int size=0;
	//Kør arrayet igennem
	for(int i=0;i<100;i++)
	{
		char tegn = Input[i];
		//Hvis tegnet ikke er NULL:
		if(tegn>0)
		{
			//læg en til størrelsen
			size++;
		}
		else
		{
			//Hvis den støder ind i NULL:
			return size;
		}
	}
}

int main(void)
{
	initialize();

	volatile int Global_Time = 0;
	volatile int Global_Time_Keeper = 0;
	volatile long Time_Days = 0;
	volatile int n = 1;
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
	volatile float Size_Humid_Sensor = 0;
	//Procent 0-100 - hvad vi måler at luftfugtigheden skal være
	volatile int Size_Air = 0;
	//Størrelse fra 0-10
	volatile int Fogger_On = 0;


	while(1)
	{
		//-------------------------------------------
		//displays der skifter og viser tal/bogstaver:
		//Hvis der ikke skal skrives noget kan foggeren virke...
		if(SoonToBe[0] == 0x00)
		{
			if(Fogger_On)
			{
				//Foggerport = 1
				PORTB = 0x01;
				//Den skal være tændt i 1/114000 del af et sekund
				//Virker på hastigheden af processoren
				//ca. 1000*1141/113000
				// et sekund * ca. hastighed 1 ms/ønskede interval
				for (int i = 0; i<10,i++){}
				//Slukker igen
				PORTB = 0x00;
				//behøver måske ikke sidste if statement når at der er så mange
				//if statements længere nede i koden...?
				//for (int i = 0; i<10,i++){}
			}
		}
		//Hvis der skal skrives noget:
		else
		{
			display(t);
			for(int i=0;i<4;i++)
			{
				//oplys de rigtige ting på de rigtige displays
				DDRC = DISPLAYS[i];
				PORTD = choices[i];
				//Timere der tæller opad
				Time++;
				Global_Time++;
				Global_Time_Keeper++;
				Time_Days++;
				_delay_ms(1);
			}
		}

		//-------------------------------------------
		//humidity sensor:

		//INDSÆT KODE DER MÅLER PROCENT LUFTFUGTIGHED HER:

		Size_Humid_Sensor = (adc_read(1)*85/1024)+10;

		//
		//Hvis den er mere end 5 %-point fra den ønskede værdi:
		if(Size_Humid_Sensor < Size_Humid - 5)
		{
			Fogger_On = 1;
		}
		else
		{
			Fogger_On = 0;
		}

		//-------------------------------------------
		//Mist maker:

		//-------------------------------------------
		//Blæsere:

		//Min / Max
		//hvis den ikke er sat til 0:
		if(Size_Air)
		{
			//tænd for blæserne:
			PORTB = 0b00001100;
		}

		//Tænd blæsere på Size_Air ud af 10

		//-------------------------------------------
		//Krystal:

		//--------------------------------------------

		//Knapper og interface:

		if(Status_Status or Status_Setting or Status_Air or Status_Humid or Status_Light or Status_Reset)
		{
			//Hvis der er blevet trykket på noget laver vi et lille delay, så at metal der hopper ikke er et problem:
			_delay_ms(50);
			//Så kan den ikke fange det to gange.
		}

		//Hvad alle knapper gør (0-7):

		if(PINB & (1<<PINB0))==0)//PIND == 0b00000001)
		{
			//Lys
			Status_Light = 1;
			Status_Humid = 0;
			Status_Air = 0;
			Time = 0;
		}
		if(PINB & (1<<PINB1))==0)//PIND == 0b00000010)
		{
			//Fugt
			Status_Humid = 1;
			Status_Air = 0;
			Status_Light = 0;
			Time = 0;
		}
		if(PINA & (1<<PINA2))==0)//PIND == 0b00000100)
		{
			//Luft
			Status_Air = 1;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
		}
		if(PINA & (1<<PINA3))==0)//PIND == 0b00001000)
		{
			//Status
			//Denne aktiverer kun, hvis det er tid til at høste
			if(Status_Status == 2)
			{
				Size_Status = 0;
				char SoonSoonToBe[] = "HOEST FAERDIG - HOEST IGEN OM 8 UGER";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
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
		if(PINA & (1<<PINA4))==0)//PIND == 0b00010000)
		{
			//Reset
			if(Status_Status == 1 && Status_Reset == 0)
			{
				Status_Reset = 1;
				char SoonSoonToBe[] = "TRYK IGEN FOR HARD RESET";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			//Nu har man trykket to gange på reset under indstillinger og den er blevet hard resat:
			if(Status_Status == 1 && Status_Reset == 1)
			{
				Size_Status = 0;
				char SoonSoonToBe[] = "HARD RESET FAERDIG";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			else
			{
				char SoonSoonToBe[] = "RESET FAERDIG";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			//Hvis det er tid til at høste
			if(Status_Status == 2)
			{
				Time_Days = 0;
				Size_Status = 0;
				//Sørger for, at status ikke aktiverer længere nede:
				Status_Status = 0;
				char SoonSoonToBe[] = "HOEST FAERDIG - HOEST IGEN OM 8 UGER";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
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
		if(PINA & (1<<PINA5))==0)//PIND == 0b00100000)
		{
			//Op
			if(Status_Air == 2)
			{
				//Justerer størrelsen af variablen
				Size_Air += 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				char SoonSoonToBe[] = Size_Air << " - LUFT UD AF 10";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Humid == 2)
			{
				//Justerer størrelsen af variablen
				Size_Humid += 5;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				char SoonSoonToBe[] = Size_Humid << " PROCENT FUGT";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Light == 2)
			{
				//Justerer variablen
				Size_Light += 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på skærmen
				char SoonSoonToBe[] = size_Light << " TIMER LYS OM DAGEN";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			Time = 0;
		}
		if(PINA & (1<<PINA6))==0)//PIND == 0b01000000)
		{
			//Indstillinger
			//registrerer at man trykker
			Status_Setting = 1;
			Status_Status = 0;
			Status_Air = 0;
			Status_Humid = 0;
			Status_Light = 0;
			Time = 0;
			char SoonSoonToBe[] = "HVAD VIL DU JUSTERE";
			SoonToBe = SoonSoonToBe;
			globsize = size(SoonToBe);
			rensinput();
		}
		if(PINA & (1<<PINA7))==0)//PIND == 0b10000000)
		{
			//Ned
			if(Status_Air == 2)
			{
				//Justerer størrelsen af variablen
				Size_Air -= 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				char SoonSoonToBe[] = Size_Air << " - Luft ud af 10";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Humid == 2)
			{
				//Justerer størrelsen af variablen
				Size_Humid -= 5;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på displayet
				char SoonSoonToBe[] = Size_Humid << " PROCENT FUGT";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Light == 2)
			{
				//Justerer variablen
				Size_Light -= 1;
				//Sørger for, at den ikke kan være uden for "range"
				range();
				//Skriver det på skærmen
				char SoonSoonToBe[] = Size_Light << " TIMER LYS OM DAGEN";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			Time = 0;
		}
		//Den virkende del af Status_Status:
		if(Status_Status)
		{
			//Display status
			char SoonSoonToBe[] = "HOEST OM " << 8*7-Size_Status << " DAGE";
			SoonToBe = SoonSoonToBe;
			globsize = size(SoonToBe);
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
				char SoonSoonToBe[] = "BRUG OP OG NED TIL AT JUSTERE LUFT";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Humid)
			{
				Status_Humid = 2;
				Status_Air = 0;
				Status_Light = 0;
				char SoonSoonToBe[] = "BRUG OP OG NED TIL AT JUSTERE FUGT";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			if(Status_Light)
			{
				Status_Light = 2;
				Status_Air = 0;
				Status_Humid = 0;
				char SoonSoonToBe[] = "BRUG OP OG NED TIL AT JUSTERE LYS";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			Time = 0;
		}
		else
		{
			//Hvad displayet skal vise efter man har trykket på knapperne:
			if(Status_Air)
			{
				char SoonSoonToBe[] = "LUFT - " << Size_Air << " UD AF 10";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
			}
			if(Status_Humid)
			{
				char SoonSoonToBe[] = "FUGT - " << Size_Humid << " PROCENT";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
			}
			if(Status_Light)
			{
				char SoonSoonToBe[] = "LYS - " << Size_Light << " TIMER OM DAGEN";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
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
				char SoonSoonToBe[] = "HOEST NU  -  TRYK PÅ STATUS ELLER RESET NAAR DETTE ER GJORT  -  HOEST NU";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
				rensinput();
			}
			else
			{
				Status_Status = 0;
				char SoonSoonToBe[] = "";
				SoonToBe = SoonSoonToBe;
				globsize = size(SoonToBe);
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
		if(t > globsize+3)
		{
			t = 0;
		}

		//Slut på interface og knapper
		//--------------------------------------------
	}
}

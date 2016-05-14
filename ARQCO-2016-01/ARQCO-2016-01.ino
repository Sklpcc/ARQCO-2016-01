#include "globarHeader.h"

/*
	TODO: (* means in progress)

		PROBLEMS:
		- Row at max brightness in 3 colors banishes blue

		MOST IMPORTANT:
		-* make some animations
		- increase the speed changing digitalWrite and similar functions within ISR

		LEAST IMPORTANT
		- enhance rgb
		- maybe extend BCM to 8bit resolution
		- add more comments
		- test PWM instead of BCM

	CHANGELOG:
		- matrix class created
		- rgb support complete
		- utf-8 strings fixed

*/

//14, 13, 12, 11 => 51 41 40 52
#define latch				40 //pin 12 storage ST-CP
#define enable				41 //pin 13 OE
#define spi_data				51 //pin 14 data DS
#define spi_clock			52 //pin 11 SH-CP
//colors[r,g,b][bcm][filas][columnas/8 = bytes]
//byte colors[3][_BCM_RESOLUTION][_ROWS][_MATRICES]; // bool colors[3][4][8][8*_MATRICES];
//I don't use bool or boolean because the implementation may take up more space than expected... bool colors[3][_ROWS][_COLUMNS][_BCM_RESOLUTION]

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte; //uint8_t¿?
Matrix M(_CLOCK_SPEED_DIV_2, MSBFIRST, SPI_MODE0);

byte pacman[2][5] = {{28,62,54,34,0},
					{ 28,62,62,62,28 } };

#define RIGHT 0
#define LEFT 1
#define DOWN 2

const byte pieces[7][4] = { {1,3,2,0},
						   {1,3,1,0},
						   {2,3,1,0},
						   {3,3,0,0},
						   {1,1,3,0},
						   {3,1,1,0},
						   {1,1,1,1} };

void setup()
{
	cli();
	_DEBUG_INIT(9600);
	row = 0;
	bitBCM = 0;
	counterBCM = 0;

	pinMode(latch, OUTPUT);
	pinMode(spi_data, OUTPUT);
	pinMode(spi_clock, OUTPUT);

	setupISR();
	M.fillMatrix(caster(RGB(0,0,0)));
	sei();
}

void loop()
{
	//animations
	//pacmanEfecto();
	M.setBuffer(caster(utf8ascii(String("String"))), 500, caster(RGB(15,0,0)));

	//delay(2000);
	
}

/*class TetrisPiece
{
	int top, left;
	int rows, columns; //max 4 rows, and 4 columns
	byte piece[4];
	TetrisPiece(int p)
	{
		rows = 2;
		columns = 3;
		if (p == 3)
			columns = 2;
		else if (p == 6)
			rows = 1, columns = 4;
		for (int i = 0; i < 4; ++i)
			piece[i] = pieces[p][i];
	}
	int move(int direction)
	{
		if (direction == RIGHT)
		{
			if (left + rows < _ROWS)
			{
				left += 1;
				return 1;
			}
			else
				return 0;
		}
		else if (direction == LEFT)
		{
			if (left - 1 >= 0)
			{
				left -= 1;
				return 1;
			}
			else
				return 0;
		}
		else if (direction == DOWN)
		{
			//colission dectection
			//corregir confusion de filas y columnas
			for (int j = 0; j < columns; ++j)
			{
				for (int i = 0; i < rows; ++i)
				{
					if (i + top > piece[j] & (1 << i))
					{
						if (LEDstate(i + top, j + left))
						{
							//col
							return 2;
						}
					}
				}
			}
			return 1;
		}
	}
	bool turn(int direction)
	{
		if (direction == LEFT)
		{

		}
		else if (direction == RIGHT)
		{

		}
	}
};*/

/*void pacmanEfecto()
{
	//129 for walls
	for (int i = 0; i < 64; ++i)
	{
		bool x = (random(0, 2) && (i > 8) && (buffer[i-1] == 65));
		buffer[i] = 65 + (x ? 8 : 0);
	}
	for (int p = 0; p < 64 - _COLUMNS; ++p)
	{
		fillMatrix(caster(RGB(0)));
		loadToRealBuffer(p);
		for (int k = 0; k < 2; ++k)
		{
			int columnas = (k ? 5 : 4);
			for (int i = 0; i < columnas; ++i)
			{
				byte value = pacman[k][i];
				for (int j = 1; j < _ROWS-2; ++j)
				{
					if (value & (1 << j))
					{
						setLED(j, i, caster(RGB(3, 15, 0)));
					}
					else
						setLED(j, i, caster(RGB(0)));
				}
			}
			delay(260);
		}
	}
}*/

ISR(TIMER1_COMPA_vect)
{
	digitalWrite(enable, HIGH);
	
	//BCM control
	if (counterBCM == _FIRST_CYCLE || counterBCM == _SECOND_CYCLE || counterBCM == _THIRD_CYCLE)
		bitBCM += 1;
	
	//Transfer
	M.transfer(bitBCM, row);
	
	//Control 595
	digitalWrite(latch, HIGH);
	digitalWrite(latch, LOW);
	digitalWrite(enable, LOW);
	pinMode(enable, OUTPUT);
	
	//Increment
	counterBCM += 1;
	row += 1;
	
	//Reset
	if (counterBCM == _FOURTH_CYCLE)
		counterBCM = bitBCM = 0;
	if (row == _ROWS)
		row = 0;
}

void noISR()
{
	digitalWrite(enable, HIGH);

	//BCM control
	if (counterBCM == _FIRST_CYCLE || counterBCM == _SECOND_CYCLE || counterBCM == _THIRD_CYCLE)
		bitBCM += 1;

	//Transfer
	M.transfer(bitBCM, row);

	//Control 595
	digitalWrite(latch, HIGH);
	digitalWrite(latch, LOW);
	digitalWrite(enable, LOW);
	pinMode(enable, OUTPUT);

	//Increment
	counterBCM += 1;
	row += 1;

	//Reset
	if (counterBCM == _FOURTH_CYCLE)
		counterBCM = bitBCM = 0;
	if (row == _ROWS)
		row = 0;
	delay(_DELAY);
}

void setupISR()
{
	TCCR1A = B00000000; //clear TCCR1A
	TCCR1B = B00000000; //clear TCCR1B
	TCCR1B |= ((1 << WGM12) | (1 << CS11) | (1 << CS10));
	OCR1A  = _SPEED; //set timer
	TIMSK1 |= (1 << OCIE1A); //enable timer
}

byte utf8ascii(byte ascii, byte &last)
{
	if (ascii<128)   // Standard ASCII-set 0..0x7F handling  
	{
		last = 0;
		return(ascii);
	}

	// get previous input
	byte c_last = last;   // get last char
	last = ascii;         // remember actual character

	switch (c_last)     // conversion depnding on first UTF8-character
	{
	case 0xC2: return  (ascii);  break;
	case 0xC3: return  (ascii | 0xC0);  break;
	case 0x82: if (ascii == 0xAC) return(0x80);       // special case Euro-symbol
	}
	return 0;                                     // otherwise: return zero, if character has to be ignored
}

String utf8ascii(const String &s)
{
	String r = "";
	char c;
	byte last = 0;
	for (int i = 0, len = s.length(); i < len; i += 1)
	{
		c = utf8ascii(s[i], last);
		if (c)
			r += c;
	}
	return r;
}
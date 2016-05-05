
#include <SPI.h>
#include "util.h"
#include "RGB.h"

/*
	TODO: (* means in progress)

		MOST IMPORTANT:
		- make some animations
		-* improve BCM with a struct or a class to make it more readable (RGB class)
		- increase the speed changing digitalWrite and similar functions within ISR
		-* add rgb support
		- make matrix class

		LEAST IMPORTANT
		- maybe extend BCM to 8bit resolution
		- search for inline keyword
		- add more comments
		- test PWM instead of BCM

	CHANGELOG:
		- start RGB support
		- created noISR function for debug purposes
		- created character handler to safely memory management (character_handler)
		- Fixed char_index values
*/
#define latch				40 //pin 12 storage ST-CP
#define enable				41 //pin 13 OE
#define spi_data			51 //pin 14 data DS
#define spi_clock			52 //pin 11 SH-CP

#define _RED				0
#define _GREEN				1
#define _BLUE				2

#define _FIRST_CYCLE		8
#define _SECOND_CYCLE		24
#define _THIRD_CYCLE		56
#define _FOURTH_CYCLE		120

#define _MATRICES			1
#define _ROWS				8
#define _COLUMNS			(_MATRICES << 3) //leds per row
#define _BCM_RESOLUTION		4
#define _SPEED				0x25

#define _DELAY				1000

#define _CLOCK_SPEED_DIV_2	8000000

volatile unsigned int maxValueForBCMResolution;

//colors[r,g,b][bcm][filas][columnas/8 = bytes]
byte colors[3][_BCM_RESOLUTION][_ROWS][_MATRICES]; // bool colors[3][4][8][8*_MATRICES];
//I don't use bool or boolean because the implementation may take up more space than expected... bool colors[3][_ROWS][_COLUMNS][_BCM_RESOLUTION]

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte; //uint8_t¿?
SPISettings spiCONF;
RGB colorTemp;

void setup()
{
	cli();
	colorTemp = RGB(6, 2, 12, true);

	row = 0;
	bitBCM = 0;
	counterBCM = 0;

	pinMode(latch, OUTPUT);
	//pinMode(enable, OUTPUT);
	pinMode(spi_data, OUTPUT);
	pinMode(spi_clock, OUTPUT);

	setupISR();
	setupSPI();
	setMaxValue();
	clearMatrix();
	unsigned char c = '~';

	set_Char(c, colorTemp);
	sei();
}

void loop()
{
	//animations
}

ISR(TIMER1_COMPA_vect)
{
	digitalWrite(enable, HIGH);
	
	//BCM control
	if (counterBCM == _FIRST_CYCLE || counterBCM == _SECOND_CYCLE || counterBCM == _THIRD_CYCLE)
		bitBCM += 1;
	
	//Transfer
	SPI.beginTransaction(spiCONF);
	for (int i = 0; i < 3; ++i) //color
		for (int j = 0; j < _MATRICES; ++j)//leds per row
			SPI.transfer(colors[i][bitBCM][row][j]);
	SPI.transfer(1 << row);
	SPI.endTransaction();
	
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
	//delay(_DELAY);
}

void noISR()
{
	digitalWrite(enable, HIGH);

	//BCM control
	if (counterBCM == _FIRST_CYCLE || counterBCM == _SECOND_CYCLE || counterBCM == _THIRD_CYCLE)
		bitBCM += 1;

	//Transfer
	SPI.beginTransaction(spiCONF);
	for (int i = 0; i < 3; ++i) //color
		for (int j = 0; j < _MATRICES; ++j)//leds per row
		{
			SPI.transfer(colors[i][bitBCM][row][j]);
		}
	SPI.transfer(1 << row);
	SPI.endTransaction();

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

void setupSPI()
{
	spiCONF = SPISettings(_CLOCK_SPEED_DIV_2, MSBFIRST, SPI_MODE0);
	SPI.begin();
}

void setupISR()
{
	TCCR1A = B00000000; //clear TCCR1A
	TCCR1B = B00000000; //clear TCCR1B
	TCCR1B |= ((1 << WGM12) | (1 << CS11) | (1 << CS10));
	OCR1A  = _SPEED; //set timer
	TIMSK1 |= (1 << OCIE1A); //enable timer
}

void setMaxValue()
{
	maxValueForBCMResolution = 0;//(1 << (_BCM_RESOLUTION + 1)) - 1;
	for(int i = 0; i < _BCM_RESOLUTION; ++i)
		maxValueForBCMResolution |= (1<<i);
}

void clearMatrix()
{
	for (int i = 0; i < 3; ++i) //color
		for(int j = 0; j < 4; ++j) //bits
			for(int k = 0; k < _ROWS; ++k) //rows
				for(int l = 0; l < _MATRICES; ++l) //columns
					colors[i][j][k][l] = 0xFF;
}


//FUNCTIONS

void setRow(byte row, byte red, byte green, byte blue)
{

}

void setColumn(byte column, byte red, byte green, byte blue)
{

}

void setLED(byte row, byte column,  RGB &color)
{
	row    = constrain(row,0, _ROWS - 1);
	column = constrain(column, 0, _COLUMNS - 1);

	int _byte = column >> 3;
	int _rbit = column - (_byte << 3);
	for (int i = 0; i < 3; ++i) //colors
	{
		byte actualColor = color.getComponent(i);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
		{
			if ((actualColor >> j) & 1)
				colors[i][j][row][_byte] |= (1 << _rbit);
			else
				colors[i][j][row][_byte] &= ~(1 << _rbit);
		}
	}
}

void set_Char(unsigned char character, RGB &color)
{
	character_handler ch(character);
	for (int i = 0; i < ch.size; ++i)
	{
		byte value = ch.getByte(i);
		for (int j = 0; j < _ROWS; ++j)
		{
			if (value & (1 << j))
			{
				setLED(j, i, color);
			}
		}
	}
}

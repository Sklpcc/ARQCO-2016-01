//#define _DEBUG
#include <SPI.h>
#include "util.h"
#include "RGB.h"
#include "macros.h"

/*
	TODO: (* means in progress)

		PROBLEMS:
		- Row at max brightness in 3 colors banishes blue
		- UTF-8 to extended ascii conversion, maybe a string class that wrap it

		MOST IMPORTANT:
		-* make some animations
		-* improve BCM with a struct or a class to make it more readable (RGB class)
		- increase the speed changing digitalWrite and similar functions within ISR
		-* add rgb support
		- make matrix class

		LEAST IMPORTANT
		- maybe extend BCM to 8bit resolution
		- add more comments
		- test PWM instead of BCM

	CHANGELOG:
		- Fixed character values
		- First animation, text right to left

*/
#define latch				40 //pin 12 storage ST-CP
#define enable				41 //pin 13 OE
#define spi_data				51 //pin 14 data DS
#define spi_clock			52 //pin 11 SH-CP
//colors[r,g,b][bcm][filas][columnas/8 = bytes]
byte colors[3][_BCM_RESOLUTION][_ROWS][_MATRICES]; // bool colors[3][4][8][8*_MATRICES];
//I don't use bool or boolean because the implementation may take up more space than expected... bool colors[3][_ROWS][_COLUMNS][_BCM_RESOLUTION]

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte; //uint8_t¿?
SPISettings spiCONF;
volatile byte buffer[_MAX_BUFFER_SIZE];
RGB colorBuffer[_MAX_COLOR_BUFFER_SIZE];
unsigned int lastRowUsed, lastIndexColor;

void setup()
{
	cli();
	_DEBUG_INIT(9600);
	lastRowUsed = 0;
	row = 0;
	bitBCM = 0;
	counterBCM = 0;

	pinMode(latch, OUTPUT);
	pinMode(spi_data, OUTPUT);
	pinMode(spi_clock, OUTPUT);

	setupISR();
	setupSPI();
	fillMatrix(caster(RGB(0,0,0)));
	String s = "000000000000000000000000000000000000000000"
	sei();
}

void loop()
{
	//animations
	//noISR();
	/*colorTemp = RGB(random(0,16), random(0,16), random(0,16));
	fillMatrix(colorOff);
	setRow(0, colorTemp);
	delay(100);
	fillMatrix(colorOff);
	setColumn(0, colorTemp);
	delay(100);
	fillMatrix(colorOff);
	setRow(7, colorTemp);
	delay(100);
	fillMatrix(colorOff);
	setColumn(7, colorTemp);
	delay(100);*/
	/*for (int i = _FIRST_CHAR; i <= _LAST_CHAR; ++i)
	{
		fillMatrix(caster(RGB(0, 0, 0)));
		set_Char(i, caster(RGB(12, 0, 0)));
		delay(300);
	}*/
	setBuffer(caster(String("A B C D E F G H I J K L M N O P Q R S T U V X Y Z")));
	//setLED(0, 0, caster(RGB(15, 0, 0)));

	delay(2000);
	
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
	delay(_DELAY);
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

void fillMatrix(RGB &color)
{
	for (int i = 0; i < 3; ++i)
	{
		byte actualColor = color.getComponent(i);
		//_DEBUG(actualColor);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			for (int k = 0; k < _ROWS; ++k)
				for (int l = 0; l < _MATRICES; ++l)
					if ((actualColor >> j) & 1)
						colors[i][j][k][l] |= B11111111;
					else
						colors[i][j][k][l] &= B00000000;
	}
}

//FUNCTIONS

void setRow(byte row, RGB &color)
{
	row = constrain(row, 0, _ROWS - 1);
	for (int i = 0; i < 3; ++i)
	{
		byte actualColor = color.getComponent(i);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			for(int l = 0; l < _MATRICES; ++l)
				if ((actualColor >> j) & 1)
					colors[i][j][row][l] |= B11111111;
				else
					colors[i][j][row][l] &= B00000000;
	}
}

void setColumn(byte column, RGB &color)
{
	column = constrain(column, 0, _COLUMNS - 1);

	int _byte = column >> 3;
	int _rbit = column - (_byte << 3);

	for (int i = 0; i < 3; ++i)
	{
		byte actualColor = color.getComponent(i);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			for (int k = 0; k < _ROWS; ++k)
				if ((actualColor >> j) & 1)
					colors[i][j][k][_byte] |= (1 << _rbit);
				else
					colors[i][j][k][_byte] &= ~(1 << _rbit);
	}
}

void setLED(byte row, byte column,  RGB &color)
{
	row = constrain(row, 0, _ROWS - 1);
	column = constrain(column, 0, _COLUMNS - 1);

	int _byte = column >> 3;
	int _rbit = column - (_byte << 3);

	for (int i = 0; i < 3; ++i)
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
	_DEBUG_PRINT(character);
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

bool setBuffer(String &s)
{
	int len = s.length();
	if (len == 0 || len > 100)
		return false;
	//load to buffer
	_DEBUG_PRINT(s);
	lastIndexColor = lastRowUsed = 0;
	for (int i = 0; i < len; ++i)
	{
		loadCharacter(s[i], caster(RGB(random(0,16), random(0,16), random(0, 16))), lastRowUsed, lastIndexColor);
	}
	for (int i = lastRowUsed; i <= lastRowUsed + _COLUMNS; ++i)
	{
		buffer[i] = 0;
	}
	for (int i = _COLUMNS; i < lastRowUsed + 1; ++i)
		_DEBUG_PRINT(buffer[i]);
	//lastRowUsed terminana apuntando al ultimo byte del ultimo caracter subido
	//now, let animation flow (print)
	show(0, lastRowUsed + _COLUMNS + 1);
	return true;
}


void show(int from, int to)
{
	int colorInd = 0;
	do
	{
		fillMatrix(caster(RGB(0, 0, 0)));
		for (int i = 0; i < _COLUMNS; ++i)
		{
			for (int j = 0; j < _ROWS; ++j)
			{
				if (buffer[i + from] & (1 << j))
					setLED(j, i, colorBuffer[colorInd]);
			}
		}
		from += 1;
		delay(100);
	} while (from + _COLUMNS < to);
}

void loadCharacter(byte character, RGB & color, unsigned int &start, unsigned int &colorStart)
{
	colorBuffer[colorStart++] = color;
	if (start == 0)
	{
		start = _COLUMNS;
	}

	character_handler ch(character);
	for (int i = 0; i < ch.size; ++i)
	{
		buffer[start++] = ch.getByte(i);
	}
}

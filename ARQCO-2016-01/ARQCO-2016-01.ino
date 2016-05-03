#include <SPI.h>
#include "util.h"

/*
	TODO:
	- make some animations
	- increase the speed changing digitalWrite and similar functions within ISR
	- add rgb support
	- make matrix class
	- maybe extend BCM to 8bit resolution
	- search for inline keyword
	- add more comments
	- test PWM instead of BCM
*/
<<<<<<< HEAD
#define latch				40 //pin 12 storage ST-CP
#define enable				41 //pin 13 OE
#define spi_data			51 //pin 14 data DS
#define spi_clock			52 //pin 11 SH-CP

#define _RED				0
#define _GREEN				1
#define _BLUE				2
=======
#define latch			40 //pin 12 storage ST-CP
#define enable			41 //pin 13 OE
#define spi_data		51 //pin 14 data DS
#define spi_clock		52 //pin 11 SH-CP

#define _RED			0
#define _GREEN			1
#define _BLUE			2
>>>>>>> 4aec26f7db5ff1df22219a1bbf8b02cc6f71b574

#define _FIRST_CYCLE		8
#define _SECOND_CYCLE		24
#define _THIRD_CYCLE		56
#define _FOURTH_CYCLE		120

<<<<<<< HEAD
#define _MATRICES			1
#define _ROWS				8
#define _COLUMNS			(_MATRICES << 3) //leds per row
#define _BCM_RESOLUTION		4
#define _SPEED				0x25
#define _USER_FRIENDLY
=======
#define _MATRICES		1
#define _ROWS			8
#define _COLUMNS		(_MATRICES << 3) //leds per row
#define _BCM_RESOLUTION		4
#define _SPEED			0x25
>>>>>>> 4aec26f7db5ff1df22219a1bbf8b02cc6f71b574

#define _CLOCK_SPEED_DIV_2	8000000

//colors[r,g,b][bcm][filas][columnas/8 = bytes]
byte colors[3][_BCM_RESOLUTION][_ROWS][_MATRICES]; // bool colors[3][4][8][8*_MATRICES];
//I don't use bool or boolean because the implementation may take up more space than expected... bool colors[3][_ROWS][_COLUMNS][_BCM_RESOLUTION]

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte, maxValueForBCMResolution; //uint8_t¿?
SPISettings spiCONF;
volatile byte buffer[21] = { 0 };


void setup()
{
	//Serial.begin(9600);

	cli(); //nointerrupts
	
	rowByte = 0;
	row = 1;
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
	loadCharacter('0');
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
			SPI.transfer(colors[i][bitBCM][rowByte][j]);
	SPI.transfer(row);
	SPI.endTransaction();
	
	//Control 595
	digitalWrite(latch, HIGH);
	digitalWrite(latch, LOW);
	digitalWrite(enable, LOW);
	pinMode(enable, OUTPUT);
	
	//Increment
	++counterBCM;
	row <<= 1;
	rowByte += 1;
	
	//Reset
	if (counterBCM == _FOURTH_CYCLE)
		counterBCM = bitBCM = 0;
	if (row == 0)
		row = 1;
	if (rowByte == _ROWS)
		rowByte = 0;
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
	maxValueForBCMResolution = 0;
	for(int i = 0; i < _BCM_RESOLUTION; ++i)
		maxValueForBCMResolution |= (1<<i);
}

void clearMatrix()
{
	for (int i = 0; i < 3; ++i) //color
		for(int j = 0; j < 4; ++j) //bits
			for(int k = 0; k < _COLUMNS; ++k) //bytes
				colors[i][j][k] = 0xFF;
}
<<<<<<< HEAD


//FUNCTIONS

void setRow(byte row, byte red, byte green, byte blue)
{

}

void setColumn(byte column, byte red, byte green, byte blue)
{

}

void setLED(byte row, byte column, byte red, byte green, byte blue)
{
	row    = constrain(row,0, 7);
	column = constrain(column, 0, _COLUMNS - 1);
	red    = constrain(red, 0, maxValueForBCMResolution);
	green  = constrain(green, 0, maxValueForBCMResolution);
	blue   = constrain(blue, 0, maxValueForBCMResolution);

	int _byte = column >> 3;
	int _rbit = column - (_byte << 3);

#ifdef _USER_FRIENDLY
	red   = maxValueForBCMResolution - red;
	green = maxValueForBCMResolution - green;
	blue  = maxValueForBCMResolution - blue;
#endif //_USER_FRIENDLY

	for (int i = 0; i < _BCM_RESOLUTION; ++i)
	{
		//byte = value << bit
		if ((red >> i) & 1)
			colors[_RED][i][row][_byte] |= (1 << _rbit);
		else
			colors[_RED][i][row][_byte] &= ~(1 << _rbit);
	}
	for (int i = 0; i < _BCM_RESOLUTION; ++i)
	{
		if ((green >> i) & 1)
			colors[_GREEN][i][row][_byte] |= (1 << _rbit);
		else
			colors[_GREEN][i][row][_byte] &= ~(1 << _rbit);
	}
	for (int i = 0; i < _BCM_RESOLUTION; ++i)
	{
		if ((blue >> i) & 1)
			colors[_BLUE][i][row][_byte] |= (1 << _rbit);
		else
			colors[_BLUE][i][row][_byte] &= ~(1 << _rbit);
	}
}

void SerialPrintCharacter(byte character)
{
	//Serial.print("Caracter: "); Serial.println((unsigned char)(character));
	character -= 32;
	int pos = pgm_read_word_near(char_index + character);
	int size = pgm_read_byte_near(charmap + pos);
	//Serial.print("pos: "); Serial.println(pos);
	//Serial.print("size: "); Serial.println(size);
	pos = pos + 1;
	for (int i = 0; i < size; ++i)
	{
		//Serial.print(pgm_read_byte_near(charmap+pos+i));
		//Serial.print(" ");
	}
	//Serial.println();
}

void loadCharacter(byte character)
{
	character -= 32;
	int pos = pgm_read_word_near(char_index + character);
	int size = pgm_read_byte_near(charmap + pos);
	pos = pos + 1;
	for (int i = 0; i < size; ++i)
	{
		buffer[8 + i] = pgm_read_byte_near(charmap + pos + i);
	}
	for (int i = 0; i < 8; ++i)
	{

	}
}
=======
>>>>>>> 4aec26f7db5ff1df22219a1bbf8b02cc6f71b574

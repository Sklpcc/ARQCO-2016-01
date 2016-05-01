#include <SPI.h>

/*
	TODO:
	- make some animations
	- increase the speed changing digitalWrite and similar functions within ISR
	- add rgb support
	- make matrix class
	- maybe extend BCM to 8bit resolution
	- search for inline keyword
	- add comments
	- test PWM instead of BCM
*/
#define latch	40 //pin 12 storage ST-CP
#define enable	41 //pin 13 OE
#define spi_ data	51 //pin 14 data DS
#define spi_clock	52 //pin 11 SH-CP

#define _RED	0
#define _GREEN	1
#define _BLUE	2

#define _FIRST_CYCLE	8
#define _SECOND_CYCLE	24
#define _THIRD_CYCLE	56
#define _FOURTH_CYCLE	120

#define _MATRICES	1
#define _ROWS		8
#define _COLUMNS	(_MATRICES << 3) //leds per row
#define _BCM_RESOLUTION 4
#define _SPEED		0x25

#define _CLOCK_SPEED_DIV_2 8000000

//colors[color][bit][byte]
//colors[r,g,b][bits - binary code modulation][8 * bytes = bits]
byte colors[3][_BCM_RESOLUTION][_COLUMNS]; // bool colors[3][4][512];
//I don't use bool or boolean beacuse the implementation may take up more space than expected... bool colors[3][_COLUMNS][_BCM_RESOLUTION]

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte, maxValueForBCMResolution; //uint8_t¿?
SPISettings spiCONF;

void setup()
{
	cli(); //nointerrupts
	
	rowByte = 0;
	row = 1;
	bitBCM = 0;
	counterBCM = 0;
	
	pinMode(latch, OUTPUT);
	pinMode(enable, OUTPUT);
	pinMode(spi_data, OUTPUT);
	pinMode(spi_clock, OUTPUT);

	setupISR();
	setupSPI();
	setMaxValue();
	
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
		for (int j = rowByte; j < rowByte + _MATRICES; ++j)//leds per row
			SPI.transfer(colors[i][bitBCM][j]);
	SPI.transfer(row);
	SPI.endTransaction();
	
	//Control 595
	digitalWrite(latch, HIGH);
	digitalWrite(latch, LOW);
	digitalWrite(enable, LOW);
	
	//Increment
	++counterBCM;
	row <<= 1;
	rowByte += _MATRICES;
	
	//Reset
	if (counterBCM == _FOURTH_CYCLE)
		counterBCM = bitBCM = 0;
	if (row == 0)
		row = 1;
	if (rowByte == _COLUMNS)
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
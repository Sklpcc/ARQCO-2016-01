#include "globalHeader.h"

/*
	TODO: (* means in progress)

		PROBLEMS:
		- Row at max brightness in 3 colors banishes blue

		MOST IMPORTANT:
		- rework Matrix.show method to make it iterable
		-* finish all animations
		-* add asynchronous effects button
		- increase the speed changing digitalWrite and similar functions within ISR

		LEAST IMPORTANT
		-* add games support (tetris, space invaders, pong)
		- enhance rgb (hsv support)
		- extend BCM to 8 bit resolution
		- test PWM instead of BCM
		- add more comments

	CHANGELOG:
		- reset button is just hardware
		- start support for effects button
		- start games support (tetris)
		- Printable2 class created for draw objects in Matrix class
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
const byte bEffect = 21;
byte actEffect;
long debouncing_time = 250;
volatile unsigned long last_micros;

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
	pinMode(bEffect, INPUT);
	attachInterrupt(digitalPinToInterrupt(bEffect), effect, RISING);

	actEffect = 0;
	last_micros = 0;

	setupISR();
	M.fillMatrix(caster(RGB(0,0,0)));
	sei();
}

void loop()
{
	//animations
	M.setBuffer("a b");
	//M.show(0, M.getLastRowUsed(), caster(RGB(6, 0, 15)), 3000, _STATIC);
	//Serial.println(actEffect);
	while (true)
	{
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 6)), 3000, _STATIC);
		delay(10);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _RIGHT_TO_LEFT);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _LEFT_TO_RIGHT);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _TOP_TO_BOTTOM);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _BOTTOM_TO_TOP);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _TOP_TO_BOTTOM | _FADE_IN);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 200, _BOTTOM_TO_TOP | _FADE_IN);
		delay(60);
		M.fillMatrix(caster(RGB(0)));
		delay(120);
	}
}

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

void effect()
{
	if ((long)(micros() - last_micros) >= debouncing_time * 1000)
	{
		if (micros() - last_micros <= 1000000)
			actEffect = (++actEffect) % _MAX_EFFECT;
		else
			actEffect = 0;
		last_micros = micros();
	}
}
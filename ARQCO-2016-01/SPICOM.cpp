/*#include "SPICOM.h"

#ifndef cbi //clear bit
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~(_BV(bit)))
#endif
#ifndef sbi //set bit
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define ENABLE_SPI_INTERRUPT		0x40
#define ENABLE_SPI					0x20
#define LEAST_FIRST					0x10
#define MASTER_MODE					0x08
#define HIGH_IDLE_CLOCK				0x04
#define FALLING_CLOCK				0x02

SPICOM::SPICOM(byte control, bool fastADC = true)
{
	SPCR = control;
	SPSR = B00000000;
	if(fastADC)
	{
		sbi(ADCSRA, ADPS2); //? cbi or sbi
		cbi(ADCSRA, ADPS1);
		cbi(ADCSRA, ADPS0);
	}
}

SPICOM::setPorts(int latch, int sck, int mosi, int miso)
{
	pinMode(latch, OUTPUT);
	pinMode(sck,   OUTPUT);
	pinMode(mosi,  OUTPUT);
	pinMode(miso,  INPUT_PULLUP);

	digitalWrite(latch, LOW);
	digitalWrite(sck,   LOW);
	digitalWrite(mosi,  LOW);
	//digitalWrite(miso,  HIGH);//no necesario por INPUT_PULLUP
}
*/
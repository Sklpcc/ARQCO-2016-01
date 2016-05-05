// SPICOM.h

/*#ifndef _SPICOM_h
#define _SPICOM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SPICOM
{

	public:
		SPICOM(byte control, bool fastADC);
		setPorts(int latch, int sck, int mosi, int miso);
};

#endif

*/
#ifndef _MATRIX_C_
#define _MATRIX_C_

#include "macros.h"
#include "RGB.h"
#include "util.h"
#include <SPI.h>

class Matrix
{
public:
	Matrix(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);
	void setLED(byte row, byte column, RGB &color);
	void setRow(byte row, RGB &color);
	void setColumn(byte column, RGB &color);
	void fillMatrix(RGB &color);
	bool LEDState(byte row, byte column);
	bool setBuffer(String & s);
	bool setBuffer(char *s);
	bool setBuffer(byte s[], int len);
	bool loadCharacter(byte character, unsigned int &start);
	bool appendCharacter(byte character);
	void transfer(int bitBCM, int row);
	void show(int from, int to, RGB &color, unsigned long speed, byte mode);
	void loadToRealBufferHorizontally(int from, RGB color);
	void loadToRealBufferVertically(int from, int k, RGB color, byte mode);

	unsigned int getLastRowUsed();
	unsigned int getlastIndexColor();

	
	byte utf8ascii(byte ascii, byte &last);
	String utf8ascii(const String &s);
	void utf8ascii(char* s);

	void clearBuffer();
	
	

private:
	byte buffer[3][_BCM_RESOLUTION][_ROWS][_MATRICES];
	byte fakeBuffer[_MAX_BUFFER_SIZE];
	unsigned int rows, columns, matrices;
	SPISettings spiCONF;
	unsigned int lastRowUsed, lastIndexColor;
};

#endif
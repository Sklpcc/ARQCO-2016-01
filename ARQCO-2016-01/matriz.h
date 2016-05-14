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
	bool setBuffer(String & s, uint16_t speed, RGB temp = RGB(0));
	bool loadCharacter(byte character, unsigned int &start, unsigned int &colorStart);
	void transfer(int bitBCM, int row);
	void show(int from, int to, RGB color,uint16_t speed);
	void loadToRealBuffer(int from, RGB color);
private:
	byte buffer[3][_BCM_RESOLUTION][_ROWS][_MATRICES];
	byte fakeBuffer[_MAX_BUFFER_SIZE];
	unsigned int rows, columns, matrices;
	SPISettings spiCONF;
};

#endif
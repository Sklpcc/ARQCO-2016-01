#include "matriz.h"

Matrix::Matrix(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
{
#if defined(_ROWS) && defined(_COLUMNS) && defined (_MATRICES)
	rows = _ROWS;
	columns = _COLUMNS;
	matrices = _MATRICES;
#else
	rows = 8;
	columns = 8;
	matrices = 1;
#endif
	spiCONF = SPISettings(clock, bitOrder, dataMode);
	SPI.begin();
}

void Matrix::setLED(byte row, byte column, RGB & color)
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
				buffer[i][j][row][_byte] |= (1 << _rbit);
			else
				buffer[i][j][row][_byte] &= ~(1 << _rbit);
		}
	}
}

void Matrix::setRow(byte row, RGB & color)
{
	row = constrain(row, 0, _ROWS - 1);
	for (int i = 0; i < 3; ++i)
	{
		byte actualColor = color.getComponent(i);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			for (int l = 0; l < _MATRICES; ++l)
				if ((actualColor >> j) & 1)
					buffer[i][j][row][l] |= B11111111;
				else
					buffer[i][j][row][l] &= B00000000;
	}
}

void Matrix::setColumn(byte column, RGB & color)
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
					buffer[i][j][k][_byte] |= (1 << _rbit);
				else
					buffer[i][j][k][_byte] &= ~(1 << _rbit);
	}
}

void Matrix::fillMatrix(RGB & color)
{
	for (int i = 0; i < 3; ++i)
	{
		byte actualColor = color.getComponent(i);
		//_DEBUG(actualColor);
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			for (int k = 0; k < _ROWS; ++k)
				for (int l = 0; l < _MATRICES; ++l)
					if ((actualColor >> j) & 1)
						buffer[i][j][k][l] |= B11111111;
					else
						buffer[i][j][k][l] &= B00000000;
	}
}

bool Matrix::LEDState(byte row, byte column)
{
	row = constrain(row, 0, _ROWS - 1);
	column = constrain(column, 0, _COLUMNS - 1);

	int _byte = column >> 3;
	int _rbit = column - (_byte << 3);
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
			if (buffer[i][j][row][_byte] & (1 << _rbit))
				return true;
	return false;
}

bool Matrix::setBuffer(String & s, uint16_t speed, RGB temp)
{
	int len = s.length();
	//load to buffer
	unsigned int lastIndexColor = 0, lastRowUsed = 0;
	for (int i = 0; i < len; ++i)
	{
		if (!loadCharacter(s[i], lastRowUsed, lastIndexColor))
			return false;
	}
	for (int i = lastRowUsed; i < lastRowUsed + _COLUMNS; ++i)
	{
		fakeBuffer[i] = 0;
	}
	//lastRowUsed terminara apuntando al ultimo byte del ultimo caracter subido
	//now, let animation flow (print)
	show(0, lastRowUsed + _COLUMNS + 1, temp, speed);
	return true;
}

bool Matrix::loadCharacter(byte character, unsigned int & start, unsigned int & colorStart)
{
	character_handler ch(character);
	if (start == 0)
		start = _COLUMNS;
	if (ch.size + start > _MAX_BUFFER_SIZE)
		return false;
	for (int i = 0; i < ch.size; ++i)
	{
		fakeBuffer[start++] = ch.getByte(i);
	}
	return true;
}

void Matrix::show(int from, int to, RGB color, uint16_t speed)
{
	unsigned long mil = millis();

	fillMatrix(caster(RGB(0)));
	loadToRealBuffer(from, color);
	++from;

	for(;from + _COLUMNS < to;)
	{
		if (millis() - mil >= speed)
		{
			fillMatrix(caster(RGB(0)));
			loadToRealBuffer(from, color);
			++from;
			mil = millis();
		}
		//delay(300);
	}
}

void Matrix::loadToRealBuffer(int from, RGB color)
{
	for (int i = 0; i < _COLUMNS; ++i)
	{
		for (int j = 0; j < _ROWS; ++j)
		{
			if (fakeBuffer[i + from] & (1 << j))
				setLED(j, i, caster(color));
		}
	}
}



void Matrix::transfer(int bitBCM, int row)
{
	SPI.beginTransaction(spiCONF);
	for (int i = 0; i < 3; ++i) //color
		for (int j = 0; j < _MATRICES; ++j)//leds per row
			SPI.transfer(buffer[i][bitBCM][row][j]);
	SPI.transfer(1 << row);
	SPI.endTransaction();
}

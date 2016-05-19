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
	lastRowUsed = lastIndexColor = 0;
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

bool Matrix::setBuffer(String & s)
{
	s = utf8ascii(s);
	int len = s.length();
	//load to buffer
	lastIndexColor = lastRowUsed = 0;

	for (int i = 0; i < len; ++i)
	{
		if (!appendCharacter(s[i]))
			return false;
	}
	//lastRowUsed terminara apuntando al ultimo byte del ultimo caracter subido
	//now, let animation flow (print)
	return true;
}

bool Matrix::setBuffer(char *s)
{
	utf8ascii(s);
	int len = strlen(s);
	//load to buffer
	lastIndexColor = lastRowUsed = 0;

	for (int i = 0; i < len; ++i)
	{
		if (!appendCharacter(s[i]))
			return false;
	}
	//lastRowUsed terminara apuntando al ultimo byte del ultimo caracter subido
	//now, let animation flow (print)
	return true;
}

bool Matrix::setBuffer(byte s[], int len)
{
	//load to buffer
	lastIndexColor = lastRowUsed = 0;

	for (int i = 0; i < len; ++i)
	{
		fakeBuffer[lastRowUsed++] = s[i];
	}
	//lastRowUsed terminara apuntando al ultimo byte del ultimo caracter subido
	//now, let animation flow (print)
	return true;
}

bool Matrix::loadCharacter(byte character, unsigned int & start)
{
	character_handler ch(character);
	if (ch.size + start > _MAX_BUFFER_SIZE)
		return false;
	for (int i = 0; i < ch.size; ++i)
	{
		fakeBuffer[start++] = ch.getByte(i);
	}
	return true;
}

bool Matrix::appendCharacter(byte character)
{
	if (lastRowUsed < 0)
		return false;
	character_handler ch(character);
	if (ch.size + lastRowUsed > _MAX_BUFFER_SIZE)
		return false;
	for (int i = 0; i < ch.size; ++i)
	{
		fakeBuffer[lastRowUsed++] = ch.getByte(i);
	}
	return true;
}

void Matrix::show(int from, int to, RGB &color, unsigned long speed, byte mode)
{
	unsigned long mil = millis();
	int step = 1;

	if (mode & _RIGHT_TO_LEFT || mode & _LEFT_TO_RIGHT)
	{
		from -= _COLUMNS;
		to += _COLUMNS;
	}
	else if (lastRowUsed < _COLUMNS)
	{
		from = from - ((_COLUMNS - lastRowUsed) >> 1); //left pad will always be less than right pad
		to = to + ((_COLUMNS - lastRowUsed) >> 1) + 1;//not necesary
	}
	if (mode & _USE_LAST)
		step = 0;
	if (mode & _RIGHT_TO_LEFT)
	{
		fillMatrix(caster(RGB(0)));
		loadToRealBufferHorizontally(from, color);
		++from;

		for (; from + _COLUMNS <= to;)
		{
			if (millis() - mil >= speed)
			{
				fillMatrix(caster(RGB(0)));
				loadToRealBufferHorizontally(from, color);
				++from;
				mil = millis();
			}
			//delay(300);
		}
	}
	else if (mode & _LEFT_TO_RIGHT)
	{
		/*
			t                                 f
			00000000--------------------00000000
		*/
		int tmp = from;
		from = to;
		to = tmp;

		from -= _COLUMNS; //data is uploaded from right to left always
		fillMatrix(caster(RGB(0)));
		loadToRealBufferHorizontally(from, color);
		--from;

		for (; from >= to;)
		{
			if (millis() - mil >= speed)
			{
				fillMatrix(caster(RGB(0)));
				loadToRealBufferHorizontally(from, color);
				--from;
				mil = millis();
			}
			//delay(300);
		}
	}
	else if (mode & _TOP_TO_BOTTOM)
	{
		//verify size...
		if (mode & _FADE_IN)
		{
			for (int k = _ROWS; k >= 0; --k) //etapa
			{
				if (step)
				{
					fillMatrix(caster(RGB(0)));
					loadToRealBufferVertically(from, k, color, mode);
					delay(speed);
				}
				++step;
			}
		}
		else
		{
			for (int k = 0; k <= _ROWS; ++k) //etapa
			{
				if (step)
				{
					fillMatrix(caster(RGB(0)));
					loadToRealBufferVertically(from, k, color, mode);
					delay(speed);
				}
				++step;
			}
		}
	}
	else if (mode & _BOTTOM_TO_TOP)
	{
		//verify size...
		if (mode & _FADE_IN)
		{
			for (int k = _ROWS; k >= 0; --k) //etapa
			{
				if (step)
				{
					fillMatrix(caster(RGB(0)));
					loadToRealBufferVertically(from, k, color, mode);
					delay(speed);
				}
				++step;
			}
		}
		else
		{
			for (int k = 0; k <= _ROWS; ++k) //etapa
			{
				if (step)
				{
					fillMatrix(caster(RGB(0)));
					loadToRealBufferVertically(from, k, color, mode);
					delay(speed);
				}
				++step;
			}
		}
	}
	else if (mode & _STATIC)
	{
		if (step)
		{
			fillMatrix(caster(RGB(0)));
			loadToRealBufferHorizontally(from, color);
			delay(speed);
		}
	}
}

void Matrix::loadToRealBufferHorizontally(int from, RGB color)
{
	byte value;
	for (int i = 0; i < _COLUMNS; ++i)
	{
		if (i + from < 0 || i + from > lastRowUsed)
			value = 0;
		else
			value = fakeBuffer[i + from];

		for (int j = 0; j < _ROWS; ++j)
		{
			if (value & (1 << j))
				setLED(j, i, caster(color));
		}
	}
}

void Matrix::loadToRealBufferVertically(int from, int k, RGB color, byte mode)
{
	byte value;
	for (int i = 0; i < _COLUMNS; ++i)
	{
		if (i + from < 0 || i + from > lastRowUsed)
			value = 0;
		else
			value = fakeBuffer[i + from];

		if (mode & _TOP_TO_BOTTOM)
			value <<= k;
		else
			value >>= k;

		for (int j = 0; j < _ROWS; ++j) //change this to a function!!!!!!
		{
			if (value & (1 << j))
				setLED(j, i, caster(color));
		}
	}
}

unsigned int Matrix::getLastRowUsed()
{
	return lastRowUsed;
}

unsigned int Matrix::getlastIndexColor()
{
	return lastIndexColor;
}

void Matrix::transfer(int bitBCM, int row)
{
	SPI.beginTransaction(spiCONF);
	for (int j = _MATRICES - 1; j >= 0; --j)//color
		for (int i = 0; i < 3; ++i) //leds per row
			SPI.transfer(buffer[i][bitBCM][row][j]);
	SPI.transfer(1 << row);
	SPI.endTransaction();
}

byte Matrix::utf8ascii(byte ascii, byte &last)
{
	if (ascii<128)   // Standard ASCII-set 0..0x7F handling  
	{
		last = 0;
		return(ascii);
	}

	// get previous input
	byte c_last = last;   // get last char
	last = ascii;         // remember actual character

	switch (c_last)     // conversion depnding on first UTF8-character
	{
	case 0xC2: return  (ascii);  break;
	case 0xC3: return  (ascii | 0xC0);  break;
	case 0x82: if (ascii == 0xAC) return(0x80);       // special case Euro-symbol
	}
	return 0;                                     // otherwise: return zero, if character has to be ignored
}

String Matrix::utf8ascii(const String &s)
{
	String r = "";
	char c;
	byte last = 0;
	for (int i = 0, len = s.length(); i < len; i += 1)
	{
		c = utf8ascii(s[i], last);
		if (c)
			r += c;
	}
	return r;
}

void Matrix::utf8ascii(char* s)
{
	int k = 0;
	char c;
	byte l = 0;
	for (int i = 0; i<strlen(s); i++)
	{
		c = utf8ascii(s[i], l);
		if (c != 0)
			s[k++] = c;
	}
	s[k] = 0;
}

void Matrix::clearBuffer()
{
	for (int i = 0; i < _MAX_BUFFER_SIZE; ++i)
		fakeBuffer[i] = 0;
}
//#define _DEBUG_USER
//#define _USE_STOP

#include "util.h"
#include "RGB.h"
#include "macros.h"
#ifdef _VSARDUINO_H_
#include <SPI/src/SPI.h>
#else
#include<SPI.h>
#endif

volatile byte bitBCM, row;
volatile unsigned int counterBCM, rowByte; //uint8_t¿?
byte actEffect, inter;

//Libraries:

#define  NOTE_C     3830    // 261 Hz 
#define  NOTE_D     3400    // 294 Hz 
#define  NOTE_E     3038    // 329 Hz 
#define  NOTE_F     2864    // 349 Hz 
#define  NOTE_G     2550    // 392 Hz 
#define  NOTE_A     2272    // 440 Hz 
#define  NOTE_B     2028    // 493 Hz 
#define  NOTE_CC     1912    // 523 Hz 

//List
template<typename T>
class List
{
public:
	List()
	{
		reset(0);
	}
	List(T value, int size = 0)
	{
		reset(size);
		for (int i = 0; i < size; ++i)
		{
			content[i] = value;
		}
	}

	bool popFront()
	{
		if (_size == 0)
			return false;
		erase(0);
		return true;
	}
	bool popBack()
	{
		if (_size == 0)
			return false;
		_size -= 1;
		return true;
	}

	T getFront()
	{
		if (_size == 0)
			return T();
		return content[0];
	}
	T getBack()
	{
		if (_size == 0)
			return T();
		return content[_size - 1];
	}

	T getPopFront()
	{
		if (_size == 0)
			return T();
		T r = content[0];
		erase(0);
		return r;
	}
	T getPopBack()
	{
		if (_size == 0)
			return T();
		T r = content[_size - 1];
		erase(size - 1);
		return r;
	}

	bool pushFront(T d)
	{
		if (_size == _capacity)
			return false;

		for (int i = _size; i > 0; --i)
			content[i] = content[i - 1];
		_size += 1;
		content[0] = d;
		return true;
	}
	bool pushBack(T d)
	{
		if (_size == _capacity)
			return false;
		content[_size++] = d;
		return true;
	}

	bool erase(const int pos)
	{
		if (pos >= _size || pos < 0 || _size == 0)
			return false;
		_size -= 1;
		for (int i = pos; i < _size; ++i) //test
			content[i] = content[i + 1];
		return true;
	}

	T getElement(const int i)
	{
		if (i >= 0 && i < _size)
			return content[i];
		return T();
	}
	T& operator[](const int index)
	{
		return content[index];
	}

	bool resize(int size)
	{
		if (size >= maxSize)
			return false;
		_capacity = size;
		return true;
	}
	bool reset(int size)
	{
		if (!resize(size))
			return false;
		this->_size = 0;
		return true;
	}
	int size()
	{
		return _size;
	}
	int capacity()
	{
		return _capacity;
	}
	int getMaxSize()
	{
		return maxSize;
	}

private:
	const static int maxSize = 70;
	T content[maxSize];
	int _capacity;
	int _size;
};

//Pair
template<typename U, typename V>
class pair
{
public:
	U first;
	V second;
	pair()
	{
		first = U();
		second = V();
	}
	pair(U first, V second)
	{
		this->first = first;
		this->second = second;
	}
	bool operator == (const pair<U, V> &a)
	{
		return (this->first == a.first) && (this->second == a.second);
	}
};
typedef pair<int, int> pii;
typedef pair<float, float> pff;
typedef pair<byte, byte> pbb;
typedef pii coord;
typedef pff coord2;
//Matrix
class Matrix
{
public:
	Matrix()
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
		MatrixToDraw = lastRowUsed = lastIndexColor = 0;
		hidden = false;
	}
	Matrix(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
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
		MatrixToDraw = lastRowUsed = lastIndexColor = 0;
		hidden = false;
	}

	void init(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
	{
		spiCONF = SPISettings(clock, bitOrder, dataMode);
		SPI.begin();
	}

	void setLED(byte row, byte column, RGB &color, byte buf = 0)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);
		buf = constrain(buf, 0, _BUFFERS - 1);

		int _byte = column >> 3; //matrix (0 - (MATRICES - 1))
		int _rbit = column - (_byte << 3); //column (0 - 7)

		for (int i = 0; i < _COLORS; ++i)
		{
			byte actualColor = color.getComponent(i);
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
			{
				if ((actualColor >> j) & 1)
					buffer[buf][i][j][row][_byte] |= (1 << _rbit);
				else
					buffer[buf][i][j][row][_byte] &= ~(1 << _rbit);
			}
		}
	}
	void setFakeLED(byte row, byte column);

	void clearLED(byte row, byte column, byte buf = 0)
	{
		setLED(row, column, caster(RGB(0)), buf);
	}
	void clearFakeLED(byte row, byte column);

	void setColumnByte(byte column, byte data, RGB &color, byte buf = 0)
	{
		for (int i = 0; i < _ROWS; ++i)
			if (data & (1 << i))
				setLED(i, column, color, buf);
	}
	void setFakeColumnByte(byte column, byte data);

	void setEntireRow(byte row, RGB &color, byte buf = 0)
	{
		row = constrain(row, 0, _ROWS - 1);
		for (int i = 0; i < 3; ++i)
		{
			byte actualColor = color.getComponent(i);
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
				for (int l = 0; l < _MATRICES; ++l)
					if ((actualColor >> j) & 1)
						buffer[buf][i][j][row][l] |= B11111111;
					else
						buffer[buf][i][j][row][l] &= B00000000;
		}
	}
	void setFakeEntireRow(byte row);

	void setEntireColumn(byte column, RGB &color, byte buf = 0)
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
						buffer[buf][i][j][k][_byte] |= (1 << _rbit);
					else
						buffer[buf][i][j][k][_byte] &= ~(1 << _rbit);
		}
	}
	void setFakeEntireColumn(byte column);

	void fillArea(byte top, byte left, byte bottom, byte right, RGB &color, byte buf = 0)
	{
		for (int i = bottom; i <= top; ++i)
			for (int j = left; j <= right; ++j)
				setLED(i, j, color, buf);
	}

	void fillFakeArea(int top, int left, int bottom, int right); //change these to unsigned

	void fillMatrix(RGB &color, byte buf = 0)
	{
		for (int i = 0; i < 3; ++i)
		{
			byte actualColor = color.getComponent(i);
			//_DEBUG(actualColor);
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
				for (int k = 0; k < _ROWS; ++k)
					for (int l = 0; l < _MATRICES; ++l)
						if ((actualColor >> j) & 1)
							buffer[buf][i][j][k][l] |= B11111111;
						else
							buffer[buf][i][j][k][l] &= B00000000;
		}
	}

	void clear(byte buf = 0)
	{
		fillMatrix(caster(RGB(0)), buf);
	}
	void clearFake();

	void drawRectangle(byte top, byte left, byte bottom, byte right, RGB &out, RGB &in, byte buf = 0);

	void drawCircle(byte centerX, byte centerY, byte rad, RGB &out, RGB &in, byte buf = 0);

	void mergeBuffers(byte source1, byte source2, byte destination = 0);

	bool LEDState(byte row, byte column, byte buf = 0)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);

		int _byte = column >> 3;
		int _rbit = column - (_byte << 3);
		for (int i = 0; i < _COLORS; ++i)
		{
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
			{
				if (!(buffer[buf][i][j][row][_byte] & (1 << _rbit)))
				{
					return true;
				}
			}
		}
		return false;
	}
	bool LEDIsColor(byte row, byte column, byte color, byte buf = 0)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);

		int _byte = column >> 3;
		int _rbit = column - (_byte << 3);
		byte led_colors = 0;
		for (int i = 0; i < _COLORS; ++i)
		{
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
			{
				if (!(buffer[buf][i][j][row][_byte] & (1 << _rbit)))
				{
					led_colors |= (1 << i);
					break;
				}
			}
		}
		if(led_colors == (1 << color))
			return true;
		return false;
	}
	bool LEDHasColor(byte row, byte column, byte color, byte buf = 0)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);

		int _byte = column >> 3;
		int _rbit = column - (_byte << 3);
		byte led_colors = 0;
		for (int i = 0; i < _COLORS; ++i)
		{
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
			{
				if (!(buffer[buf][i][j][row][_byte] & (1 << _rbit)))
				{
					led_colors |= (1 << i);
					break;
				}
			}
		}
		if (led_colors & (1 << color))
			return true;
		return false;
	}

	bool setBuffer(String & s, byte space = 0)
	{
		s = utf8ascii(s);
		int len = s.length();
		//load to buffer
		lastIndexColor = lastRowUsed = 0;

		for (int i = 0; i < len; ++i)
		{
			if (!appendCharacter(s[i]))
				return false;
			for (int j = 0; j < space && lastRowUsed < _MAX_BUFFER_SIZE; ++j)
				fakeBuffer[lastRowUsed++] = 0;
		}
		return true;
	}
	bool setBuffer(char *s, byte space = 0)
	{
		utf8ascii(s);
		int len = strlen(s);
		//load to buffer
		lastIndexColor = lastRowUsed = 0;

		for (int i = 0; i < len; ++i)
		{
			if (!appendCharacter(s[i]))
				return false;
			for (int j = 0; j < space && lastRowUsed < _MAX_BUFFER_SIZE; ++j)
				fakeBuffer[lastRowUsed++] = 0;
		}
		return true;
	}

	bool setBuffer(byte s[], int len)
	{
		//load to buffer
		lastIndexColor = lastRowUsed = 0;

		for (int i = 0; i < len; ++i)
		{
			fakeBuffer[lastRowUsed++] = s[i];
		}
		return true;
	}

	bool loadCharacter(byte character, unsigned int &start)
	{
		character_handler ch(character);
		if (ch.size + start > _MAX_BUFFER_SIZE)
			return false;
		for (int i = 0; i < ch.size; ++i)
			fakeBuffer[start++] = ch.getByte(i);
		return true;
	}
	bool appendCharacter(byte character)
	{
		if (lastRowUsed < 0)
			return false;
		character_handler ch(character);
		if (ch.size + lastRowUsed > _MAX_BUFFER_SIZE)
			return false;
		for (int i = 0; i < ch.size; ++i)
			fakeBuffer[lastRowUsed++] = ch.getByte(i);
		return true;
	}

	void transfer(int bitBCM, int row)
	{
		SPI.beginTransaction(spiCONF);
		if(!hidden)
			for (int j = _MATRICES - 1; j >= 0; --j)//color
				for (int i = 0; i < 3; ++i) //leds per row
					SPI.transfer(buffer[MatrixToDraw][i][bitBCM][row][j]);
		else
			for (int j = _MATRICES - 1; j >= 0; --j)//color
				for (int i = 0; i < 3; ++i) //leds per row
					SPI.transfer(B11111111);
		SPI.transfer(1 << row);
		SPI.endTransaction();
	}

	void show(int from, int to, RGB &color, unsigned long speed, byte mode)
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

	void loadToRealBufferHorizontally(int from, RGB color)
	{
		byte value;
		for (int i = 0; i < _COLUMNS; ++i)
		{
			if (i + from < 0 || i + from >= lastRowUsed)
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
	void loadToRealBufferVertically(int from, int k, RGB color, byte mode)
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

	unsigned int getLastRowUsed()
	{
		return lastRowUsed;
	}
	unsigned int getlastIndexColor()
	{
		return lastIndexColor;
	}

	void hide()
	{
		hidden = true;
	}
	void unhide()
	{
		hidden = false;
	}

	void setBufferToDraw(byte buf)
	{
		buf = constrain(buf, 0, 1);
		MatrixToDraw = buf;
	}

	RGB getLEDRGB(int row, int column)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);

		RGB r;
		int _byte = column >> 3;
		int _rbit = column - (_byte << 3);
		byte led_colors;
		for (int i = 0; i < _COLORS; ++i)
		{
			led_colors = B11111111;
			for (int j = 0; j < _BCM_RESOLUTION; ++j)
			{
				if (!(buffer[0][i][j][row][_byte] & (1 << _rbit)))
				{
					led_colors &= ~(1 << j);
				}
			}
			if (i == 0)
			{
				r.red = led_colors;
			}
			else if (i == 1)
			{
				r.green = led_colors;
			}
			else if (i == 2)
			{
				r.blue = led_colors;
			}
		}
		return r;
	}
	byte getLEDRGBValue(byte row, byte column, byte color)
	{
		row = constrain(row, 0, _ROWS - 1);
		column = constrain(column, 0, _COLUMNS - 1);
		color = constrain(color, 0, 3);

		int _byte = column >> 3;
		int _rbit = column - (_byte << 3);
		byte color = B11111111;
		for (int j = 0; j < _BCM_RESOLUTION; ++j)
		{
			if (!(buffer[0][color][j][row][_byte] & (1 << _rbit)))
			{
				color &= ~(1 << j);
			}
		}
		return ~color;
	}
	byte getLEDRed(byte row, byte column)
	{
		return getLEDRGBValue(row, column, 0);
	}
	byte getLEDGreen(byte row, byte column)
	{
		return getLEDRGBValue(row, column, 1);
	}
	byte getLEDBlue(byte row, byte column)
	{
		return getLEDRGBValue(row, column, 2);
	}

	void saveColumn(byte column)
	{
		column = constrain(column, 0, _COLUMNS - 1);

		for (int i = 0; i < _ROWS; ++i)
		{
			if (LEDState(i, column))
				savedColumn[i] = getLEDRGB(i, column);
			else
				savedColumn[i] = RGB(0);
		}
	}

	void restoreSaveColumn(byte column)
	{
		column = constrain(column, 0, _COLUMNS - 1);

		for (int i = 0; i < _ROWS; ++i)
		{
			setLED(i, column, savedColumn[i]);
		}
	}

	byte utf8ascii(byte ascii, byte &last)
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
	String utf8ascii(const String &s)
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
	void utf8ascii(char* s)
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

	void columnasDerecha(int col)
	{
		for (int i = col; i > 0; i--)
		{

			int _byte = col >> 3; //matrix (0 - (MATRICES - 1))
			int _rbit = col - (_byte << 3); //column (0 - 7)
			
			int _byte2 = (col - 1) >> 3; //matrix (0 - (MATRICES - 1))
			int _rbit2 = (col - 1) - (_byte << 3); //column (0 - 7)

			for (int a = 0; a < _COLORS; ++a)
			{
				for (int b = 0; b < _BCM_RESOLUTION; ++b)
				{
					for (int k = 0; k < _ROWS; ++k)
					{
						if (!(buffer[0][a][b][k][_byte] & (1 << _rbit2)))
						{
							buffer[0][a][b][k][_byte] |= (1 << _rbit);
						}
						else
							buffer[0][a][b][k][_byte] &= ~(1 << _rbit);
						buffer[0][a][b][k][_byte] &= ~(1 << _rbit2);
					}
				}
			}
		}
	}
private:
	byte buffer[_BUFFERS][_COLORS][_BCM_RESOLUTION][_ROWS][_MATRICES];
	byte fakeBuffer[_MAX_BUFFER_SIZE];
	RGB savedColumn[_ROWS];
	unsigned int rows, columns, matrices;
	SPISettings spiCONF;
	unsigned int lastRowUsed, lastIndexColor;
	byte MatrixToDraw;
	bool hidden;
};
Matrix M;

//Snake
class snake
{
public:
	snake(Matrix *M = nullptr, RGB sC = RGB(15,0,0), RGB fC = RGB(0, 0, 15), pair<int, int> start = pair<int, int>(0, 0))
	{
		init();
		this->M = M;
		L.pushBack(start);
		snakeColor = sC;
		foodColor = fC;
		points = 0;
		lifes = 3;
	}
	void init()
	{
		state = _SNAKE_START;
		L.reset(64);
		dir = _ARRIBA;
		lose = false;
	}
	byte move()
	{
		if (!M)
			return state = _SNAKE_ERROR;

		pair<int, int> next = L.getFront();
		//snake_disableInterrupt(inter);
		switch (dir)
		{
		case _ARRIBA:
			next._s++;
			break;
		case _ABAJO:
			next._s--;
			break;
		case _IZQUIERDA:
			next._f--;
			break;
		case _DERECHA:
			next._f++;
			break;
		}
		//snake_enableInterrupt(inter);
		//check errors while drawing
		if (next._f < 0 || next._f >= _COLUMNS || next._s < 1 || next._s >= _ROWS)
		{
			switch (dir)
			{
			case _ARRIBA:
				next._s = 1;
				break;
			case _ABAJO:
				next._s = 7;
				break;
			case _IZQUIERDA:
				next._f = _COLUMNS - 1;
				break;
			case _DERECHA:
				next._f = 0;
				break;
			}
		}

		for (int i = 0, len = L.size() - 1; len > 0 && i < len; ++i)
			if (L[i] == next)//llamar a funcion de perdida
				return state = _SNAKE_LOSE;

		L.pushFront(next);

		if (M->LEDState(next._s, next._f) && !(L[L.size() - 1] == next)) //es comida
			return state = _SNAKE_EAT;

		//aqui va el popBack

		return state = _SNAKE_NOTHING;
	}
	void draw()
	{
		if (!M)
			return;
		//probar pintar todo o solo pintar el inicio y borrar el final
		if (state == _SNAKE_NOTHING)
		{
			_DEBUG_PRINT_3(DRAW_NOTHING);
			_DEBUG_PRINT_3(TAIL);
			_DEBUG_PRINT_2(y, L[L.size() - 1]._s);
			_DEBUG_PRINT_2(x, L[L.size() - 1]._f);
			M->setLED(L[L.size() - 1]._s, L[L.size() - 1]._f, caster(RGB(0)));
			_DEBUG_PRINT_3(HEAD);
			_DEBUG_PRINT_2(y, L[0]._s);
			_DEBUG_PRINT_2(x, L[0]._f);
			M->setLED(L[0]._s, L[0]._f, snakeColor);
			_DEBUG_PRINT_2(TAMANIO_ANTES, L.size());
			L.popBack();
			_DEBUG_PRINT_2(TAMANIO_DESPUES, L.size());
		}
		else if (state == _SNAKE_EAT)
		{
			M->setLED(L[0]._s, L[0]._f, snakeColor);
			state = _SNAKE_NOTHING;
		}
		else if (state == _SNAKE_START)
		{
			for (int i = 0, len = L.size(); i < len; ++i)
			{
				M->setLED(L[i]._s, L[i]._f, snakeColor);
			}
		}
		for (int i = 0; i < lifes; ++i)
			M->setLED(0, i, caster(RGB(0, 15, 0)));

		for (int i = 0; i < points; ++i)
			M->setLED(0, i + 8, caster(RGB(15, 15, 0)));
	}
	void setDirection(int dir)
	{
		this->dir = dir;
	}
	int getDirection()
	{
		return this->dir;
	}
	void reset(pair<int, int> start = pair<int, int>(0, 0))
	{
		init();
		L.pushBack(start);
	}

	byte getState()
	{
		return state;
	}
	byte setState(byte st)
	{
		this->state = st;
	}
	void makeFood()
	{
		byte r, c;
		do
		{
			r = random(1, _ROWS);
			c = random(0, _COLUMNS);
		}while (M->LEDState(r, c));
		M->setLED(r, c, caster(RGB(0, 0, 15)));
	}
	void putPiece(byte row, byte col)
	{
		L.pushBack(pair<int, int>(col, row));
	}

	bool die()
	{
		if (lifes == 0)
			return false;
		lifes -= 1;
		return true;
	}

	bool addPoint()
	{
		if (points == 7)
			return false;
		points += 1;
		return true;
	}
	void setPoints(byte p)
	{
		points = p;
	}
	void setLifes(byte l)
	{
		lifes = l;
	}

private:
	List<pair<int, int> > L;
	int dir;
	bool lose;
	static byte inter;
	Matrix *M;
	RGB snakeColor;
	RGB foodColor;
	byte state;
	byte points;
	byte lifes;
};
snake Snake;

void snake_up()
{
	if ((long)(micros() - last_micros[0]) >= debouncing_time * 1000)
	{
		if (Snake.getDirection() != _ABAJO)
			Snake.setDirection(_ARRIBA);
		last_micros[0] = micros();
	}
}

void snake_right()
{
	if ((long)(micros() - last_micros[1]) >= debouncing_time * 1000)
	{
		if (Snake.getDirection() != _IZQUIERDA)
			Snake.setDirection(_DERECHA);
		last_micros[1] = micros();
	}
}

void snake_left()
{
	if ((long)(micros() - last_micros[2]) >= debouncing_time * 1000)
	{
		if (Snake.getDirection() != _DERECHA)
			Snake.setDirection(_IZQUIERDA);
		last_micros[2] = micros();
	}
}

void snake_down()
{
	if ((long)(micros() - last_micros[3]) >= debouncing_time * 1000)
	{
		if(Snake.getDirection() != _ARRIBA)
			Snake.setDirection(_ABAJO);
		last_micros[3] = micros();
	}
}

void snake_bA()
{
	if ((long)(micros() - last_micros[4]) >= debouncing_time * 1000)
	{

		last_micros[4] = micros();
	}
}

void snake_bB()
{
	if ((long)(micros() - last_micros[5]) >= debouncing_time * 1000)
	{

		last_micros[5] = micros();
	}
}

void snake_enableInterrupts()
{
	attachInterrupt(digitalPinToInterrupt(2), snake_up, RISING);
	attachInterrupt(digitalPinToInterrupt(3), snake_right, RISING);
	attachInterrupt(digitalPinToInterrupt(18), snake_down, RISING);
	attachInterrupt(digitalPinToInterrupt(19), snake_left, RISING);
	attachInterrupt(digitalPinToInterrupt(20), snake_bA, RISING);
	attachInterrupt(digitalPinToInterrupt(21), snake_bB, RISING);
}
void snake_enableInterrupt(byte interrupt)
{
	if (interrupt & _SNAKE_UP)
		attachInterrupt(digitalPinToInterrupt(2), snake_up, RISING);
	if (interrupt & _SNAKE_RIGHT)
		attachInterrupt(digitalPinToInterrupt(3), snake_right, RISING);
	if (interrupt & _SNAKE_LEFT)
		attachInterrupt(digitalPinToInterrupt(18), snake_down, RISING);
	if (interrupt & _SNAKE_DOWN)
		attachInterrupt(digitalPinToInterrupt(19), snake_left, RISING);
	if (interrupt & _SNAKE_BA)
		attachInterrupt(digitalPinToInterrupt(20), snake_bA, RISING);
	if (interrupt & _SNAKE_BB)
		attachInterrupt(digitalPinToInterrupt(21), snake_bB, RISING);
}
void snake_disableInterrupts()
{
	detachInterrupt(2);
	detachInterrupt(3);
	detachInterrupt(18);
	detachInterrupt(19);
	detachInterrupt(20);
	detachInterrupt(21);
}
void snake_disableInterrupt(byte interrupt)
{
	if (interrupt & _SNAKE_UP)
		detachInterrupt(2);
	if (interrupt & _SNAKE_RIGHT)
		detachInterrupt(3);
	if (interrupt & _SNAKE_LEFT)
		detachInterrupt(18);
	if (interrupt & _SNAKE_DOWN)
		detachInterrupt(19);
	if (interrupt & _SNAKE_BA)
		detachInterrupt(20);
	if (interrupt & _SNAKE_BB)
		detachInterrupt(21);
}

class snakeHandler
{
public:
	snakeHandler(Matrix *Mx = nullptr, snake *Sx = nullptr)
	{
		this->M = Mx;
		this->S = Sx;
	}
	void setup()
	{
		*S = snake(M, RGB(15, 0, 0), RGB(0, 0, 15), pair<int, int>(9, 4));
		S->putPiece(4, 8);
		S->putPiece(4, 7);
		S->putPiece(4, 6);
		S->setDirection(_DERECHA);
		inter = _SNAKE_UP | _SNAKE_DOWN | _SNAKE_RIGHT | _SNAKE_LEFT;
		snake_enableInterrupt(inter);
		M->fillMatrix(caster(RGB(0, 0, 0)));
		S->draw();
		S->makeFood();
	}

	bool loop()
	{
		byte res = S->move();
		_DEBUG_PRINT(res);
		if (res == _SNAKE_LOSE)
		{
			delay(300);
			//
			M->hide();
			delay(500);
			M->unhide();
			delay(500);
			M->hide();
			delay(500);
			M->unhide();
			delay(500);
			//
			if (!S->die())
			{
				//S->setPoints(0);
				//S->setLifes(3);
				return false;
			}
			M->fillMatrix(caster(RGB(0)));
			S->reset(pair<int, int>(9, 4));
			S->putPiece(4, 8);
			S->putPiece(4, 7);
			S->putPiece(4, 6);
			S->setDirection(_DERECHA);
			S->draw();
			S->makeFood();
		}
		else if (res == _SNAKE_ERROR)
		{
			while (true)
			{
				digitalWrite(13, HIGH);
				delay(300);
				digitalWrite(13, LOW);
				delay(300);
			}
		}
		delay(150);
		S->draw();
		if (res == _SNAKE_EAT)
		{
			if (!S->addPoint())
			{
				delay(300);
				//
				M->hide();
				delay(500);
				M->unhide();
				delay(500);
				M->hide();
				delay(500);
				M->unhide();
				delay(500);
				//
				S->setPoints(0);
				S->setLifes(3);
				//
				M->fillMatrix(caster(RGB(0)));
				S->reset(pair<int, int>(9, 4));
				S->putPiece(4, 8);
				S->putPiece(4, 7);
				S->putPiece(4, 6);
				S->setDirection(_DERECHA);
				S->draw();
				S->makeFood();
				return true;
			}
			S->setState(_SNAKE_START);
			S->draw();
			S->makeFood();
		}
		delay(300);
		return true;
	}
	void reset()
	{

	}
private:
	snake *S;
	Matrix *M;
};

snakeHandler SH(&M, &Snake);

//T2
#define _TETRIS_RIGHT 0
#define _TETRIS_LEFT 1
#define _TETRIS_DOWN 2
#define _TETRIS_O
#define _TETRIS_T	
#define _TETRIS_S	
#define _TETRIS_Z
#define _TETRIS_I
#define _TETRIS_J
#define _TETRIS_L
byte UPN[] = { 0x10,0x00,0x24,0x24,0x24,0x38,0x00,0x1C,0x24,0x1C,0x04,0x00,0x1C,0x24,0x24,0x24,00 };
byte JUEGO_TERMINADO[] = { 0x8A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						   0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x20, 0x20, 0x20, 0x20, 0x24, 0x24,
						   0x18, 0x00, 0x33, 0x22, 0x22, 0x22, 0x32, 0x6C, 0x00, 0x38, 0x44, 0x7C,
						   0x04, 0x04, 0x78, 0x00, 0x6C, 0x32, 0x22, 0x3C, 0x20, 0x1C, 0x00,
						   0x38, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x7F, 0x49, 0x08, 0x08, 0x08,
						   0x08, 0x08, 0x1C, 0x00, 0x1C, 0x22, 0x3E, 0x02, 0x02, 0x3C, 0x00, 0x36,
						   0x0C, 0x04, 0x04, 0x04, 0x3E, 0x00, 0x3E, 0x54, 0x54, 0x54, 0x54, 0xFE,
						   0x00, 0x10, 0x00, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x36, 0x4C,
						   0x44, 0x44, 0x44, 0x6E, 0x00, 0x1C, 0x22, 0x3C, 0x22, 0x22, 0x7C, 0x00,
						   0x30, 0x20, 0x2C, 0x32, 0x22, 0x22, 0x22, 0x7C, 0x00, 0x1C, 0x22, 0x22,
						   0x22, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte tPieces[19][4] = { { 0x03, 0x03, 0x00, 0x00 }, //O
					   { 0x03, 0x01, 0x01, 0x00 },  //L1
					   { 0x07, 0x04, 0x00, 0x00 },  //L2
					   { 0x02, 0x02, 0x03, 0x00 },  //L3
					   { 0x01, 0x07, 0x00, 0x00 },  //L4
					   { 0x01, 0x01, 0x03, 0x00 },  //J1
					   { 0x07, 0x01, 0x00, 0x00 },  //J2
					   { 0x03, 0x02, 0x02, 0x00 },  //J3
					   { 0x04, 0x07, 0x00, 0x00 },  //J4
					   { 0x01, 0x01, 0x01, 0x01 },  //I1
					   { 0x0F, 0x00, 0x00, 0x00 },  //I2
					   { 0x01, 0x03, 0x02, 0x00 },  //S1
					   { 0x06, 0x03, 0x00, 0x00 },  //S2
					   { 0x01, 0x03, 0x01, 0x00 },  //T1
					   { 0x07, 0x02, 0x00, 0x00 },  //T2
					   { 0x02, 0x03, 0x02, 0x00 },  //T3
					   { 0x02, 0x07, 0x00, 0x00 },  //T4
					   { 0x02, 0x03, 0x01, 0x00 },  //Z1
					   { 0x03, 0x06, 0x00, 0x00 } }; //Z2
RGB tColors[7] = { RGB(15, 15, 0), RGB(0, 0, 15), RGB(15, 5, 0), RGB(0, 15, 15), RGB(15, 0, 0), RGB(15, 0, 15), RGB(0, 15, 0) };

byte juego = 0;
bool juegoPongIniciado, juegoTetrisIniciado, juegoSnakeIniciado;

#define _BALL_IZQUIERDA false
#define _BALL_DERECHA true
#define _BALL_ARRIBA false
#define _BALL_ABAJO true

void GAME_OVER()
{
	for (int i = 1, len = JUEGO_TERMINADO[0] - 1; i <= len - 14; ++i)
	{
		M.clear();
		for (int j = 0; j < 16; ++j)
		{
			M.setColumnByte(j, JUEGO_TERMINADO[i + j], caster(RGB(15, 0, 0)));
		}
		delay(70);
	}
	delay(200);
}

class Sound
{
public:
	Sound(int p = 9) :pin(p) {}
	void makeSound(int note, int duration)
	{
		tone(pin, note, duration);
		delay(10);
		noTone(pin);
	}
private:
	int pin;
} S;

class Ball
{
public:
	Ball() :color(RGB(15, 0, 0)) {}
	Ball(int x, int y)
	{
		pos.first = x;
		pos.second = y;
		h = _BALL_DERECHA;
		v = _BALL_ARRIBA;
		color = RGB(15, 0, 0);
		_DEBUG_PRINT_3(BOLA_SETEADA);
	}
	void draw()
	{
		M->setLED(pos.first, pos.second, color);
		_DEBUG_PRINT_3(BOLA_PINTADA_EN:);
		_DEBUG_PRINT_2(columna, pos.first);
		_DEBUG_PRINT_2(fila, pos.second);
		_DEBUG_PRINT_3(*********);
		_STOP;
	}
	void clear()
	{
		M->clearLED(pos.first, pos.second);
	}
	void chekWall()
	{
		if (h == _BALL_DERECHA && next.first == _ROWS)
			h = _BALL_IZQUIERDA;
		else if (h == _BALL_IZQUIERDA && next.first == -1)
			h = _BALL_DERECHA;
		if (v == _BALL_ARRIBA && next.second == -1)
			v = _BALL_ABAJO;
	}
	void updatePos()
	{
		pos.first = next.first;
		pos.second = next.second;
	}
	void calculateNext()
	{
		if (h == _BALL_IZQUIERDA)
			next.first = pos.first - 1;
		else
			next.first = pos.first + 1;

		if (v == _BALL_ABAJO)
			next.second = pos.second + 1;
		else
			next.second = pos.second - 1;
	}
	int getX()
	{
		return pos.first;
	}
	void setX(int pX)
	{
		this->pos.first = pX;
	}
	int getY()
	{
		return pos.second;
	}
	void setY(int pY)
	{
		this->pos.second = pY;
	}
	bool getDirX()
	{
		return this->h;
	}
	void setDirX(bool dX)
	{
		this->h = dX;
	}
	bool getDirY()
	{
		return this->v;
	}
	void setDirY(bool dY)
	{
		this->v = dY;
	}

	int getNextX()
	{
		return next.first;
	}
	int getNextY()
	{
		return next.second;
	}
	void setColor(RGB &color)
	{
		this->color = color;
	}
	void setMatrix(Matrix *M)
	{
		this->M = M;
	}
	void reset(int x, int y)
	{
		pos.first = x;
		pos.second = y;
		h = _BALL_DERECHA;
		v = _BALL_ARRIBA;
		color = RGB(15, 0, 0);
	}
private:
	RGB color;
	coord pos;
	coord next;
	bool h, v; //h=false=izq, h=true=der, v=false=arriba, v=true=abajo
	Matrix *M;
};

class Pad
{
public:
	Pad(int pin = A0, int y = 15) :pin(A0), color(RGB(0, 0, 15)), pos(0,15) {}
	void clear()
	{
		M->clearLED(pos.first - 1, pos.second);
		M->clearLED(pos.first, pos.second);
		M->clearLED(pos.first + 1, pos.second);
	}
	void draw()
	{
		M->setLED(pos.first - 1, pos.second, color);
		M->setLED(pos.first, pos.second, color);
		M->setLED(pos.first + 1, pos.second, color);
	}
	void update()
	{
		int tmp = analogRead(pin);
		if (tmp <= 169)
		{
			pos.first = 1;
		}
		else if (tmp <= 339)
		{
			pos.first = 2;
		}
		else if (tmp <= 509)
		{
			pos.first = 3;
		}
		else if (tmp <= 679)
		{
			pos.first = 4;
		}
		else if (tmp <= 849)
		{
			pos.first = 5;
		}
		else
		{
			pos.first = 6;
		}
	}
	int getX()
	{
		return pos.first;
	}
	void setX(int pX)
	{
		this->pos.first = pX;
	}
	int getY()
	{
		return pos.second;
	}
	void setY(int pY)
	{
		this->pos.second = pY;
	}
	void setColor(RGB &color)
	{
		this->color = color;
	}
	void setMatrix(Matrix *M)
	{
		this->M = M;
	}
private:
	Matrix *M;
	RGB color;
	int vel;
	coord pos;
	int pin;
};

class Pong
{
public:
	Pong() {}
	void setup(Matrix *M = nullptr, RGB BallColor = RGB(15,0,0), RGB PadColor = RGB(0,0,15), RGB barColor = RGB(0,15,0))
	{
		vel = 150;
		this->M = M;
		this->barColor = barColor;
		B.setX(random(0, _ROWS));
		B.setY(0);
		B.setColor(BallColor);
		B.setMatrix(M);
		P.setColor(PadColor);
		P.setMatrix(M);
		lifes = 5;
		M->fillMatrix(caster(RGB(0)));
		_DEBUG_PRINT_3(TODO_SETEADO);
		reset();
		B.draw();
		delay(100);
		//P.update();
		//P.draw();
		//barTimeToActivate = millis() + 15;
		//lastRefresh = millis();
		timeToActivateAgain = millis() + 1600;
	}
	bool update()
	{
		if (millis() - refreshBar >= 90)
		{
			P.clear();
			P.update();
			P.draw();
			refreshBar = millis();
		}
		//if (millis() - barTimeToActivate >= 2500)
		//{
			if (!barActive && B.getY() == 0 && (millis() > timeToActivateAgain))
			{
				barActive = true;
				if (barActive)
				{
					barTime = millis() + random(2300, 3100);
					if (setBar())
						drawBar();
					else
						barActive = false;
				}
			}
			//barTimeToActivate = millis();
		//}

		if (barActive && millis() > barTime)
		{
			barActive = false;
			timeToActivateAgain = millis() + 500;
			clearBar();
		}

		//actualizacion
		if (millis() - lastRefresh >= vel)
		{
			B.clear();
			B.calculateNext();
			bool col = checkColission();
			B.calculateNext();
			B.updatePos();
			B.draw();
			_DEBUG_PRINT(B.getX());
			_DEBUG_PRINT(B.getY());
			_STOP;
			if (col)
			{
				//perdio una vida
				delay(200);
				_DEBUG_PRINT(vel);
				lifes--;
				timeToActivateAgain = millis() + 2500;
				vel = 200;
				if (lifes == 0)
				{
					//perdio para siempre
					return false;
				}
				else
					curtain();
				reset();
			}
			lastRefresh = millis();
		}
		return true;
	}
	void pongSound()
	{
		tone(9, NOTE_C, 500);
		delay(10);
		noTone(9);
	}
	void barSound()
	{
		tone(9, NOTE_E, 500);
		delay(10);
		noTone(9);
	}
	int checkColission() //bool retornando 0 en caso nada
	{
		int bX = B.getNextX(), bY = B.getNextY(), pX = P.getX(), pY = P.getY();

		//PAD
		if (bY == pY)
		{
			_DEBUG_PRINT_3(EN_LA_SIGUIENTE_CHOCA);
			if ((B.getDirX() == _BALL_IZQUIERDA && bX == pX + 1) || (B.getDirX() == _BALL_DERECHA && bX == pX - 1))
			{
				B.setDirX(!B.getDirX());
				B.setDirY(_BALL_ARRIBA);
				pongSound();
			}
			else if ((bX >= pX - 1 && bX <= pX + 1) || (B.getX() >= pX - 1 && B.getX() <= pX + 1))
			{
				pongSound();
				B.setDirY(_BALL_ARRIBA);
			}
			else
			{
				makeSound(NOTE_CC + 500);
				return 1;
			}
		}
		else if (barActive && bY == barY)
		{
			barSound();
			_DEBUG_PRINT_3(CHOCA_CON_LA_BARRA);
			if(vel > 40)
				vel -= (random(0, 10));
			B.setDirY(_BALL_ARRIBA);
		}

		if (B.getDirX() == _BALL_DERECHA && bX == _ROWS)
		{
			pongSound();
			_DEBUG_PRINT_3(PARED_DERECHA);
			B.setDirX(_BALL_IZQUIERDA);
		}
		else if (B.getDirX() == _BALL_IZQUIERDA && bX == -1)
		{
			pongSound();
			_DEBUG_PRINT_3(PARED_IZQUIERDA);
			B.setDirX(_BALL_DERECHA);
		}
		if (B.getDirY() == _BALL_ARRIBA && bY == -1)
		{
			pongSound();
			_DEBUG_PRINT_3(PARED_ARRIBA);
			B.setDirY(_BALL_ABAJO);
		}
		return 0;
	}
	bool setBar()
	{	
		int r = random(4, 6);
		_DEBUG_PRINT_2(random, r);
		barY = r;
		return true;
	}
	void drawBar()
	{
		M->setEntireColumn(barY, barColor);
	}
	void clearBar()
	{
		M->setEntireColumn(barY, caster(RGB(0)));
	}
	void reset()
	{
		M->fillMatrix(caster(RGB(0)));
		B.reset(4, 14);
		B.setX(random(0, _ROWS));
		B.setY(0);
		B.setColor(caster(RGB(random(0, 16), random(0, 16), random(0, 16))));
		randomSeed(analogRead(A1));
		P.setColor(caster(RGB(random(0, 16), random(0, 16), random(0, 16))));
		barActive = false;
		barTime = 0;
	}
	void makeSound(int note)
	{
		tone(9, note, 500);
		delay(10);
		noTone(9);
	}
	void curtain()
	{

		for (int i = 0; i < _COLUMNS; ++i)
		{
			makeSound(NOTE_CC);
			M->setEntireColumn(i, caster(RGB(15,0,0)));
			delay(50);
		}
		for (int i = _COLUMNS - 1; i >= 0; --i)
		{
			makeSound(NOTE_C);
			M->setEntireColumn(i, caster(RGB(0)));
			delay(50);
		}
	}
private:
	Ball B;
	Pad P;
	byte lifes;
	bool barActive;
	byte barY;
	unsigned long barTime;
	unsigned long barTimeToActivate;
	unsigned long timeToActivateAgain;
	RGB barColor;
	bool first;
	Matrix *M;
	int vel;
	unsigned long lastRefresh;
	unsigned long refreshBar;
};

Pong P;

class TetrisPiece
{
public:
	TetrisPiece(int x, int y)
	{
		piecePos = piece = random(7);
		next.first = pos.first = x;
		next.second = pos.second = y;
		color = tColors[piece];
		dieColor = RGB(15, 0, 0);
	}
	TetrisPiece()
	{
		reset();
	}
	byte& operator[](const int index)
	{
		return data[index];
	}
	int getX()
	{
		return pos.first;
	}
	void setX(int nX)
	{
		this->pos.first = nX;
	}
	int getNextX()
	{
		return next.first;
	}
	int getY()
	{
		return pos.second;
	}
	void setY(int nY)
	{
		this->pos.second = nY;
	}
	int getNextY()
	{
		return next.second;
	}
	void setColor(RGB cl)
	{
		color = cl;
	}
	void setMatrix(Matrix *M)
	{
		this->M = M;
	}
	void turn()
	{
		byte maybePiecePos = piecePos;
		switch (piece)
		{
		case 0:
			maybePiecePos = 0;
			break;
		case 1:
			maybePiecePos++;
			if (maybePiecePos == 5)
				maybePiecePos = 1;
			break;
		case 2:
			maybePiecePos++;
			if (maybePiecePos == 9)
				maybePiecePos = 5;
			break;
		case 3:
			maybePiecePos++;
			if (maybePiecePos == 11)
				maybePiecePos = 9;
			break;
		case 4:
			maybePiecePos++;
			if (maybePiecePos == 13)
				maybePiecePos = 11;
			break;
		case 5:
			maybePiecePos++;
			if (maybePiecePos == 17)
				maybePiecePos = 13;
			break;
		case 6:
			maybePiecePos++;
			if (maybePiecePos == 19)
				maybePiecePos = 17;
			break;
		}
		byte posData[4];
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				posData[i] = tPieces[maybePiecePos][i];
			}
		}
		if (checkTurnRight(posData) && checkTurnLeft(posData) && checkTurnDown(posData))
		{
			for (int i = 0; i < 4; ++i)
			{
				data[i] = posData[i];
			}
			piecePos = maybePiecePos;
		}
	}
	void calculateNext(int dir)
	{
		switch (dir)
		{
		case _TETRIS_RIGHT:
			next.first++;
			break;
		case _TETRIS_LEFT:
			next.first--;
			break;
		case _TETRIS_DOWN:
			next.second++;
			break;
		}
	}
	bool move(int dir)
	{
		calculateNext(dir);
		switch (dir)
		{
		case _TETRIS_RIGHT:
			_DEBUG_PRINT_3(MOVER_DERECHA);
			if (!checkRight())
			{
				_DEBUG_PRINT_3(NOPE);
				next = pos;
			}
			break;
		case _TETRIS_LEFT:
			_DEBUG_PRINT_3(MOVER_IZQUIERDA);
			if (!checkLeft())
			{
				_DEBUG_PRINT_3(NOPE);
				next = pos;
			}
			break;
		case _TETRIS_DOWN:
			_DEBUG_PRINT_3(MOVER_ABAJO);
			if (!checkDown())
			{
				_DEBUG_PRINT_3(NOPE);
				next = pos;
				return 0;
			}
			else
			{
				moveSound(NOTE_G);
			}
			break;
		}
		pos = next;
		return 1;
	}
	void moveSound(int note)
	{
		tone(9, note, 500);
		delay(10);
		noTone(9);
	}
	void draw()
	{
		for (int i = 0; i < 4; ++i) //y
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j) //x
			{
				if ((data[i] >> j) & 1)
				{
					M->setLED(next.first + j, next.second + i, color);
				}
			}
		}
	}
	bool checkRight()
	{
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((data[i] >> j) & 1)
				{
					if (next.first + j >= _ROWS)
						return false;
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}
	bool checkLeft()
	{
		if (next.first < 0)
			return false;
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((data[i] >> j) & 1)
				{
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}
	bool checkDown()
	{
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((data[i] >> j) & 1)
				{
					if (next.second + i >= _COLUMNS)
						return false;
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}

	bool checkTurnRight(byte d[])
	{
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((d[i] >> j) & 1)
				{
					if (next.first + j >= _ROWS)
						return false;
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}
	bool checkTurnLeft(byte d[])
	{
		if (next.first < 0)
			return false;
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((d[i] >> j) & 1)
				{
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}
	bool checkTurnDown(byte d[])
	{
		for (int i = 0; i < 4; ++i)
		{
			if (next.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((d[i] >> j) & 1)
				{
					if (next.second + i >= _COLUMNS)
						return false;
					if (M->LEDState(next.first + j, next.second + i))
						return false;
				}
			}
		}
		return true;
	}

	void clear()
	{
		for (int i = 0; i < 4; ++i)
		{
			if (pos.second + i < 0)
				continue;
			for (int j = 0; j < 4; ++j)
			{
				if ((data[i] >> j) & 1)
				{
					M->clearLED(pos.first + j, pos.second + i);
				}
			}
		}
	}
	void die()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if ((data[i] >> j) & 1)
				{
					M->setLED(next.first + j, next.second + i, dieColor);
				}
			}
		}
	}
	bool reset()
	{
		piece = random(7);
		color = tColors[piece];
		_DEBUG_PRINT(piece);
		switch (piece)
		{
		case 0:
			piecePos = 0;
			break;
		case 1:
			piecePos = 1;
			break;
		case 2:
			piecePos = 5;
			break;
		case 3:
			piecePos = 9;
			break;
		case 4:
			piecePos = 11;
			break;
		case 5:
			piecePos = 13;
			break;
		case 6:
			piecePos = 17;
			break;
		}

		tail.first = 0;
		tail.second = 0;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				data[i] = tPieces[piecePos][i];
			}
			if (data[i] == 0)
			{
				tail.second = i - 1;
			}
			for (int j = 0; j < 4; ++j)
			{
				if (((data[i] >> j) & 1) && j > tail.first)
				{
					tail.first = j;
				}
			}
		}

		next.first = pos.first = (_ROWS - tail.first) / 2;
		next.second = pos.second = (-tail.second);
		_DEBUG_PRINT(next.first);
		_DEBUG_PRINT(next.second);
		//comprobar que hay lugar
		if (checkDown())
			return true;
		return false;
	}
	void checkRow()
	{
		for (int i = _COLUMNS - 1; i >= 0; --i)
		{
			byte cont = 0;
			for (int j = 0; j < _ROWS; ++j)
			{
				if (M->LEDState(j, i))
				{
					cont += (1 << j);
				}
			}
			if (cont == 0)
				return;
			if (cont == 255)
			{
				M->saveColumn(i);
				for (int k = 0; k < 4; ++k)
				{
					moveSound(NOTE_E);
					M->setEntireColumn(i, caster(RGB(0)));
					delay(70);
					moveSound(NOTE_D);
					M->restoreSaveColumn(i);
					//M->setEntireColumn(i, caster(RGB(15, 0, 0)));
					delay(70);
				}
				M->setEntireColumn(i, caster(RGB(0)));
				for (int k = i; k > 0; --k)
				{
					for (int z = 0; z < _ROWS; ++z)
					{
						_DEBUG_PRINT_2(columna, k - 1);
						_DEBUG_PRINT_2(fila, z);
						bool led = M->LEDState(z, k - 1);
						_DEBUG_PRINT(led);
						if (led)
						{
							//M->setLED(z, k - 1, caster(RGB(15, 0, 0)));
							RGB r = M->getLEDRGB(z, k - 1);
							_DEBUG_PRINT(r.red);
							_DEBUG_PRINT(r.green);
							_DEBUG_PRINT(r.blue);
							M->setLED(z, k, r);
							M->clearLED(z, k - 1);
						}
						_STOP;
					}
				}
				M->setEntireColumn(0, caster(RGB(0)));
				i += 1;
			}
		}
	}
private:
	byte piece;
	coord pos;
	coord next;
	coord tail;
	byte data[4];
	RGB color;
	RGB dieColor;
	Matrix *M;
	byte piecePos;
};

class Tetris
{
public:
	Tetris() {}
	void setup(Matrix *M = nullptr)
	{
		M->clear();
		Tt.reset();
		Tt.setMatrix(M);
		Tt.draw();
		vel = 500;
	}
	void makeSound(int note)
	{
		tone(9, note, 500);
		delay(10);
		noTone(9);
	}
	bool update()
	{
		Tt.clear();
		if (!Tt.move(_TETRIS_DOWN))
		{
			Tt.draw();
			vel = 500;
			makeSound(NOTE_D);
			delay(10);
			makeSound(NOTE_G);
			Tt.checkRow();

			if (!Tt.reset())
				return false;

			Tt.draw();
			_DEBUG_PRINT_3(STOP);
		}
		else
		{
			Tt.draw();
		}
		delay(vel);
		return true;
	}
	void setMatrix(Matrix *M)
	{
		this->M = M;
	}
	void setVelocity(int v)
	{
		vel = v;
	}
	TetrisPiece Tt;
private:
	Matrix *M;
	int vel;
};
Tetris T;

void tetris_right()
{
	if ((long)(micros() - last_micros[0]) >= debouncing_time * 1000)
	{
		T.Tt.clear();
		T.Tt.move(_TETRIS_RIGHT);
		T.Tt.draw();
		last_micros[0] = micros();
	}
}

void tetris_left()
{
	if ((long)(micros() - last_micros[1]) >= debouncing_time * 1000)
	{
		T.Tt.clear();
		T.Tt.move(_TETRIS_LEFT);
		T.Tt.draw();
		last_micros[1] = micros();
	}
}

void tetris_down()
{
	if ((long)(micros() - last_micros[2]) >= debouncing_time * 1000)
	{
		T.setVelocity(50);
		last_micros[2] = micros();
	}
}

void tetris_turn()
{
	if ((long)(micros() - last_micros[3]) >= debouncing_time * 1000)
	{
		T.Tt.clear();
		T.Tt.turn();
		T.Tt.draw();
		last_micros[3] = micros();
	}
}

void game()
{
	if ((long)(micros() - last_micros[4]) >= debouncing_time * 1000)
	{
		juego = (juego + 1) % 4;
		if (juego == 0)
		{
			juegoPongIniciado = juegoTetrisIniciado = 0;
		}
		else if (juego == 1)
		{
			juegoTetrisIniciado = 0;
		}
		else if (juego == 2)
		{
			juegoPongIniciado = 0;
		}
		else if (juego == 3)
		{
			juegoSnakeIniciado = 0;
		}
		last_micros[4] = micros();
	}
}

class Controller
{
public:

private:
	Matrix M;
};

//

//14, 13, 12, 11 => 51 41 40 52
#define latch				40 //pin 12 storage ST-CP
#define enable				41 //pin 13 OE
#define spi_data				51 //pin 14 data DS
#define spi_clock			52 //pin 11 SH-CP
//colors[r,g,b][bcm][filas][columnas/8 = bytes]
//byte colors[3][_BCM_RESOLUTION][_ROWS][_MATRICES]; // bool colors[3][4][8][8*_MATRICES];
//I don't use bool or boolean because the implementation take up more space than expected (1 byte instead 1 bit)... bool colors[3][_ROWS][_COLUMNS][_BCM_RESOLUTION]


void setup()
{
	randomSeed(analogRead(A1));
	cli();
	_DEBUG_INIT(9600);
	_PIN_TO_DEBUG(25);
	row = 0;
	bitBCM = 0;
	counterBCM = 0;

	pinMode(13, OUTPUT); //used for error

	pinMode(latch, OUTPUT);
	pinMode(spi_data, OUTPUT);
	pinMode(spi_clock, OUTPUT);
	setupISR();
	M.init(_CLOCK_SPEED_DIV_2, MSBFIRST, SPI_MODE0);
	M.fillMatrix(caster(RGB(0)));

	//attachInterrupt(digitalPinToInterrupt(21), game, RISING);
	//SH.setup();
	//juego = 0;

	sei();
}

void loop()
{
	//animations
	/*M.setBuffer("a b c d e f g h i j k");
	//M.show(0, M.getLastRowUsed(), caster(RGB(6, 0, 15)), 3000, _STATIC);
	//Serial.println(actEffect);
	while (true)
	{
		//M.show(0, M.getLastRowUsed(), caster(RGB(0, 15, 0)), 3000, _STATIC);
		//delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _RIGHT_TO_LEFT);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _LEFT_TO_RIGHT);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _TOP_TO_BOTTOM);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _BOTTOM_TO_TOP);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _TOP_TO_BOTTOM | _FADE_IN);
		delay(60);
		M.show(0, M.getLastRowUsed(), caster(RGB(15, 0, 0)), 100, _BOTTOM_TO_TOP | _FADE_IN);
		delay(60);
		M.fillMatrix(caster(RGB(0)));
		delay(120);
	}*/

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

	//Control
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
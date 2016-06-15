
#ifndef _MACROS_H_
#define _MACROS_H_

#include "arduino.h"

#define _RED					0
#define _GREEN				1
#define _BLUE				2

#define _RAINBOW				1
#define _WORD_BY_WORD			2
#define _ONE_COLOR			3

#define _FIRST_CYCLE			8		//for first bit by each row (1 cycle * 8 rows)
#define _SECOND_CYCLE		24		//for second bit (2 cycle * 8 rows) + _FIRST_CYCLE
#define _THIRD_CYCLE			56		//for third bit (4 cycles * 8 rows) + _SECOND_CYCLE
#define _FOURTH_CYCLE		120		//for four bit (8 cycles * 8 rows) + _SECOND_CYCLE

#define _MATRICES			2
#define _ROWS				8
#define _COLUMNS				(_MATRICES << 3) //leds per row

#define _COLORS				3
#define _BUFFERS			1

#define _BCM_RESOLUTION		4
#define _MAX_VALUE_BCM		((1 << _BCM_RESOLUTION) - 1)
#define _SPEED				0x25
#define _DELAY				1000

#define _CLOCK_SPEED_DIV_2	8000000

#define _USER_FRIENDLY
#define _USES_FRIENDLY

#define _MAX_BUFFER_SIZE				400
//1300
#define _MAX_COLOR_BUFFER_SIZE		150

#define _RIGHT_TO_LEFT		1
#define _LEFT_TO_RIGHT		2
#define _TOP_TO_BOTTOM		4
#define _BOTTOM_TO_TOP		8
#define _STATIC				16
#define _FADE_IN				32
#define _USE_LAST			64
#define _LEFT_SPACES			128
#define _RIGHT_SPACES			256

#define _FIRST_CHAR 32
#define _LAST_CHAR 255

#define _MAX_EFFECT 7

#define _ARRIBA 1
#define _ABAJO 2
#define _IZQUIERDA 3
#define _DERECHA 4
#define _NEUTRO 0

#define _f first
#define _s second

#define _SNAKE_UP		1
#define _SNAKE_DOWN		2
#define _SNAKE_RIGHT	4
#define _SNAKE_LEFT		8
#define _SNAKE_BA		16
#define _SNAKE_BB		32

#define _SNAKE_EAT		1
#define _SNAKE_LOSE		2
#define _SNAKE_NOTHING	3
#define _SNAKE_ERROR	4
#define _SNAKE_START	5

#define _LED_RED		0
#define _LED_BLUE		2
#define _LED_GREEN		1

bool debounce(int pin);

extern bool pinSetted;
extern int pinToDebug;

extern volatile unsigned long last_micros[15];

#ifdef _DEBUG_USER
#define _DEBUG_INIT(x)\
		do{Serial.begin(x);}while(false)
#define _DEBUG_PRINT(x)\
		do{\
			Serial.print(#x);\
			Serial.print(": ");\
			Serial.println(x);\
		}while(false)
#define _DEBUG_PRINT_2(x, y)\
		do{\
			Serial.print(#x);\
			Serial.print(": ");\
			Serial.println(y);\
		}while(false)
#define _DEBUG_PRINT_3(x)\
		do{\
			Serial.println(#x);\
		}while(false)
#ifdef _USE_STOP
#define _PIN_TO_DEBUG(pin)\
		do{\
			pinSetted = true;\
			pinToDebug = pin;\
			pinMode(pinToDebug, INPUT);\
		}while(false)

#define _STOP\
	do{\
		if(pinSetted)\
		{\
			while(!debounce(pinToDebug));\
		}\
	}while(false)
#else
#define _PIN_TO_DEBUG(pin)\
		do{}while(false)
#define _STOP\
		do{}while(false)
#endif
#else
#define _DEBUG_PRINT_2(x, y)\
		do{}while(false)
#define _DEBUG_PRINT_3(x)\
		do{}while(false)
#define _DEBUG_PRINT(x)\
		do{}while(false)
#define _DEBUG_INIT(x)\
		do{}while(false)
#define _PIN_TO_DEBUG(pin)\
		do{}while(false)
#define _STOP\
		do{}while(false)
#endif

template <typename T>
T& caster(T&& t)
{
	return t;
}
#define debouncing_time 250L

#endif //_MACROS_H_
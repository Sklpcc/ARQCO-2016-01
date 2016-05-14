
#ifndef _MACROS_H_
#define _MACROS_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define _RED					0
#define _GREEN				1
#define _BLUE				2

#define _RAINBOW 1
#define _WORD_BY_WORD 2
#define _ONE_COLOR 3

#define _FIRST_CYCLE			8		//for first bit by each row (1 cycle * 8 rows)
#define _SECOND_CYCLE		24		//for second bit (2 cycle * 8 rows) + _FIRST_CYCLE
#define _THIRD_CYCLE			56		//for third bit (4 cycles * 8 rows) + _SECOND_CYCLE
#define _FOURTH_CYCLE		120		//for four bit (8 cycles * 8 rows) + _SECOND_CYCLE

#define _MATRICES			1
#define _ROWS				8
#define _COLUMNS				(_MATRICES << 3) //leds per row

#define _BCM_RESOLUTION		4
#define _MAX_VALUE_BCM		15
#define _SPEED				0x25
#define _DELAY				1000

#define _CLOCK_SPEED_DIV_2	8000000

#define _USER_FRIENDLY
#define _USES_FRIENDLY

#define _MAX_BUFFER_SIZE		1300
#define _MAX_COLOR_BUFFER_SIZE		150

#define _RIGHT_TO_LEFT		1
#define _LEFT_TO_RIGHT		2
#define _TOP_TO_BOTTOM		4
#define _BOTTOM_TO_TOP		8

#define _DEBUG

#ifdef _DEBUG
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
#else
#define _DEBUG_PRINT_2(x)\
		do{}while(false)

#define _DEBUG_PRINT(x)\
		do{}while(false)
#define _DEBUG_INIT(x)\
		do{}while(false)
#endif
template <typename T>
T& caster(T&& t)
{
	return t;
}

#define _FIRST_CHAR 32
#define _LAST_CHAR 255

#endif //_MACROS_H_
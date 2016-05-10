#pragma once
#ifndef _RGB_H_
#define _RGB_H_
#include <Arduino.h>
#include "macros.h"
struct RGB
{
	byte red, green, blue; //waste of memory, but this allows to handle higher (max 8) BCM resolutions without any change

	RGB();
	RGB(byte red, byte green, byte blue);
	RGB(byte all);
	RGB(const RGB &color);
	byte getComponent(byte pos);
};
#endif // _RGB_H_
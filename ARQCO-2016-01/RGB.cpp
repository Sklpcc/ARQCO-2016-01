
#include "RGB.h"

RGB::RGB()
{
#ifdef _USER_FRIENDLY
	red = green = blue = _MAX_VALUE_BCM;
#else
	red = green = blue = 0;
#endif
}

RGB::RGB(byte red, byte green, byte blue)
{
#ifdef _USER_FRIENDLY
		red = _MAX_VALUE_BCM - red;
		green = _MAX_VALUE_BCM - green;
		blue = _MAX_VALUE_BCM - blue;
#endif
	this->red = constrain(red, 0, _MAX_VALUE_BCM);
	this->green = constrain(green, 0, _MAX_VALUE_BCM);
	this->blue = constrain(blue, 0, _MAX_VALUE_BCM);
}

RGB::RGB(byte all)
{
#ifdef _USER_FRIENDLY
	all = _MAX_VALUE_BCM - all;
#endif
	all = constrain(all, 0, _MAX_VALUE_BCM);
	this->red = all;
	this->green = all;
	this->blue = all;
}

byte RGB::getComponent(byte pos)
{
	pos = constrain(pos, 0, 2);
	if (pos == 0)
		return red;
	if (pos == 1)
		return green;
	return blue;
}

RGB::RGB(const RGB &color)
{
	this->red = color.red;
	this->green = color.green;
	this->blue = color.blue;
}
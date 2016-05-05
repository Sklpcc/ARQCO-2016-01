
#include "RGB.h"

RGB::RGB()
{
	red = green = blue = 0;
}

RGB::RGB(byte red, byte green, byte blue, bool friendly)
{
	if (friendly)
	{
		red = _MAX_VALUE - red;
		green = _MAX_VALUE - green;
		blue = _MAX_VALUE - blue;
	}
	this->red = constrain(red, 0, _MAX_VALUE);
	this->green = constrain(green, 0, _MAX_VALUE);
	this->blue = constrain(blue, 0, _MAX_VALUE);
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
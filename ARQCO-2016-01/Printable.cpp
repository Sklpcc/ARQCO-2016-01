// 
// 
// 

#include "Printable.h"

Printable2::Printable2(int h, int w, int k, int x, int y)
{
	this->h = h;
	this->w = w;
	this->k = k;
	this->x = x;
	this->y = y;
}

bool Printable2::draw(Matrix &M, int k)
{
	return false;
}

bool Printable2::overlay()
{
	return false;
}

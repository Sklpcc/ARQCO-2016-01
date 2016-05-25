// Printable2.h

#ifndef _PRINTABLE_H
#define _PRINTABLE_H

#include "macros.h"
#include "matriz.h"

class Printable2
{
public:
	Printable2(int h, int w, int k = 1, int x = 0, int y = 0);
	virtual bool draw(Matrix &M, int k);
	virtual bool overlay();
protected:
	int x, y, k, w, h;
};

#endif


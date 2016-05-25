// TetrisPiece.h

#ifndef _TETRISPIECE_h
#define _TETRISPIECE_h

#include "macros.h"
#include "matriz.h"
#include "Printable.h"

const byte pieces[7][4 + 2] = { {0x02, 0x02, 0x03, 0x03, 0x00, 0x00},
								{0x02, 0x03, 0x01, 0x03, 0x01, 0x00},
								{0x03, 0x02, 0x06, 0x03, 0x00, 0x00},
								{0x03, 0x02, 0x07, 0x01, 0x00, 0x00},
								{0x04, 0x01, 0x0F, 0x00, 0x00, 0x00},
								{0x03, 0x02, 0x01, 0x07, 0x00, 0x00},
								{0x03, 0x02, 0x03, 0x06, 0x00, 0x00} };

class TetrisPiece : public Printable2
{
public:
	TetrisPiece();
	bool draw(Matrix &M, int k = 0);
	bool overlay();
	void turn();
	void moveLeft();
	void moveRight();
	void moveDown();
	void fall();
private:
	byte piece[4];
	byte turned;
	byte nPiece;
};

#endif

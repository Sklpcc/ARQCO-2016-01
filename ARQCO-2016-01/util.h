#include <avr/pgmspace.h>

#ifndef _UTIL_H_
#define _UTIL_H_

#include "macros.h"

const byte charmap[] PROGMEM =
{
0x001,0x000,
0x001,0x07D,
0x003,0x0E0,0x000,0x0E0,
0x005,0x024,0x0FF,0x024,0x0FF,0x024,
0x005,0x024,0x052,0x0FF,0x052,0x04C,
0x005,0x062,0x064,0x008,0x013,0x023,
0x005,0x06E,0x099,0x0A5,0x042,0x005,
0x001,0x0E0,
0x003,0x018,0x066,0x081,
0x003,0x081,0x066,0x018,
0x005,0x014,0x008,0x03E,0x008,0x014,
0x005,0x008,0x008,0x03E,0x008,0x008,
0x002,0x005,0x006,
0x005,0x008,0x008,0x008,0x008,0x008,
0x002,0x003,0x003,
0x006,0x003,0x004,0x008,0x010,0x020,0x0C0,
0x005,0x07E,0x081,0x081,0x081,0x07E,
0x005,0x001,0x081,0x0FF,0x001,0x001,
0x005,0x043,0x085,0x089,0x091,0x063,
0x005,0x042,0x081,0x091,0x091,0x06E,
0x006,0x00C,0x014,0x064,0x085,0x0FF,0x005,
0x005,0x002,0x0F1,0x091,0x091,0x08E,
0x005,0x03E,0x051,0x091,0x091,0x08E,
0x005,0x0C0,0x080,0x083,0x09C,0x0E0,
0x005,0x06E,0x091,0x091,0x091,0x06E,
0x005,0x071,0x089,0x089,0x08A,0x07C,
0x002,0x066,0x066,
0x002,0x065,0x066,
0x003,0x008,0x014,0x022,
0x003,0x014,0x014,0x014,
0x003,0x022,0x014,0x008,
0x005,0x040,0x080,0x08D,0x088,0x070,
0x005,0x01E,0x021,0x02F,0x02B,0x01E,
0x007,0x001,0x007,0x0BD,0x0C4,0x03D,0x007,0x001,
0x006,0x081,0x0FF,0x091,0x091,0x091,0x06E,
0x005,0x07E,0x081,0x081,0x081,0x0C2,
0x006,0x081,0x0FF,0x081,0x081,0x042,0x03C,
0x006,0x081,0x0FF,0x091,0x0B9,0x081,0x0C3,
0x006,0x081,0x0FF,0x091,0x0B8,0x080,0x0C0,
0x006,0x07E,0x081,0x081,0x089,0x0CE,0x008,
0x007,0x081,0x0FF,0x091,0x010,0x091,0x0FF,0x081,
0x005,0x081,0x081,0x0FF,0x081,0x081,
0x005,0x006,0x081,0x081,0x0FE,0x080,
0x007,0x081,0x0FF,0x089,0x018,0x0A4,0x0C3,0x081,
0x005,0x081,0x0FF,0x081,0x001,0x007,
0x007,0x081,0x0FF,0x0E1,0x010,0x0E1,0x0FF,0x081,
0x007,0x081,0x0FF,0x0E1,0x01C,0x083,0x0FF,0x080,
0x005,0x07E,0x081,0x081,0x081,0x07E,
0x005,0x081,0x0FF,0x089,0x088,0x070,
0x005,0x07C,0x082,0x083,0x083,0x07D,
0x007,0x081,0x0FF,0x089,0x088,0x08C,0x072,0x001,
0x005,0x063,0x092,0x091,0x051,0x0CE,
0x007,0x0C0,0x080,0x081,0x0FF,0x081,0x080,0x0C0,
0x007,0x080,0x0FE,0x081,0x001,0x081,0x0FE,0x080,
0x007,0x080,0x0E0,0x09C,0x003,0x09C,0x0E0,0x080,
0x007,0x080,0x0FE,0x081,0x01E,0x081,0x0FE,0x080,
0x007,0x081,0x0C3,0x024,0x018,0x024,0x0C3,0x081,
0x007,0x080,0x0C0,0x0B1,0x00F,0x0B1,0x0C0,0x080,
0x005,0x0C3,0x085,0x099,0x0A1,0x0C3,
0x003,0x0FF,0x081,0x081,
0x006,0x0C0,0x020,0x010,0x008,0x004,0x003,
0x003,0x081,0x081,0x0FF,
0x005,0x010,0x020,0x040,0x020,0x010,
0x005,0x001,0x001,0x001,0x001,0x001,
0x002,0x0C0,0x020,
0x006,0x016,0x029,0x029,0x029,0x01F,0x001,
0x006,0x081,0x0FF,0x011,0x021,0x021,0x01E,
0x005,0x01E,0x021,0x021,0x021,0x032,
0x006,0x01E,0x021,0x021,0x091,0x0FF,0x001,
0x005,0x01E,0x029,0x029,0x029,0x019,
0x005,0x021,0x07F,0x0A1,0x0A1,0x0A1,
0x006,0x018,0x025,0x025,0x015,0x03E,0x020,
0x007,0x081,0x0FF,0x011,0x020,0x021,0x01F,0x001,
0x005,0x021,0x021,0x0BF,0x001,0x001,
0x004,0x021,0x021,0x0A1,0x03E,
0x006,0x081,0x0FF,0x008,0x02D,0x033,0x021,
0x005,0x001,0x081,0x0FF,0x001,0x001,
0x007,0x021,0x03F,0x021,0x01F,0x021,0x01F,0x001,
0x007,0x021,0x03F,0x011,0x020,0x021,0x01F,0x001,
0x005,0x01E,0x021,0x021,0x021,0x01E,
0x006,0x021,0x03F,0x015,0x024,0x024,0x018,
0x006,0x018,0x024,0x024,0x015,0x03F,0x021,
0x005,0x021,0x03F,0x011,0x021,0x021,
0x005,0x013,0x029,0x029,0x029,0x036,
0x006,0x020,0x07E,0x021,0x021,0x021,0x002,
0x007,0x020,0x03E,0x001,0x001,0x022,0x03F,0x001,
0x007,0x020,0x038,0x026,0x001,0x026,0x038,0x020,
0x007,0x020,0x03E,0x021,0x00E,0x021,0x03E,0x020,
0x006,0x021,0x033,0x00C,0x00C,0x033,0x021,
0x007,0x020,0x031,0x029,0x007,0x02D,0x030,0x020,
0x005,0x033,0x025,0x029,0x031,0x023,
0x004,0x008,0x036,0x041,0x041,
0x001,0x0FF,
0x004,0x041,0x041,0x036,0x008,
0x006,0x008,0x010,0x010,0x008,0x008,0x010,
0x003,0x01F,0x011,0x01F,
0x008,0x024,0x03C,0x066,0x0A5,0x0A5,0x081,0x042,
0x007,0x018,0x03C,0x05E,0x0B5,0x04E,0x03C,0x018,
0x002,0x001,0x006,
0x005,0x001,0x011,0x07F,0x090,0x080,
0x004,0x001,0x006,0x001,0x006,
0x008,0x003,0x003,0x000,0x003,0x003,0x000,0x003,0x003,
0x005,0x020,0x020,0x0FF,0x020,0x020,
0x005,0x024,0x024,0x0FF,0x024,0x024,
0x003,0x020,0x040,0x020,
0x008,0x062,0x064,0x008,0x013,0x023,0x000,0x003,0x003,
0x005,0x012,0x0A9,0x069,0x0A9,0x026,
0x002,0x008,0x014,
0x006,0x03C,0x042,0x081,0x0FF,0x091,0x081,
0x007,0x018,0x03C,0x05E,0x0B5,0x04E,0x03C,0x018,
0x004,0x023,0x0A5,0x069,0x0B1,
0x007,0x018,0x03C,0x05E,0x0B5,0x04E,0x03C,0x018,
0x007,0x018,0x03C,0x05E,0x0B5,0x04E,0x03C,0x018,
0x002,0x060,0x080,
0x002,0x020,0x0C0,
0x004,0x060,0x080,0x060,0x080,
0x004,0x020,0x0C0,0x020,0x0C0,
0x002,0x018,0x018,
0x004,0x018,0x018,0x018,0x018,
0x008,0x018,0x018,0x018,0x018,0x018,0x018,0x018,0x018,
0x004,0x008,0x010,0x008,0x010,
0x008,0x040,0x070,0x040,0x070,0x040,0x020,0x040,0x070,
0x004,0x012,0x0A9,0x069,0x0A6,
0x002,0x014,0x008,
0x008,0x000,0x00E,0x011,0x011,0x00E,0x015,0x015,0x009,
0x007,0x018,0x03C,0x05E,0x0B5,0x04E,0x03C,0x018,
0x003,0x053,0x035,0x059,
0x005,0x030,0x089,0x007,0x089,0x030,
0x001,0x000,
0x001,0x05F,
0x004,0x01C,0x022,0x063,0x022,
0x006,0x011,0x07F,0x091,0x091,0x081,0x061,
0x006,0x041,0x03E,0x022,0x022,0x03E,0x041,
0x005,0x0C0,0x02A,0x01F,0x02A,0x0C0,
0x001,0x0E7,
0x005,0x012,0x069,0x0A9,0x0A6,0x098,
0x003,0x080,0x000,0x080,
0x007,0x01C,0x022,0x05D,0x055,0x055,0x022,0x01C,
0x004,0x092,0x0AA,0x0AA,0x072,
0x005,0x008,0x014,0x000,0x008,0x014,
0x004,0x008,0x008,0x008,0x00C,
0x003,0x008,0x008,0x008,
0x007,0x01C,0x022,0x05D,0x059,0x055,0x022,0x01C,
0x003,0x040,0x040,0x040,
0x003,0x040,0x0A0,0x040,
0x003,0x00A,0x01E,0x00A,
0x004,0x048,0x098,0x0A8,0x048,
0x003,0x044,0x092,0x06C,
0x002,0x040,0x080,
0x004,0x00F,0x002,0x00C,0x002,
0x005,0x018,0x03C,0x03F,0x020,0x03F,
0x001,0x008,
0x002,0x001,0x002,
0x003,0x050,0x0F0,0x010,
0x003,0x048,0x0A8,0x048,
0x005,0x014,0x008,0x000,0x014,0x008,
0x006,0x050,0x0F3,0x014,0x00A,0x016,0x06F,
0x007,0x050,0x0F3,0x014,0x008,0x011,0x06B,0x005,
0x006,0x050,0x0AB,0x074,0x00A,0x016,0x06F,
0x005,0x00E,0x011,0x0B1,0x001,0x002,
0x007,0x001,0x007,0x09D,0x064,0x01D,0x007,0x001,
0x007,0x001,0x007,0x01D,0x064,0x09D,0x007,0x001,
0x007,0x001,0x007,0x05D,0x0A4,0x05D,0x007,0x001,
0x007,0x001,0x007,0x09D,0x064,0x09D,0x047,0x001,
0x007,0x001,0x007,0x09D,0x024,0x09D,0x007,0x001,
0x007,0x001,0x007,0x01D,0x0A4,0x01D,0x007,0x001,
0x008,0x001,0x007,0x01D,0x064,0x0FF,0x091,0x091,0x081,
0x004,0x078,0x084,0x085,0x086,
0x006,0x021,0x03F,0x0A9,0x069,0x021,0x033,
0x006,0x021,0x03F,0x069,0x0A9,0x021,0x033,
0x006,0x021,0x03F,0x069,0x0A9,0x061,0x033,
0x006,0x021,0x03F,0x0A9,0x029,0x0A1,0x033,
0x005,0x001,0x091,0x05F,0x001,0x001,
0x005,0x001,0x011,0x05F,0x081,0x001,
0x005,0x001,0x051,0x09F,0x041,0x001,
0x005,0x001,0x051,0x01F,0x041,0x001,
0x006,0x089,0x0FF,0x089,0x081,0x042,0x03C,
0x007,0x021,0x03F,0x0B1,0x08C,0x0A3,0x03F,0x020,
0x005,0x01E,0x0A1,0x061,0x021,0x01E,
0x005,0x01E,0x021,0x061,0x0A1,0x01E,
0x005,0x01E,0x061,0x0A1,0x061,0x01E,
0x005,0x01E,0x0A1,0x061,0x0A1,0x05E,
0x005,0x01E,0x0A1,0x021,0x0A1,0x01E,
0x005,0x022,0x014,0x008,0x014,0x022,
0x007,0x001,0x03C,0x046,0x05A,0x062,0x03C,0x080,
0x007,0x020,0x03E,0x0A1,0x041,0x021,0x03E,0x020,
0x007,0x020,0x03E,0x021,0x041,0x0A1,0x03E,0x020,
0x007,0x020,0x03E,0x061,0x081,0x061,0x03E,0x020,
0x007,0x020,0x0BE,0x021,0x001,0x021,0x0BE,0x020,
0x005,0x030,0x009,0x047,0x089,0x030,
0x003,0x03E,0x014,0x008,
0x005,0x07F,0x080,0x099,0x069,0x006,
0x006,0x00A,0x095,0x055,0x015,0x00F,0x001,
0x006,0x00A,0x015,0x055,0x095,0x00F,0x001,
0x006,0x00A,0x055,0x095,0x055,0x00F,0x001,
0x006,0x00A,0x095,0x055,0x095,0x04F,0x001,
0x006,0x00A,0x055,0x015,0x015,0x04F,0x001,
0x006,0x00A,0x015,0x055,0x015,0x00F,0x001,
0x008,0x016,0x029,0x029,0x029,0x01E,0x029,0x029,0x029,
0x003,0x038,0x045,0x046,
0x005,0x00E,0x095,0x055,0x015,0x00D,
0x005,0x00E,0x015,0x055,0x095,0x00D,
0x005,0x00E,0x055,0x095,0x055,0x00D,
0x005,0x00E,0x055,0x015,0x055,0x00D,
0x002,0x040,0x02F,
0x002,0x02F,0x040,
0x003,0x020,0x04F,0x020,
0x003,0x020,0x00F,0x020,
0x004,0x046,0x049,0x039,0x01E,
0x007,0x021,0x03F,0x091,0x0A0,0x0A1,0x01F,0x001,
0x005,0x00E,0x091,0x051,0x011,0x00E,
0x005,0x00E,0x011,0x051,0x091,0x00E,
0x005,0x00E,0x051,0x091,0x051,0x00E,
0x005,0x00E,0x051,0x091,0x051,0x08E,
0x005,0x00E,0x051,0x011,0x051,0x00E,
0x005,0x008,0x008,0x02A,0x008,0x008,
0x007,0x001,0x01C,0x026,0x02A,0x032,0x01C,0x040,
0x007,0x010,0x01E,0x081,0x041,0x012,0x01F,0x001,
0x007,0x010,0x01E,0x001,0x041,0x092,0x01F,0x001,
0x007,0x010,0x01E,0x041,0x081,0x052,0x01F,0x001,
0x007,0x020,0x0BE,0x001,0x001,0x022,0x0BF,0x001,
0x005,0x011,0x009,0x026,0x044,0x018,
0x006,0x0FF,0x024,0x042,0x042,0x024,0x018,
0x005,0x011,0x049,0x006,0x044,0x018
};

//entero 2bytes
const uint16_t char_index[] PROGMEM =
{
0x000,0x002,0x004,0x008,0x00E,0x014,0x01A,
0x020,0x022,0x026,0x02A,0x030,0x036,0x039,
0x03F,0x042,0x049,0x04F,0x055,0x05B,0x061,
0x068,0x06E,0x074,0x07A,0x080,0x086,0x089,
0x08C,0x090,0x094,0x098,0x09E,0x0A4,0x0AC,
0x0B3,0x0B9,0x0C0,0x0C7,0x0CE,0x0D5,0x0DD,
0x0E3,0x0E9,0x0F1,0x0F7,0x0FF,0x107,0x10D,
0x113,0x119,0x121,0x127,0x12F,0x137,0x13F,
0x147,0x14F,0x157,0x15D,0x161,0x168,0x16C,
0x172,0x178,0x17B,0x182,0x189,0x18F,0x196,
0x19C,0x1A2,0x1A9,0x1B1,0x1B7,0x1BC,0x1C3,
0x1C9,0x1D1,0x1D9,0x1DF,0x1E6,0x1ED,0x1F3,
0x1F9,0x200,0x208,0x210,0x218,0x21F,0x227,
0x22D,0x232,0x234,0x239,0x240,0x244,0x24C,
0x254,0x257,0x25D,0x262,0x26B,0x271,0x277,
0x27B,0x284,0x28A,0x28D,0x294,0x29C,0x2A1,
0x2A9,0x2B1,0x2B4,0x2B7,0x2BC,0x2C1,0x2C4,
0x2C9,0x2D2,0x2D7,0x2E0,0x2E5,0x2E8,0x2F1,
0x2F9,0x2FD,0x303,0x305,0x307,0x30C,0x313,
0x31A,0x320,0x322,0x328,0x32C,0x334,0x339,
0x33F,0x344,0x348,0x350,0x354,0x358,0x35C,
0x361,0x365,0x368,0x36D,0x373,0x375,0x378,
0x37C,0x380,0x386,0x38D,0x395,0x39C,0x3A2,
0x3AA,0x3B2,0x3BA,0x3C2,0x3CA,0x3D2,0x3DB,
0x3E0,0x3E7,0x3EE,0x3F5,0x3FC,0x402,0x408,
0x40E,0x414,0x41B,0x423,0x429,0x42F,0x435,
0x43B,0x441,0x447,0x44F,0x457,0x45F,0x467,
0x46F,0x475,0x479,0x47F,0x486,0x48D,0x494,
0x49B,0x4A2,0x4A9,0x4B2,0x4B6,0x4BC,0x4C2,
0x4C8,0x4CE,0x4D1,0x4D4,0x4D8,0x4DC,0x4E1,
0x4E9,0x4EF,0x4F5,0x4FB,0x501,0x507,0x50D,
0x515,0x51D,0x525,0x52D,0x535,0x53B,0x542
};

/*
"ARQUITECTURA DE COMPUTADORAS 2016-01. Proyecto Final del Curso. Profesor: Ing. Bady Elder Cruz Días... los que vamos a vivir te saludan." //136 caracteres
"ARQUITECTURADECOMPUTADORAS2016-01.ProyectoFinaldelCurso.Profesor:Ing.BadyElderCruzDías...losquevamosavivirtesaludan."
*/

struct character_handler
{
	uint16_t pos;
	byte size;
	character_handler()
	{
		size = B11111111;
	}
	character_handler(uint16_t pos, byte size) : pos(pos), size(size) {};
	character_handler(byte character)
	{
		character = constrain(character, _FIRST_CHAR, _LAST_CHAR);
		character -= _FIRST_CHAR;
		pos = pgm_read_word_near(char_index + character);
		size = pgm_read_byte_near(charmap + pos);
		pos += 1;
	}
	byte getByte(byte bytePos)
	{
		if (size == B11111111)
			return size;
		bytePos = constrain(bytePos, 0, size - 1);
		return pgm_read_byte_near(charmap + pos + bytePos);
	}
};

#endif //_UTL_H_
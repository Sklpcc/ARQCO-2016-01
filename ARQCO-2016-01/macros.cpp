
#include "macros.h"

bool debounce(int pin)
{
	int contador = 0;
	bool estado, estadoAnterior;
	do
	{
		estado = digitalRead(pin);
		if (estado != estadoAnterior)
		{
			contador = 0;
			estadoAnterior = estado;
		}
		else
		{
			contador += 1;
		}
		delay(1);
	} while (contador < 10);
	return estado;
}


bool pinSetted = false;
int pinToDebug;
volatile unsigned long last_micros[15];
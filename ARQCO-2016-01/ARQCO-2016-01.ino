const int filas[8] = {2,3,4,5,6,7,8,9};
const int columnas[8] = {14,15,16,17,18,19,20,21};

byte caracter[8] = {0,12,20,100,133,255,5,0};

void setup()
{
	for(int i = 0; i < 8; ++i)
	{
		pinMode(filas[i], OUTPUT);
		pinMode(columnas[i], OUTPUT);
	}
	for(int i = 0; i < 8; ++i)
	{
		digitalWrite(filas[i], LOW);
		digitalWrite(columnas[i], HIGH);
	}
}

void loop()
{
	muestraPantalla();
}

void muestraPantalla()
{
	for(int columna = 0, temp; columna < 8; ++columna)
	{
		for(int i = 0; i < 8; ++i)
			digitalWrite(filas[i], LOW);
			
		for(int j = 0; j < 8; ++j)
			digitalWrite(columnas[j], j != columna);

		for(int fila = 0; fila < 8; ++fila)
			if(((caracter[columna] >> fila) & 1) == 1)
				digitalWrite(filas[fila], HIGH);
	}
}


#include <WString.h>
class Matriz
{
public:
	Matriz();
	~Matriz();
	void agregarCaracter(char c, char rgb[]);
	void desplazar(char direccion, char modo);
	bool mostrarTexto(String s, char efecto);
	void print();
private:
	const int _filas = 8;
	const int _columnas = 64;
	int uc;
	char matriz[64][3];
};

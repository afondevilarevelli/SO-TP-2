#ifndef CONVERSOR_H_
#define CONVERSOR_H_


#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char* intToChar(int num);
int charToInt(char* ch);
int cantidadDeDigitos(int num);

int convertirAInt(char* pal);
char* convertirAString(int num, int cantidadDeDigitos);

#endif 
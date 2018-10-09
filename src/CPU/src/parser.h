#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/string.h> 
#include "libCPU.h"

typedef struct{   
    typePalabraReservada palabraReservada; //concentrar
    char* p1; //abrir, wait, signal, flush, close, borrar
    char* p2; //crear
    char* p3; //asignar
}t_operacion;

//EJEMPLOS DE SENTENCIAS
/*
-abrir /equipos/Racing.txt
-concentrar
-asignar /equipos/Racing.txt 9 GustavoBou
-wait Conmebol
-signal Conmebol
-flush /equipos/Racing.txt
-close /equipos/Racing.txt
-crear /equipos/Racing.txt 11
-borrar /equipos/Racing.txt
*/

//FUNCIONES
t_operacion parse(char* line);

#endif
#include "parser.h"

t_operacion parse(char* line){
    t_operacion retorno;

	if(line == NULL || string_equals_ignore_case(line, "")){
		retorno.palabraReservada = BLANCO;
        retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
		return retorno;
	}

	char* auxLine = string_duplicate(line);
	string_trim(&auxLine);
	char** split = string_n_split(auxLine, 4, " ");

	char* palReservada = split[0];
	// split[1] = parametro1
    // split[2] = parametro2
    // split[3] = parametro3		
	
	if(string_equals_ignore_case(palReservada, "abrir")){
		retorno.palabraReservada = ABRIR;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "concentrar")){
		retorno.palabraReservada = CONCENTRAR;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "asignar")){
		retorno.palabraReservada = ASIGNAR;
		retorno.p1 = split[1];
        retorno.p2 = split[2];
        retorno.p3 = split[3];
	} else if(string_equals_ignore_case(palReservada, "wait")){
		retorno.palabraReservada = WAIT;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "signal")){
		retorno.palabraReservada = SIGNAL;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "flush")){
		retorno.palabraReservada = FLUSH;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "close")){
		retorno.palabraReservada = CLOSE;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "crear")){
		retorno.palabraReservada = CREAR;
		retorno.p1 = split[1];
        retorno.p2 = split[2];
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "borrar")){
		retorno.palabraReservada = BORRAR;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "#")){
		retorno.palabraReservada = NUMERAL;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else{
        fprintf(stderr, "No se encontro la palabra reservada <%s>\n", palReservada);
		return;
    }

	free(auxLine);
	return retorno;
}
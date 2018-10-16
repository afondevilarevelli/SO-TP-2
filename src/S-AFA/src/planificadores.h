#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include "libSAFA.h"
#include <stdlib.h>
#include <stdio.h>

void planificadorLargoPlazo();
void planificarSegunRR(CPU* cpu);
void planificarSegunVRR(CPU* cpu);
void planificarSegunAlgoritmoPropio(CPU* cpu);
DTB* obtenerDTBAEjecutarSegunRR();

void  identificarProceso(socket_connection * connection ,char** args);


#endif

#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include "libSAFA.h"

void planificadorLargoPlazo();
void planificarSegunRR(int quantum);
void planificarSegunVRR(int quantum);
DTB* obtenerDTBAEjecutarSegunRR();

#endif
#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include </usr/include/sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "../../sample-socket/socket.h"

// devuelve un true(1) si el archivo existe, u falso(0)  si no existe
void  validarArchivo(socket_connection * , char **);
//crear el archivo, cada elemento del vector sera una linea del archivo
void crearArchivo(char * ,size_t * );
//devuelvo la cant de bytes del path(archivo)
size_t   obtenerDatos(char * path,off_t * ,size_t *  );
void guardarDatos(char * ,off_t  *,size_t * ,char *);
void borrarArchivo(char *);

#endif
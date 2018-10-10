#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "../../sample-socket/socket.h"

//uso enum para ser mas claro en el codigo
// noExiste = 0, existe =1
// yaCreado = 0,recienCreado = 1, noCreado = 2
//el caso que sea 2 es para casos de error
 
typedef enum{noExiste,existe};
typedef enum{yaCreado,recienCreado,noCreado};
typedef enum{noBorrado,recienBorrado};

// devuelve un true(1) si el archivo existe, u falso(0)  si no existe
void  validarArchivo(socket_connection * ,char* path);
//crear el archivo, cada elemento del vector sera una linea del archivo
void crearArchivo(socket_connection *,char * ,size_t * );
//devuelvo la cant de bytes del path(archivo)
size_t   obtenerDatos(socket_connection *,char * path,off_t * ,size_t *  );
void guardarDatos(socket_connection *,char * ,off_t  *,size_t * ,char *);
void borrarArchivo(socket_connection *,char *);
int verificarSiExisteArchivo(char * path);
#endif
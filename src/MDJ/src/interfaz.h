#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../sample-socket/socket.h"
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <sys/file.h>

//uso enum para ser mas claro en el codigo
// noExiste = -1, existe =0
// yaCreado = 0,recienCreado = 1, noCreado = 2
// no borrado = -1; recienBorrado = 0
//el caso que sea 2 es para casos de error
 enum{noExiste = -1,existe = 0 };
 enum{yaCreado = 0,recienCreado = 1,errorCreado = -1}; //para DAM
 enum{recienBorrado = 0,noBorrado = -1,errorBorrado = -2};

//estructura por si hay que usar hilos
typedef struct {
socket_connection * connection;
char ** args;
}argumentos;

typedef struct {
t_list *  bloqLibres;
t_list * bloqOcupados;
char ** bloqArchivo;
int bloques;
}t_bloques;



//estructura para guardar la informacion del archivo
typedef struct{
int fd;
char * path;
int * bloques;//vector con todos los bloques
size_t  size; 
int estado;
}t_archivo;


//estructura del metadata 
typedef struct {
size_t tamanio_bloques;
int cantidad_bloques;
char * magic_number;
}t_metadata_filesystem;

typedef struct{
t_bitarray * bitarray;//  1 ocupado , 0 libre  
}t_metadata_bitmap;

typedef struct{
int fd;
size_t  tamanio_archivo_enBytes;
int  * bloques;
}t_metadata_filemetadata;

typedef struct {
	int puerto;
	char * ptoMontaje;
	int  retardo;
} t_config_MDJ;

int TAM_BLOQUE;
int CANT_BLOQUES;

t_config_MDJ * datosConfMDJ;
t_metadata_filesystem * fs;
char * directorioMontaje;
void aplicarRetardo();
// devuelve un true(1) si el archivo existe, u falso(0)  si no existe
void  validarArchivo(socket_connection * ,char**);
//crear el archivo, cada elemento del vector sera una linea del archivo
void crearArchivo(socket_connection *,char **);
//devuelvo la cant de bytes del path(archivo)
void  obtenerDatos(socket_connection *,char **);
void guardarDatos(socket_connection *,char **);
void borrarArchivo(socket_connection *,char **);
t_list* _list_duplicate(t_list* self);
int verificarSiExisteArchivo(char * );
int * crearBloques(int, char * ,size_t);
char * obtenerPtoMontaje();
t_bitarray * crearBitmap(int);
t_bloques * asignarBloques(t_list * , t_list *,size_t);
char ** obtenerBloques(char *);
int cantElementos2(char **);
int cantElementos1(char *);
void liberarLista(t_list * );
char * obtenerDatosBloque(int);
int is_regular_file(const char *);
void escribirBloque(int bloque, int offset, int length, char * buffer);
int obtenerBloqueInicial(char * path,off_t offset);
char ** obtenerBloquesDesdeConsola(char * path);
#endif
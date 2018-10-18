#include  <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../../sample-socket/socket.h"
#include "consolaMDJ.h"
#include "interfaz.h"



//ESTRUCTURAS
typedef struct {
	int puerto;
	char *  ip;
	char * ptoMontaje;
	int  retardo;
} t_config_MDJ;

typedef struct {
t_list *  directorios;
}t_directorios;

typedef struct{
int fd;
char * mem_ptro;
char * path;
}

typedef struct {
size_t tamanio_bloques;
int cantidad_bloques;
char * magic_number;
}t_metadata_filesystem;

typedef struct{
t_bitarray estado_bloques; //  1 ocupado , 0 libre
}t_metadata_bitmap;

typedef struct{
size_t  tamanio_archivo_enBytes;
int * bloques;
}t_metadata_filemetadata;



//VARIABLES
t_log* logger;
t_dictionary *  fns;	/* Funciones de socket */
pthread_mutex_t mx_main;	/* Semaforo de main */
int portServer;
char * directorioMontaje;
t_config * conf;
t_config_MDJ  *  datosConfMDJ;



//FUNCIONES
void configure_logger();
void close_logger();
void cerrarPrograma();
void validarExistencia(socket_connection * connection, char ** args);



//CONFIG

t_config_MDJ *  read_and_log_config(char*);

//SOCKETS
void disconnect(socket_connection *);
void identificarProceso(socket_connection * connection ,char** args);

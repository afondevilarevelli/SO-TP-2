#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "socket.h"

//ESTRUCTURAS
typedef struct {
	int puerto;
	char* modo;
	int tamanio;
	int maximoLinea;
	int tamanioPagina;
} t_config_FM9;

typedef struct {
	int base;
	int limite;
} t_tabla_segmentos;

//VARIABLES
t_log* logger;
t_config_FM9* datosConfigFM9;
t_dictionary * callableRemoteFunctions;
pthread_mutex_t mx_main;

void* memoria;

t_list* lista_tabla_segmentos;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//SOCKETS
void identificarProceso(socket_connection * connection ,char** args);
void disconnect(socket_connection*);
void DAM_FM9_guardarGDT(socket_connection * connection ,char** args);

//SEGMENTACION PURA
void inicializarMemoriaConSegmentacion();
int devolverPosicionNuevoSegmento(int);

//CONFIG
t_config_FM9* read_and_log_config(char*);


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
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

//VARIABLES
t_log* logger;
t_config_FM9* datosConfigFM9;
t_dictionary * callableRemoteFunctions;
pthread_mutex_t mx_main;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//SOCKETS
void connectionNew(socket_connection*);
void disconnect(socket_connection*);
void DAM_FM9_handshake(socket_connection *, char **);

//CONFIG
t_config_FM9* read_and_log_config(char*);

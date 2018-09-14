#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>
#include "../../sample-socket/socket.h"


//ESTRUCTURAS
typedef struct {
	int puerto;
	char* algoritmoPlanif;
	int quantum;
	int gradoMultiprog;
	int retardo;
} t_config_SAFA;

//VARIABLES GLOBALES
t_log* logger;
t_config_SAFA* datosConfigSAFA;
pthread_mutex_t mx_main;
t_dictionary* fns;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//CONFIG
void read_and_log_config(char*);

//CallableRemoteFunctions
void DAM_SAFA_handshake(socket_connection* socketInfo, char** msg);
void CPU_SAFA_handshake(socket_connection* socketInfo, char** msg);



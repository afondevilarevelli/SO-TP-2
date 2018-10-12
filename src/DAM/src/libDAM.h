#ifndef libDAM_H_
#define libDAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include "../../sample-socket/socket.h"


//ESTRUCTURAS
typedef struct {
	int puertoEscucha;
	char* IPSAFA;
	int puertoSAFA;
    char* IPMDJ;
    int puertoMDJ;
    char* IPFM9;
    int puertoFM9;
    int transferSize;
} t_config_DAM;

//----------------------------//

//VARIABLES
t_log* logger;
t_config_DAM* _datosDAM;
t_dictionary* callableRemoteFunctionsSAFA;
t_dictionary* callableRemoteFunctionsFM9;
t_dictionary* callableRemoteFunctionsMDJ;
t_dictionary* callableRemoteFunctionsCPU;
pthread_mutex_t mx_main;	/* Semaforo de main */
t_config_DAM*  datosConfigDAM;


//----------------------------//


//LOGS
void configure_logger();
t_config_DAM* read_and_log_config(char*);
void close_logger();

//----------------------------//


//PROTOTIPOS
void cerrarPrograma();
void elementoDestructorDiccionario(void *);
void existeArchivo(socket_connection* , char * );
void verificarArchivoCreado(socket_connection* , char *);
void verificameSiArchivoFueBorrado(socket_connection*,char *);

//diccionarios
void identificarProceso(socket_connection * connection ,char** args);
void MDJ_DAM_existeArchivo(socket_connection*,char ** );
void MDJ_DAM_verificarArchivoCreado(socket_connection*,char **);
void MDJ_DAM_verificameSiArchivoFueBorrado(socket_connection*,char **);

#endif

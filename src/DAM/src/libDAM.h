#ifndef libDAM_H_
#define libDAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
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

typedef struct{
    char idGDT[2];
    char path[40]; 
    char dummy[1];
    char* socketCPU;
}parametrosCarga;

typedef struct{
    char idGDT[2];
    char* path;
    char pagina[3]; 
    char segmento[4];
    char desplazamiento[4];
    char* socketCPU;
    char cantLineas[2];
}parametrosFlush;
//----------------------------//

//VARIABLES
t_log* logger;
t_dictionary* callableRemoteFunctions;
pthread_mutex_t mx_main;	/* Semaforo de main */
t_config_DAM*  datosConfigDAM;

pthread_mutex_t m_pedido;

int socketSAFA;
int socketFM9;
int socketMDJ;

char* ruta;
char* baseSegmento;
char* pagina;
char* desplazamiento;
char* cantidadDeLineas;
int offsetAcumulado;
//----------------------------//


//LOGS
void configure_logger();
t_config_DAM* read_and_log_config(char*);
void close_logger();

//----------------------------//


//PROTOTIPOS
void cerrarPrograma();
void elementoDestructorDiccionario(void *);

void hiloCarga(parametrosCarga*);
void hiloFlush(parametrosFlush* params);

//diccionarios
void identificarProceso(socket_connection * connection ,char** args);
void MDJ_DAM_resultadoCreacionArchivo(socket_connection*,char ** );
void MDJ_DAM_existeArchivo(socket_connection*,char ** );
void MDJ_DAM_resultadoBorradoArchivo(socket_connection*,char **);
void CPU_DAM_existeArchivo(socket_connection* socketMDJ, char ** args);
void CPU_DAM_crearArchivo(socket_connection* connection, char** args);
void CPU_DAM_borrarArchivo(socket_connection* connection, char** args);
void FM9_DAM_archivoCargadoCorrectamente(socket_connection* , char**);
void MDJ_DAM_avisarResultadoDTB(socket_connection* socketInf,char ** args);
void CPU_DAM_solicitudDeFlush(socket_connection* , char** );
void FM9_DAM_respuestaFlush(socket_connection* connection, char** args);
void MDJ_DAM_respuestaFlush(socket_connection* connection, char** args);
void CPU_DAM_solicitudCargaGDT(socket_connection* connection, char ** args);
void MDJ_DAM_respuestaCargaGDT(socket_connection * , char **);
#endif

#ifndef LIB_SAFA_H
#define LIB_SAFA_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <semaphore.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include  "../../sample-socket/socket.h"
# include "../../Utils/gestionArchConf.h"


//ESTRUCTURAS
typedef enum {NEW, READY, BLOCKED, RUNNING, FINISHED } status_t;

typedef struct {
	int puerto;
	char* algoritmoPlanif;
	int quantum;
	int gradoMultiprog;
	int retardo;
} t_config_SAFA;

typedef struct{
	int id;
	char* rutaScript;
	int PC; //program counter
	int flagInicializado;
	t_list* archivosAbiertos;
	status_t status;
}DTB;


//--------------------------------------//



//VARIABLES GLOBALES

//semaforos
sem_t sem_estadoCorrupto;
sem_t cantProcesosEnReady;
sem_t cantProcesosEnNew;
pthread_mutex_t m_colaReady;
pthread_mutex_t m_colaBloqueados;
pthread_mutex_t m_colaNew;

t_log* logger;
t_config_SAFA* datosConfigSAFA;
pthread_mutex_t mx_main;
t_dictionary* fns;

t_queue* colaNew;
t_queue* colaReady;
t_queue* colaBloqueados;
t_queue* colaFinalizados;

t_list* hilos;

//booleanos para manejar el estado corrupto
bool estadoCorrupto; 
bool damConectado;
bool unCpuConectado;

//--------------------------------------------//

//FUNCIONES UTILES
void encolarDTB(t_queue* c, DTB* d, pthread_mutex_t m);
DTB* desencolarDTB(t_queue* c, pthread_mutex_t m);

//LOGS
void configure_logger();
void close_logger();
t_config_SAFA * read_and_log_config(char*);

//--------------------------------------------//


//PROTOTIPOS
void cerrarPrograma();
void identificarDAM( socket_connection* socketInfo, char** msg);

//--------------------------------------------//



//CallableRemoteFunctions
void identificarProceso(socket_connection * connection ,char** args);
void newConnection(socket_connection* socketInfo, char** msg);

//Funciones Para La Funcion De Status
void mostrarInformacionDTB(DTB*);
void* buscarIdGdtAsociado(int);
void* buscarDTBEnColas(int, t_queue*);
static inline char *stringFromState(status_t);
#endif

#ifndef libCPU_H
#define libCPU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include "../../sample-socket/socket.h" 

//ESTRUCTURA
typedef struct {
	char* ipS;
	int puertoS;
	char*  ipD;
	int puertoD;
	char* ipF;
	int puertoF;
	int retardo;
} t_config_CPU;

typedef struct{
	int idGDT;
	char rutaScript[60];
	int programCounter; 
	int flagInicializado;
	int quantumAEjecutar;
	int pagina;
	int segmento;
	int desplazamiento;
	int cantLineas; //del archivo script
}parametros;
//EJEMPLOS DE SENTENCIAS
/*
-abrir /equipos/Racing.txt
-concentrar
-asignar /equipos/Racing.txt 9 GustavoBou
-wait Conmebol
-signal Conmebol
-flush /equipos/Racing.txt
-close /equipos/Racing.txt
-crear /equipos/Racing.txt 11
-borrar /equipos/Racing.txt
*/

int quantum;
int idCPU;

typedef enum{
	ABRIR, //I/O si no se encuentra abierto por el DTB
	CONCENTRAR, 
	ASIGNAR, 
	WAIT, 
	SIGNAL, 
	FLUSH, 
	CLOSE, 
	CREAR, //I/O
	BORRAR, 
	NUMERAL, //para comentarios
	ERROR
}palabraReservada_t;

typedef struct{   
    palabraReservada_t palabraReservada; //concentrar
    char* p1; //abrir, wait, signal, flush, close, borrar
    char* p2; //crear
    char* p3; //asignar
	bool ultimaSentencia;
}operacion_t;

//VARIABLES GLOBALES
t_log* logger;
t_config* archivo_Config;
t_config_CPU* datosCPU;
t_dictionary * callableRemoteFunctionsCPU;

pthread_mutex_t m_main;
pthread_mutex_t m_busqueda;
pthread_mutex_t m_puedeEjecutar;

sem_t sem_esperaAbrir;
sem_t sem_esperaClose;
sem_t sem_esperaEjecucion;
sem_t sem_esperaDatos;

//VAR GLOB SOCKETS
int socketDAM;
int socketSAFA;
int socketFM9;

bool archivoExistente;
bool archivoAbiertoAbrir;
bool archivoAbiertoAsignar;
bool archivoAbiertoFlush;
bool archivoAbiertoClose;
bool resultadoCloseOk;
bool resultadoWaitOk;

char datosPedidos[200];
int ultimaSentencia;

char* paginaArchivo;
char* segmentoArchivo;
char* desplazamientoArchivo;
char* cantLineasArchivo;

//FUNCIONES

void configure_loggerCPU();
t_config_CPU* read_and_log_configCPU(char* path);
void close_loggerCPU();

void* intentandoConexionConSAFA(int );
void* intentandoConexionConDAM(int );
void* intentandoConexionConFM9(int );
void disconnect();

operacion_t obtenerSentenciaParseada(int, int, int, int, int, int);

//PARSER
operacion_t parse(char* line);
void destruirOperacion(operacion_t op);

void permisoDeEjecucion(parametros* parms);
//callable remote functions
void permisoConcedidoParaEjecutar(socket_connection * connection ,char** args); //SAFA
void establecerQuantumYID(socket_connection * connection ,char** args); //SAFA
void pausarPlanificacion(socket_connection* ,char**);
void continuarPlanificacion(socket_connection*,char**);
void ejecucionAbrir(socket_connection*, char**);
void ejecucionAbrirExistencia(socket_connection*, char**);
void ejecucionAsignar(socket_connection*, char**);
void ejecucionClose(socket_connection*, char**);
void ejecucionFlush(socket_connection*, char**);
void ejecucionWait(socket_connection*, char**);
void finalizacionClose(socket_connection*, char**);
void funcionHiloAbrir();
void funcionHiloClose();
void funcionHilo(char*); 
void funcionHiloObtencionDatos();
void resultadoObtencionDatos(socket_connection * connection ,char** args);
void avisarTerminoClock(socket_connection * connection ,char** args);
#endif

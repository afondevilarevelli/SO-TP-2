#ifndef libCPU_H
#define libCPU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include "../../sample-socket/socket.h" 
#include "../../Utils/gestionArchConf.h"
#include "../../Utils/gestionProcesos.h"
#include "parser.h"


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

typedef enum{
	ABRIR, 
	CONCENTRAR, 
	ASIGNAR, 
	WAIT, 
	SIGNAL, 
	FLUSH, 
	CLOSE, 
	CREAR, 
	BORRAR, 
	NUMERAL, //para comentarios
	BLANCO  //para señalar fin de programa
}t_palabraReservada;

//VARIABLES GLOBALES
t_log* logger;
t_config* archivo_Config;
t_config_CPU* datosCPU;
t_dictionary * callableRemoteFunctionsCPU;

//FUNCIONES

void configure_logger();
t_config_CPU* read_and_log_config(char*);
void close_logger();

void* intentandoConexionConSAFA(int* );
void* intentandoConexionConDAM(int* );
void* intentandoConexionConFM9(int* );

void SAFA_CPU_handshake(socket_connection *, char **);
void DAM_CPU_handshake(socket_connection *, char **);
void FM9_CPU_handshake(socket_connection *, char **);

void saliendo_por_error(int , char* , void* );
void disconnect();
void exit_gracefully(int );


#endif

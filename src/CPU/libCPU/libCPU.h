#ifndef libCPU_H
#define libCPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <sys/socket.h>


#define IP INADDR_ANY //MACRO

//ESTRUCTURA
typedef struct {
	char* ipS;
	int puertoS;
	char*  ipD;
	int puertoD;
	int retardo;
} t_config_CPU;

//VARIABLES GLOBALES
t_log* logger;
t_config* archivo_Config;
t_config_CPU* datosCPU;

//FUNCIONES

void configure_logger();
void read_and_log_config(char*);
void saliendo_por_error(int , char* , void* );
void esperando_respuesta(int );
void enviando_mensaje(int , char* );
void exit_gracefully(int );
void close_logger();

#endif

#include  <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include  "../../gestionArchConfig/gestionArchConf.h"



//ESTRUCTURAS
typedef struct {
	int puerto;
	char *  ip;
	char * ptoMontaje;
	int  retardo;
} t_config_MDJ;

//VARIABLES
t_log* logger;


//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//CONFIG

void read_and_log_config(char*);
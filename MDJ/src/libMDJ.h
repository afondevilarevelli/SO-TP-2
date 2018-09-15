#include  <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../sample-socket/socket.h"
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
void configure_logger();
void close_logger();
void DAM_MDJ_handshake(socket_connection * , char ** );
void cerrarPrograma();
void elementoDestructorDiccionario(void *);

//CONFIG

t_config_MDJ *  read_and_log_config(char*);

//SOCKETS

void DAM_MDJ_handshake(socket_connection * , char **) ;
void disconnect(socket_connection *);
void connectionNew(socket_connection* );
char * getIp();

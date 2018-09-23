#include  <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include "../../sample-socket/socket.h"
#include "../../Utils/gestionArchConf.h"
#include   "../../Utils/gestionProcesos.h"
#include "consolaMDJ.h"



//ESTRUCTURAS
typedef struct {
	int puerto;
	char *  ip;
	char * ptoMontaje;
	int  retardo;
} t_config_MDJ;

//VARIABLES
t_log* logger;
t_dictionary *  fns;	/* Funciones de socket */
pthread_mutex_t mx_main;	/* Semaforo de main */
int portServer;
t_config * conf;
t_config_MDJ  *   datosConfMDJ;



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

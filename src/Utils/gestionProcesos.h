#ifndef GESTIONPROCESOS_H_
#define GESTIONPROCESOS__H_



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
#include  "../sample-socket/socket.h"
# include "gestionArchConf.h"





typedef struct {
char *  proceso;
}proc;

typedef enum {NEW, READY, BLOCKED, RUNNING, FINISHED } status_t;

typedef struct{
	int id;
	char* rutaScript;
	int PC; //program counter
	int flagInicializado;
	t_list* archivosAbiertos;
	status_t status;
}DTB;


void identificarProceso(socket_connection * connection ,char** args);
proc * pro;
t_log* logger;


#endif /* GESTIONPROCESOS_H_ */

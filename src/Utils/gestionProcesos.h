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

proc * pro;

char* identificarProceso(t_log* logger, socket_connection * connection ,char** args);

#endif /* GESTIONPROCESOS_H_ */

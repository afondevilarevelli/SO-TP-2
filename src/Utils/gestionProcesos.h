#ifndef GESTIONPROCESOS_H_
#define GESTIONPROCESOS__H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "../sample-socket/socket.h"

typedef struct {
char *  proceso;
}proc;

void identificarProceso(socket_connection * connection ,char** args);
proc * pro;
t_log* logger;


#endif /* GESTIONPROCESOS_H_ */

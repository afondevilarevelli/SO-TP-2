#include <stdio.h>
#include <stdlib.h>
#include "gestionProcesos.h"

void identificarProceso(socket_connection * connection ,char** args)
{
     proc * pro = malloc(sizeof(proc));
     pro->proceso = args[0];
     log_info (logger,"Se conecto %s en el socket NRO %d  con IP %s  PUERTO %d", pro->proceso,connection->socket,connection->ip,connection-> port);
     free(pro);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "consolaSAFA.h"
#include "libSAFA/libSAFA.h"

void connectionNew(socket_connection* socketInfo);
void disconnect(socket_connection* socketInfo);
void elementoDestructorDiccionario();

int main(void){
	fns = dictionary_create();
	dictionary_put(fns, "DAM_SAFA_handshake", &DAM_SAFA_handshake);

	configure_logger();
	read_and_log_config("S-AFA.config");
	close_logger();

	printf("Escuchando puerto 8080");
	createListen(datosConfigSAFA->puerto, &connectionNew, fns, &disconnect, NULL);

  	// bloqueo el thread, dejando la coneccion abierta
 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	dictionary_destroy_and_destroy_elements(fns, &elementoDestructorDiccionario);
    return 0;
}

//FUNCIONES
void connectionNew(socket_connection* socketInfo) {
	printf("Se ha conectado un usuario con ip %s en socket n°%d\n", socketInfo->ip, socketInfo->socket);
}

void disconnect(socket_connection* socketInfo) {
  printf("socket n° %d se ha desconectado.\n", socketInfo->socket);
}

void elementoDestructorDiccionario(void* data){
	free(data);
}


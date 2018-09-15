#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "consolaSAFA.h"
#include "libSAFA.h"

void connectionNew(socket_connection* socketInfo);
void disconnect(socket_connection* socketInfo);
void elementoDestructorDiccionario();

int main(void){
        signal(SIGINT, cerrarPrograma);
	fns = dictionary_create();
   dictionary_put(fns, "DAM_SAFA_handshake", &DAM_SAFA_handshake);
    dictionary_put(fns, "CPU_SAFA_handshake", &CPU_SAFA_handshake);

	configure_logger();
	read_and_log_config("S-AFA.config");
	
	log_info(logger, "Voy a escuchar por mi puerto: %d", datosConfigSAFA->puerto);
	createListen(datosConfigSAFA->puerto, &connectionNew, fns, &disconnect, NULL);
  	// bloqueo el thread, dejando la coneccion abierta
 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

    return EXIT_SUCCESS;
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

void cerrarPrograma() {
    log_info(logger, "Voy a cerrar SAFA");
    close_logger();
     dictionary_destroy(fns); 
     free(datosConfigSAFA);
   // dictionary_destroy_and_destroy_elements(fns);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "consolaSAFA.h"
#include "libSAFA.h"

void connectionNew(socket_connection* socketInfo);
void disconnect(socket_connection* socketInfo);

int main(void){
        //senial para que al cortar el flujo, se libere memoria
        signal(SIGINT, cerrarPrograma);

     // diccionarios para la funcion runFunction....
     fns = dictionary_create();
     dictionary_put(fns, "DAM_SAFA_handshake", &DAM_SAFA_handshake);
     dictionary_put(fns,"identificarProceso",&identificarProceso);
     dictionary_put(fns, "CPU_SAFA_handshake", &CPU_SAFA_handshake);

        colaReady = queue_create();
        colaBloqueados = queue_create();
        colaFinalizados = queue_create();
        colaNew = queue_create();
        hilos = list_create();

        pthread_t hiloPCP, hiloConsola;
        list_add(hilos, &hiloPCP);
        list_add(hilos, &hiloConsola);


        //configuracion de loggers
	configure_logger();
        read_and_log_config("S-AFA.config");
	log_info(logger, "Voy a escuchar por el puerto: %d", datosConfigSAFA->puerto);

        sem_init(&entradaGDT, 0, datosConfigSAFA->gradoMultiprog);
        sem_init(&cantProcesosEnReady, 0, 0);
        pthread_mutex_init(&m_colaReady, NULL);
	pthread_mutex_init(&m_colaBloqueados, NULL);
        pthread_mutex_init(&m_colaNew, NULL);

        pthread_create(&hiloConsola, NULL, (void*)&consolaSAFA, NULL);
        pthread_create(&hiloPCP, NULL, (void*)&planificarSegunRR, &datosConfigSAFA->quantum);
       

          
        //pongo a escuchar el server
	createListen(datosConfigSAFA->puerto, NULL, fns, &disconnect, NULL);


        
  	// bloqueo el thread, dejando la coneccion abierta
 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

        dictionary_destroy_and_destroy_elements(fns, (void*) free);
        pthread_mutex_destroy(&m_colaReady);
        pthread_mutex_destroy(&m_colaBloqueados);
        pthread_mutex_destroy(&m_colaNew);
        
        //cerrar hilos, no se como se hace

    return EXIT_SUCCESS;
}



void disconnect(socket_connection* socketInfo) {
  log_info(logger,"socket n° %d se ha desconectado.\n", socketInfo->socket);
}

void cerrarPrograma() {
    log_info(logger, "Voy a cerrar SAFA");
    close_logger();
    dictionary_destroy_and_destroy_elements(fns, (void*) free); 
    free(datosConfigSAFA);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}
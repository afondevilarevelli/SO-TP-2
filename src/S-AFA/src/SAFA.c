#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "consolaSAFA.h"
#include "libSAFA.h"

void connectionNew(socket_connection* socketInfo);
void disconnect(socket_connection* socketInfo);

int main(void){

        //señal para que al cortar el flujo, se libere memoria
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

        pthread_t hiloPCP, hiloConsola, hiloPLP;
        list_add(hilos, &hiloPCP);
        list_add(hilos, &hiloConsola);
        list_add(hilos, &hiloPLP);


        //configuracion de loggers
	configure_logger();
        read_and_log_config("S-AFA.config");
	log_info(logger, "Voy a escuchar por el puerto: %d", datosConfigSAFA->puerto);

        sem_init(&cantProcesosEnNew, 0, 0);
        sem_init(&cantProcesosEnReady, 0, 0);
        pthread_mutex_init(&m_colaReady, NULL);
	pthread_mutex_init(&m_colaBloqueados, NULL);
        pthread_mutex_init(&m_colaNew, NULL);

        pthread_create(&hiloConsola, NULL, (void*)&consolaSAFA, NULL);
        pthread_create(&hiloPCP, NULL, (void*)&planificarSegunRR, &datosConfigSAFA->quantum);
        pthread_create(&hiloPLP, NULL, (void*)&planificadorLargoPlazo, NULL);
       
        pthread_detach(hiloConsola);
        pthread_detach(hiloPCP);
        pthread_detach(hiloPLP);

        estadoCorrupto = true;
        //pongo a escuchar el server
	createListen(datosConfigSAFA->puerto, NULL, fns, &disconnect, NULL);


        
  	// bloqueo el thread, dejando la coneccion abierta
 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);       

        
        //cerrar hilos, no se como se hace

    return EXIT_SUCCESS;
}



void disconnect(socket_connection* socketInfo) {
  log_info(logger,"socket n° %d se ha desconectado.\n", socketInfo->socket);
}

void cerrarPrograma() {
    log_info(logger, "Voy a cerrar SAFA");
    pthread_mutex_destroy(&m_colaReady);
    pthread_mutex_destroy(&m_colaBloqueados);
    pthread_mutex_destroy(&m_colaNew);

    close_logger();
    dictionary_destroy_and_destroy_elements(fns, (void*) free); 
    free(datosConfigSAFA->algoritmoPlanif);
    free(datosConfigSAFA);

    queue_destroy_and_destroy_elements(colaReady, (void*)free);
    queue_destroy_and_destroy_elements(colaFinalizados, (void*)free);
    queue_destroy_and_destroy_elements(colaNew, (void*)free);
    queue_destroy_and_destroy_elements(colaBloqueados, (void*)free);
    list_destroy(hilos);


    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}
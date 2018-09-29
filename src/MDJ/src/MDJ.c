#include "libMDJ.h"

char * path = "MDJ.config";


int main(void) {
      
     signal(SIGINT, cerrarPrograma);
     configure_logger();
     datosConfMDJ =  read_and_log_config(path);
     portServer = datosConfMDJ->puerto;
     fns = dictionary_create();
      dictionary_put(fns, "DAM_MDJ_handshake", &DAM_MDJ_handshake);
         dictionary_put(fns, "identificarProceso", &identificarProceso);
      setValue(conf,path,"IP",getIp());
       //consolaMDJ();

       //Pongo a escuchar el server en el puerto elegido
        int listener = createListen(5001, &connectionNew ,fns, &disconnect ,NULL);
        log_info(logger,"Escuchando puerto %d " , portServer);
        if(listener == -1)
	{ 
               log_error(logger,"Error al crear escucha en puerto %d.\n", portServer);
		exit(1);
	}

     
	printf("Escuchando nuevos clientes en puerto %d.\n", portServer);

	
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

  
	return EXIT_SUCCESS;
}

void cerrarPrograma() {
    log_info(logger, "Voy a cerrar MDJ");
    close_logger();
    dictionary_destroy_and_destroy_elements(fns, (void*)free); 
    free(datosConfMDJ);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}


#include "libMDJ.h"

char * path = "MDJ.config";


void validarExistencia(socket_connection * , char ** );
int main(void) {
      
     signal(SIGINT, cerrarPrograma);
     configure_logger();
     datosConfMDJ =  read_and_log_config(path);
     portServer = datosConfMDJ->puerto;
     fns = dictionary_create();
      dictionary_put(fns, "identificarProcesoEnMDJ", &identificarProceso);
      dictionary_put(fns,"validarArchivo",&validarArchivo);
      setValue(conf,path,"IP",getIp());
       //consolaMDJ();

       //Pongo a escuchar el server en el puerto elegido
        createListen(5001, NULL ,fns, &disconnect ,NULL);
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
    dictionary_destroy(fns);
    free(datosConfMDJ);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}




#include "libMDJ.h"

//PATH RELATIVO PQ SINO NO ANDA EN OTRAS COMPUS
char * path = "MDJ.config";



int main(void) {
      
     signal(SIGINT, cerrarPrograma);
     configure_logger();
     datosConfMDJ = read_and_log_config(path);
     portServer = datosConfMDJ->puerto;
     fns = dictionary_create();
     dictionary_put(fns,"validarArchivo",&validarArchivo);
     dictionary_put(fns,"crearArchivo",&crearArchivo);
     dictionary_put(fns,"borrarArchivo",&borrarArchivo);
     dictionary_put(fns,"identificarProcesoEnMDJ",&identificarProceso);
     dictionary_put(fns,"guardarDatos",&guardarDatos);
     dictionary_put(fns,"obtenerDatos",&obtenerDatos);

   fs = malloc(sizeof(t_metadata_filesystem));
char * motanjeMasBin = string_new();
string_append(&motanjeMasBin,obtenerPtoMontaje());
string_append(&motanjeMasBin,"/Metadata/Metadata.bin");
t_config * metadata = config_create(motanjeMasBin);
fs->tamanio_bloques = (size_t) config_get_int_value(metadata,"TAMANIO_BLOQUES");
fs->cantidad_bloques = config_get_int_value(metadata,"CANTIDAD_BLOQUES");
fs->magic_number = malloc(strlen(config_get_string_value(metadata,"MAGIC_NUMBER")) + 1);
strcpy(fs->magic_number,config_get_string_value(metadata,"MAGIC_NUMBER"));
config_destroy(metadata);

TAM_BLOQUE = fs->tamanio_bloques;
CANT_BLOQUES = fs->cantidad_bloques;

     pthread_t hiloConsola;

    pthread_mutex_init(&mdjInterfaz,NULL);

    pthread_create(&hiloConsola, NULL, (void*)&consolaMDJ, NULL);

    pthread_detach(hiloConsola);   
    
       //Pongo a escuchar el server en el puerto elegido
       int listener =  createListen(portServer, NULL ,fns, &disconnect ,NULL);
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
    if( datosConfMDJ->ptoMontaje!=NULL)
        free(datosConfMDJ->ptoMontaje);
    free(datosConfMDJ);
    if(fs->magic_number != NULL)
        free(fs->magic_number);
    free(fs);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}




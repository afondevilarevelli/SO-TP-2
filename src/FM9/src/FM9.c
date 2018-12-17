#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "comandosMemoria.h"

void consolaFM9();

void cerrarPrograma() {
	log_info(logger, "Voy a cerrar FM9");

	close_logger();
	free(datosConfigFM9);
	free(memoria);
	list_destroy(lista_tabla_segmentos);
	list_destroy(lista_archivos);
	dictionary_destroy(callableRemoteFunctions);
	pthread_mutex_unlock(&mx_main);
	pthread_mutex_destroy(&mx_main);
}

int main(void) {
	pthread_t hiloConsola;
	//Esto es para liberar memoria despues de ctrl-c
	signal(SIGINT, cerrarPrograma);

	configure_logger();

	datosConfigFM9 = read_and_log_config("FM9.config");

	//Me fijo el modo
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		log_info(logger, "Modo segmentacion");
		inicializarMemoriaConSegmentacion();
	}else{
		if(strcmp(datosConfigFM9->modo, "TPI")==0){
			inicializarMemoriaConPaginacionInvertida();
		}else{
			if(strcmp(datosConfigFM9->modo, "SPA")==0){
				log_info(logger, "Modo Segmentacion Paginada");
				log_error(logger, "TODO Segmentacion paginada");
				cerrarPrograma();
			}else{
				log_error(logger, "Modo no reconocido");
				cerrarPrograma();
			}
		}
	}
	
	lista_archivos = list_create();
	callableRemoteFunctions = dictionary_create();

	dictionary_put(callableRemoteFunctions, "identificarProcesoEnFM9", &identificarProceso);
	dictionary_put(callableRemoteFunctions, "DAM_FM9_cargarArchivo", &solicitudCargaArchivo);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_actualizarLosDatosDelArchivo", &actualizarDatosDTB);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_cerrarElArchivo", &cerrarArchivoDelDTB);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_obtenerDatos", &obtenerDatos);
	//Funcion para iniciar la ejecucion del Flush
	//dictionary_put(callableRemoteFunctions, "DAM_FM9_obtenerArchivo", &obtenerArchivo);

	log_info(logger, "Voy a escuchar el puerto: %d", datosConfigFM9->puerto);

	//pthread_create(&hiloConsola, NULL, (void*)&consolaFM9, NULL);       
    //pthread_detach(hiloConsola); 

	createListen(datosConfigFM9->puerto,NULL,
			callableRemoteFunctions, &disconnect, NULL);
	loadCommands();  

	log_info(logger, "Estoy escuchando el puerto: %d", datosConfigFM9->puerto);
/*
	char* args[5] = {"1","linea 1\nlinea 2\nlinea 3\n","ultima","1","1"};
	char** aa = args;

	char* args4[4] = {"1", "2", "-1", "0", "-1"};
	char** cc = args4;
	solicitudCargaArchivo(NULL, aa);

	obtenerDatos(NULL, cc);

	executeCommand("dump 1");*/
	//conexion al servidor----------------------------
	
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main); 
	return EXIT_SUCCESS;
}

void consolaFM9(){
	char *buffer;
	size_t bufsize = 1024;
	buffer = (char *)malloc(bufsize * sizeof(char));
	while(1) {
		log_info(logger, "Ingrese un Comando");
		getline(&buffer, &bufsize, stdin);
		while(buffer == NULL)
			getline(&buffer, &bufsize, stdin);
		if(strcmp(buffer, "salir") != 0)
			executeCommand(buffer);
		else
			break;
	}
	free(buffer);
}
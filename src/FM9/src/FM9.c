#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "comandosMemoria.h"


void cerrarPrograma() {
	log_info(logger, "Voy a cerrar FM9");

	close_logger();
	free(datosConfigFM9);
	free(memoria);
	pthread_mutex_destroy(&m_memoria);
	list_destroy(lista_tabla_segmentos);
	list_destroy(tabla_paginasInvertidas);
	dictionary_destroy(callableRemoteFunctions);
	pthread_mutex_unlock(&mx_main);
	pthread_mutex_destroy(&mx_main);
}

int main(void) {
	pthread_t hiloConsola;
	tamanioOcupadoBufferAux = 0;
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
	pthread_mutex_init(&m_memoria, NULL);
	callableRemoteFunctions = dictionary_create();

	dictionary_put(callableRemoteFunctions, "identificarProcesoEnFM9", &identificarProceso);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_actualizarLosDatosDelArchivo", &actualizarDatosDTB);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_cerrarElArchivo", &cerrarArchivoDeDTB);
	dictionary_put(callableRemoteFunctions, "CPU_FM9_obtenerDatos", &obtenerDatosCPU);
	dictionary_put(callableRemoteFunctions, "DAM_FM9_cargarBuffer", &cargarBuffer);
	dictionary_put(callableRemoteFunctions, "DAM_FM9_obtenerDatosFlush", &DAM_FM9_obtenerDatosFlush);

	log_info(logger, "Voy a escuchar el puerto: %d", datosConfigFM9->puerto);

	pthread_create(&hiloConsola, NULL, (void*)&consolaFM9, NULL);       
    pthread_detach(hiloConsola); 

	createListen(datosConfigFM9->puerto,NULL,
			callableRemoteFunctions, &disconnect, NULL);
	loadCommands();  

	log_info(logger, "Estoy escuchando el puerto: %d", datosConfigFM9->puerto);
/*
	char* buffers[5] = {"linea"," 1\nli","nea 2","\nline","a 331\n"};
	char* buffers2[4] = {"Soy re capo\n","Aguante\n","Chupam","ela"};
	char* args1[5] = {"1", buffers[0], "sigue", "0", "1"};
	char* args2[5] = {"1", buffers[1], "sigue", "0", "0"};
	char* args3[5] = {"1", buffers[2], "sigue", "0", "0"};
	char* args4[5] = {"1", buffers[3], "sigue", "0", "0"};
	char* args5[5] = {"1", buffers[4], "ultima", "0", "0"};
	char** aa = args1;
	char** bb = args2;
	char** cc = args3;
	char** dd = args4;
	char** ee = args5;

	char* args199[5] = {"1", buffers2[0], "sigue", "0", "1"};
	char* args29[5] = {"1", buffers2[1], "sigue", "0", "0"};
	char* args39[5] = {"1", buffers2[2], "sigue", "0", "0"};
	char* args49[5] = {"1", buffers2[3], "ultima", "0", "0"};
	char** aa1 = args199;
	char** bb1 = args29;
	char** cc1 = args39;
	char** dd1 = args49;

	cargarBuffer(NULL,aa);
	cargarBuffer(NULL,bb);
	cargarBuffer(NULL,cc);
	cargarBuffer(NULL,dd);
	cargarBuffer(NULL,ee);

	cargarBuffer(NULL,aa1);
	cargarBuffer(NULL,bb1);
	cargarBuffer(NULL,cc1);
	cargarBuffer(NULL,dd1);

	char* args6[5] = {"1", "2", "1", "-1", "0"};
	char** tt = args6;
	obtenerDatosCPU(NULL, tt);

	char* args7[7] = {"1","0","3","-1","0","-1","-1"};
	char** gg = args7;
	char* args8[7] = {"1","3","3","-1","0","-1","-1"};
	char** gh = args8;
	char* args9[7] = {"1","6","3","-1","0","-1","-1"};
	char** gi = args9;
	char* args10[7] = {"1","9","3","-1","0","-1","-1"};
	char** gj = args10;
	char* args11[7] = {"1","12","3","-1","0","-1","-1"};
	char** gk = args11;
	char* args12[7] = {"1","15","3","-1","0","-1","-1"};
	char** gl = args12;
	char* args13[7] = {"1","18","3","-1","0","-1","-1"};
	char** gm = args13;
	char* args14[7] = {"1","21","3","-1","0","-1","-1"};
	char** gn = args14;
	char* args15[7] = {"1","24","3","-1","0","-1","-1"};
	char** go = args15;*/
/*
	DAM_FM9_obtenerDatosFlush(NULL,gg);
	DAM_FM9_obtenerDatosFlush(NULL,gh);
	DAM_FM9_obtenerDatosFlush(NULL,gi);
	DAM_FM9_obtenerDatosFlush(NULL,gj);
	DAM_FM9_obtenerDatosFlush(NULL,gk);
	DAM_FM9_obtenerDatosFlush(NULL,gl);
	DAM_FM9_obtenerDatosFlush(NULL,gm);
	DAM_FM9_obtenerDatosFlush(NULL,gn);
	DAM_FM9_obtenerDatosFlush(NULL,go); */

	//char* args19[5] = {"1","-1","0","-1","juanito.txt"};
	//char** memo = args19;
	//cerrarArchivoDeDTB(NULL, memo);
	//executeCommand("dump 1"); 
	//conexion al servidor----------------------------
	
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main); 
	return EXIT_SUCCESS;
}
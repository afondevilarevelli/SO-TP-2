#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "libDAM.h"

void disconnect();

int main(void){
     
	 signal(SIGINT, cerrarPrograma);
	
       	configure_logger();
	datosConfigDAM =read_and_log_config("DAM.config");
	//diccionarios

        callableRemoteFunctionsMDJ = dictionary_create();
	callableRemoteFunctionsFM9 = dictionary_create();
	 callableRemoteFunctionsSAFA = dictionary_create();
	callableRemoteFunctionsCPU = dictionary_create();
	
	//--------------------------------------------------------------------
	
	dictionary_put(callableRemoteFunctionsFM9, "FM9_DAM_handshake", &FM9_DAM_handshake);
	dictionary_put(callableRemoteFunctionsSAFA, "SAFA_DAM_handshake", &SAFA_DAM_handshake);
	dictionary_put(callableRemoteFunctionsMDJ, "MDJ_DAM_handshake", &MDJ_DAM_handshake);
      dictionary_put(callableRemoteFunctionsMDJ, "MDJ_DAM_existeArchivo", &MDJ_DAM_existeArchivo);
      dictionary_put(callableRemoteFunctionsCPU, "CPU_DAM_handshake", &CPU_DAM_handshake);

          


	int socketSAFA = connectServer("172.17.0.1", 8001,callableRemoteFunctionsSAFA, &disconnect, NULL);
	int socketFM9 = connectServer("172.17.0.1",8003, callableRemoteFunctionsFM9, &disconnect, NULL);
	int socketMDJ = connectServer("172.17.0.1", 5001, callableRemoteFunctionsMDJ, &disconnect, NULL);
         
	
	if(socketSAFA== -1  ){
	log_error(logger,"no se pudo conectar con SAFA");
	 }
        else  {
        log_info(logger,"voy a hacer un handshake con SAFA");
         runFunction(socketSAFA,"identificarProceso",1,"DAM");
        runFunction(socketSAFA,"DAM_SAFA_handshake",0);
         }

     if (socketFM9 == -1) {
      log_error(logger,"no se pudo conectar con FM9");
       }
       else  {
        log_info(logger,"voy a hacer un handshake con  FM9");
         runFunction(socketFM9,"DAM_FM9_handshake",0);
         }
      

        if(socketMDJ == -1){
        log_error(logger,"no se pudo conectar con MDJ");
        }
       else {
       log_info(logger,"voy a hacer un handshake con  MDJ");
        //runFunction(socketMDJ,"identificarProceso",1,"DAM");
        runFunction(socketMDJ,"DAM_MDJ_handshake",0);
       }
      free(pro);

        log_info(logger,"voy a escuchar el puerto %d ",datosConfigDAM->puertoEscucha);
	int listener = createListen(datosConfigDAM->puertoEscucha, &connectionNew,callableRemoteFunctionsCPU, &disconnect, pro);
       
 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);


	return EXIT_SUCCESS;
}

//FUNCIONES
void disconnect(){
  log_info(logger,"se ha desconectado :(");
}


void cerrarPrograma() {
    log_info(logger, "Voy a cerrar DAM");
    close_logger();
     dictionary_destroy(callableRemoteFunctionsSAFA);
     dictionary_destroy(callableRemoteFunctionsFM9);
     dictionary_destroy(callableRemoteFunctionsMDJ);
     free(datosConfigDAM);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}

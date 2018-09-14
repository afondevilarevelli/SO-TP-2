#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mySocket.h"

#include "libDAM.h"

void disconnect();



int main(void){
     
	
	
	
	t_config_DAM* datosConfigDAM;
   
	configure_logger();
	datosConfigDAM =read_and_log_config("DAM.config");
	
	//diccionarios
	t_dictionary* callableRemoteFunctionsFM9 = dictionary_create();
	t_dictionary* callableRemoteFunctionsSAFA = dictionary_create();
	
	//--------------------------------------------------------------------
	
	dictionary_put(callableRemoteFunctionsFM9, "FM9_DAM_handshake", &FM9_DAM_handshake);
	dictionary_put(callableRemoteFunctionsSAFA, "SAFA_DAM_handshake", &SAFA_DAM_handshake);
	//dictionary_put(callableRemoteFunctions, "MDJ_DAM_handshake", &MDJ_DAM_handshake);
	
	
	//coneccion al servidor----------------------------------
	int socket2 = connectServer(datosConfigDAM->IPSAFA,datosConfigDAM->puertoSAFA, callableRemoteFunctionsSAFA, &disconnect, NULL);
	int socket = connectServer(datosConfigDAM->IPFM9, datosConfigDAM->puertoFM9, callableRemoteFunctionsFM9, &disconnect, NULL);
	/*int socketSAFA = connectServer(datosConfigDAM->IPSAFA, datosConfigDAM->puertoSAFA, callableRemoteFunctions, &disconnect, NULL);
	int socketMDJ = connectServer(datosConfigDAM->IPMDJ, datosConfigDAM->puertoMDJ, callableRemoteFunctions, &disconnect, NULL);
	*/
	printf("el socket SAFA dio :%d \n",socketSAFA);
	if(socket == -1){
		log_info(logger,"no se pudo conectar con FM9");
		return -1;
	}
	
	
	
	log_info(logger,"Voy a hacer un handshake con SAFA");

	runFunction(socket,"DAM_FM9_handshake",0);
	//Espero a que me responda memoria
	sleep(10);
	
  	if (socket2 == -1) {
    	printf("no me pude conectar con SAFA");
    	return -1;
  	}
  
  	sleep(1);
  	// este metodo no existe en el protocolo del servidor, por eso va a putear el server
  	runFunction(socket2,"DAM_SAFA_handshake",0);

  	sleep(10);
  	
	free(datosConfigDAM);
    	                      
	close_logger();
	return 0;
}

//FUNCIONES
void disconnect(){
  log_info(logger,"se ha desconectado :(");
}

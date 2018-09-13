#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mySocket.h"

#include "libDAM.h"

void disconnect();



int main(void){
     
	/*
	int socket = connectTo(SAFA_IP, SAFA_PORT);
	t_dictionary * fns = dictionary_create();*/
	t_config_DAM* datosConfigDAM;
   
	configure_logger();
	datosConfigDAM =read_and_log_config("DAM.config");
	
	t_dictionary* callableRemoteFunctions = dictionary_create();

	dictionary_put(callableRemoteFunctions, "FM9_DAM_handshake", &FM9_DAM_handshake);

	int socket = connectServer(datosConfigDAM->IPFM9, datosConfigDAM->puertoFM9, callableRemoteFunctions, &disconnect, NULL);
	
	if(socket == -1){
		log_info(logger,"no se pudo conectar ");
		return -1;
	}
	
	log_info(logger,"Voy a hacer un handshake con FM9");

	runFunction(socket,"DAM_FM9_handshake",0);
	
	//Espero a que me responda memoria
	sleep(10);
	
	
	
	

     /* if(recvWithBasicProtocol(socket, &mensajeARecibir) == -1)
	{
		perror("Error al recibir datos del nuevo socket");
		exit(1);
	}
    printf("Se ha recibido el siguiente mensaje: %s\n", (char*)mensajeARecibir);
    
  	socketSAFA = connectServer("127.0.0.2", datosConfigDAM->puertoSAFA, fns, &disconnectSAFA, NULL);


  	if (socketSAFA == -1) {
    	printf("no me pude conectar");
    	return -1;
  	}
  
  	sleep(1);
  	// este metodo no existe en el protocolo del servidor, por eso va a putear el server
  	runFunction(socketSAFA, "imprimirMensaje", 1, "Hola Safa, soy el Diego (DAM)");

	free(datosConfigDAM);
    	                      */
	close_logger();
	return 0;
}

//FUNCIONES
void disconnect(){
  log_info(logger,"se ha desconectado :(");
}

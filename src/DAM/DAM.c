#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mySocket.h"

#include "libDAM.h"

void disconnectSAFA();



int main(void){
      int socket = connectTo(SAFA_IP, SAFA_PORT);
	t_dictionary * fns = dictionary_create();


	configure_logger();
	read_and_log_config("DAM.config");
	close_logger();

    if(recvWithBasicProtocol(socket, &mensajeARecibir) == -1)
	{
		perror("Error al recibir datos del nuevo socket");
		exit(1);
	}
    printf("Se ha recibido el siguiente mensaje: %s\n", (char*)mensajeARecibir);*/
    
    
    
    

  	socketSAFA = connectServer("127.0.0.2", datosConfigDAM->puertoSAFA, fns, &disconnectSAFA, NULL);


  	if (socketSAFA == -1) {
    	printf("no me pude conectar");
    	return -1;
  	}
  
  	sleep(1);
  	// este metodo no existe en el protocolo del servidor, por eso va a putear el server
  	runFunction(socketSAFA, "imprimirMensaje", 1, "Hola Safa, soy el Diego (DAM)");

	free(datosConfigDAM);
	return 0;
}

//FUNCIONES
void disconnectSAFA(){
  printf("se ha desconectado del SAFA :(\n");
}

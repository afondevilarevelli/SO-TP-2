#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/mySocket.h"
#include "libDAM.h"

#define SAFA_IP INADDR_ANY
#define SAFA_PORT 3067



int main(void){
    /* int socket = connectTo(SAFA_IP, SAFA_PORT);

	//Le envio al coordinador que tipo de proceso soy
	char* mensajeAEnviar = "Hola S-AFA, soy el Diego";
    void* mensajeARecibir;
	sendWithBasicProtocol(socket, (void*)mensajeAEnviar, strlen(mensajeAEnviar)+1);
    printf("Se ha enviado el siguiente mensaje: %s\n\n", mensajeAEnviar);

    if(recvWithBasicProtocol(socket, &mensajeARecibir) == -1)
	{
		perror("Error al recibir datos del nuevo socket");
		exit(1);
	}
    printf("Se ha recibido el siguiente mensaje: %s\n", (char*)mensajeARecibir);*/
    
    //archivo de configuracion y logs
    
    configure_logger();
    
    read_and_log_config("DAM.config");

    close_logger();
    
    

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <arpa/inet.h>
#include <string.h>
#include "consolaSAFA.h"
#include "../shared/mySocket.h"
#include "libSAFA/libSAFA.h"

#define IP INADDR_ANY //Toma el ip de la pc
#define PORT 3067

void atenderConexionEntrante(int listener);

int main(void){

	configure_logger();

	read_and_log_config("S-AFA.config");

	close_logger();

	//dejare al coordinador escuchar nuevas conexiones a traves del IP y PUERTO indicados
	int listener = listenOn(IP, PORT);
	int max_fd = listener;
	fd_set master_set, read_set;
	FD_ZERO(&master_set);
	FD_SET(listener, &master_set);

	//lo dejare atender las conexiones entrantes que detecta el listener
	while(1)
	{
		read_set = master_set;
		select(listener  + 1, &read_set, NULL, NULL, NULL);

		atenderConexionEntrante(listener);
	}



    return 0;
}
/*
void obtenerIPyPuertoDeSAFA(t_config * pConf, int * ip, int * puerto)
{
	char * strIP = config_get_string_value(pConf, "A-AFA_IP");
	*ip = inet_addr(strIP);

	*puerto= htons(config_get_int_value(pConf, "S-AFA_PUERTO"));

} */

void atenderConexionEntrante(int listener)
{
	void * mensajeAEnviar = "Hola Diego, soy el S-AFA";
	void * mensajeARecibir;

	//acepto la conexion
	int nuevaConexion = acceptClient(listener);

	int bytes = recvWithBasicProtocol( nuevaConexion, &mensajeARecibir);
	if(!bytes)
	{
		perror("Error al recibir datos del nuevo socket");
		exit(1);
	}

	printf("Se ha recibido el siguiente mensaje: %s\n\n", (char*)mensajeARecibir);

	sendWithBasicProtocol(nuevaConexion, mensajeAEnviar, strlen((char*)mensajeAEnviar)+1);
	printf("Se ha enviado el siguiente mensaje: %s\n", (char*)mensajeAEnviar);

}
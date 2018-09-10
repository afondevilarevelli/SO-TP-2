#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "../../sample-socket/socket.h"
#include "libMDJ.h"


#define IP INADDR_ANY

//Globales
t_dictionary *  fns;	/* Funciones de socket */
pthread_mutex_t mx_main;	/* Semaforo de main */
int portServer;

//prototipos

void newClient(socket_connection *);
void DAM_saludar(socket_connection *,char **);
void client_connectionClosed(socket_connection *);
char * getIp();


int main(void) {
          
       t_config * conf = config_create("MDJ.config");
          portServer = obtenerInt(conf,"MDJ.config","PUERTO");  /* Puerto de escucha */
	  fns = dictionary_create();
	  dictionary_put(fns, "DAM_saludar", &DAM_saludar);
          setValue(conf,"MDJ.config","IP",getIp());
        
         configure_logger();
        read_and_log_config("MDJ.config");
       close_logger();

       //Pongo a escuchar el server en el puerto elegido
        if(createListen(portServer, &newClient, fns, &client_connectionClosed, NULL) == -1)
	{
		printf("Error al crear escucha en puerto %d.\n", portServer);
		exit(1);
	}
       
       sleep(1);
     
	printf("Escuchando nuevos clientes en puerto %d.\n", portServer);

	
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
       
  
	return EXIT_SUCCESS;
}


// Funcion que se produce cuando se conecta un nuevo cliente
void newClient(socket_connection * connection)
{
	printf("Se ha conectado un nuevo cliente. Socket = %d, IP = %s, Puerto = %d.\n", connection->socket, connection->ip, connection->port);
}


// El cliente me saluda
void DAM_saludar(socket_connection * connection, char ** args)
{
	printf("Cliente (Socket = %d, IP = %s, Puerto = %d) me saluda = %s %s %s.\n", connection->socket, connection->ip, connection->port, args[0], args[1], args[2]);
        sleep(2);
	

	//devuelvo saludo
	runFunction(connection->socket, "MDJ_saludar", 2, "hola", "DAM!");
}

char *  getIp()
{
 int fd;
 struct ifreq ifr;

 fd = socket(  AF_INET, SOCK_STREAM, AF_UNSPEC);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "docker0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 /* display result */
 return  inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

 
}

// Funcion que se produce cuando se desconecta un cliente
void client_connectionClosed(socket_connection * connection)
{
	printf("Se ha desconectado un cliente. Socket = %d, IP = %s, Puerto = %d.\n", connection->socket, connection->ip, connection->port);
}
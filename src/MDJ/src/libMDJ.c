#include "libMDJ.h"



void configure_logger() {

	char * nombrePrograma = "MDJ.log";
	char * nombreArchivo = "MDJ";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se genero log de MDJ");
     
}

void close_logger() {
	log_info(logger, "Cierro log de MDJ");
	log_destroy(logger);
}


//SOCKETS

void identificarProceso(socket_connection * connection ,char** args){	
    log_info(logger,"Se ha conectado %s en el socket NRO %d  con IP %s,  PUERTO %d\n", args[0],connection->socket,connection->ip,connection-> port);   
}

void disconnect(socket_connection* socketInfo) {
	log_info(logger, "socket n  %d  se ha desconectado.", socketInfo->socket);
}


char *  getIp(){
int fd;
 struct ifreq ifr;
 fd =socket(AF_INET, SOCK_STREAM, 0);
 ifr.ifr_addr.sa_family = AF_INET;
strncpy(ifr.ifr_name, "docker0", IFNAMSIZ-1);
 ioctl(fd, SIOCGIFADDR, &ifr);
close(fd);
 return  (inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}



//CONFIG
t_config_MDJ *  read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo MDJ.config");
        t_config* archivo_Config  = config_create(path);
	if (existeArchivoConf(path) == 0) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);   }
        else
      {
       log_info(logger,"Se verifico que existe MDJ.config"); }
	
        t_config_MDJ  * datosMDJ = malloc(sizeof(t_config_MDJ));
	datosMDJ->puerto = obtenerInt(archivo_Config,path,"PUERTO");
	char*  ip = string_new();
	string_append(&ip,  obtenerString(archivo_Config,path,"IP"));
	datosMDJ->ip = ip;
        char*  ptoMontaje = string_new();
	string_append(&ptoMontaje, obtenerString(archivo_Config,path,"PTO_MONTAJE"));
        datosMDJ->ptoMontaje = ptoMontaje;
	datosMDJ->retardo= obtenerInt(archivo_Config,path,"RETARDO");
	log_info(logger, "	PUERTO: %d", datosMDJ->puerto);
	log_info(logger, "	IP: %s", datosMDJ->ip);
	log_info(logger, "	PTO_MONTAJE: %s", datosMDJ->ptoMontaje);
	log_info(logger, "	RETARDO: %d", datosMDJ->retardo);
	log_info(logger, "Fin de lectura");

       config_destroy(archivo_Config);
       free(ip);
       free(ptoMontaje);
       return datosMDJ;

}

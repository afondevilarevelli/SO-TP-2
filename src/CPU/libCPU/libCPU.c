#include "libCPU.h"

void configure_logger() {

	char * nombrePrograma = "CPU.log";
	char * nombreArchivo = "CPU";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se Creo El Archivo Log de CPU");

}

void close_logger() {
	log_info(logger, "Cierro log de CPU");
	log_destroy(logger);
}

//CONFIG
void read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo CPU.config");
        archivo_Config  = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);   }
        else
      {
       log_info(logger,"Se verifico que existe CPU.config"); }

    datosCPU = malloc(sizeof(t_config_CPU));

    char* ipS = string_new();
    string_append(&ipS,  config_get_string_value(archivo_Config,"IP_SAFA"));
    datosCPU->ipS = ipS;
	//datosCPU->ipS = config_get_int_value(archivo_Config,"IP_SAFA");

	datosCPU->puertoS = config_get_int_value(archivo_Config,"S-AFA_PUERTO");

	char* ipD = string_new();
	string_append(&ipD,  config_get_string_value(archivo_Config,"IP_DAM"));
	datosCPU->ipD = ipD;

	datosCPU->puertoD = config_get_int_value(archivo_Config,"DAM_PUERTO");

	datosCPU->retardo= config_get_int_value(archivo_Config,"RETARDO");

	log_info(logger, "IP_SAFA: %s", datosCPU->ipS);
	log_info(logger, "S-AFA_PUERTO: %d", datosCPU->puertoS);
	log_info(logger, "IP_DAM: %s", datosCPU->ipD);
	log_info(logger, "DAM_PUERTO: %d", datosCPU->puertoD);
	log_info(logger, "RETARDO: %d", datosCPU->retardo);
	log_info(logger, "Todos los Datos Fueron Cargados");

	config_destroy(archivo_Config);
	free(datosCPU);

}

void saliendo_por_error(int socket, char* error, void* buffer)
{
	if(buffer != NULL)
	{
		free(buffer);
	}

	log_error(logger, error);
	close(socket);
	exit_gracefully(1);

}

void enviando_mensaje(int socket, char* mensaje) {

  log_info(logger, "Enviando Mensaje");

  sendWithBasicProtocol(socket, (void *)mensaje, strlen(mensaje) + 1 );

  log_info(logger,"El Mensaje: %s Fue Enviado Sin Problemas", mensaje);
  }


void  esperando_respuesta(int socket) {

    char * buffer = malloc(sizeof(char*));

    int result_recv = recvWithBasicProtocol(socket,(void *) &buffer);

    if(result_recv < 0)
    {
    	saliendo_por_error(socket, "No se pudo recibir el mensaje", buffer);
    }

    log_info(logger, "Mensaje Recibido: '%s'", buffer);
    free(buffer);

  }

void exit_gracefully(int return_nr) {

	log_destroy(logger);
	exit(return_nr);

}

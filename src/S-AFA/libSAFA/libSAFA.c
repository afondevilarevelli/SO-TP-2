#include <stdio.h>
#include <stdlib.h>
#include "libSAFA.h"

//LOG
void configure_logger() {

	char * nombrePrograma = "SAFA.log";
	char * nombreArchivo = "SAFA";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se genero log de S-AFA");
}

void close_logger() {
	log_info(logger, "Cierro log de S-AFA");
	log_destroy(logger);
}

//CONFIG
void read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo S-AFA.config");

	t_config* archivo_Config = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);
	}

	datosConfigSAFA = malloc(sizeof(t_config_SAFA));

	datosConfigSAFA->puerto = config_get_int_value(archivo_Config, "S-AFA_PUERTO");
	char* alg = string_new();
	string_append(&alg, config_get_string_value(archivo_Config, "ALGORITMO_PLANIF"));
	datosConfigSAFA->algoritmoPlanif = alg;
	datosConfigSAFA->quantum = config_get_int_value(archivo_Config, "QUANTUM");
	datosConfigSAFA->gradoMultiprog = config_get_int_value(archivo_Config, "GRADO_MULTIPROG");
	datosConfigSAFA->retardo = config_get_int_value(archivo_Config, "RETARDO");

	log_info(logger, "	PUERTO: %d", datosConfigSAFA->puerto);
	log_info(logger, "	ALGORITMO_PLANIFICACION: %s", datosConfigSAFA->algoritmoPlanif);
	log_info(logger, "	QUANTUM: %d", datosConfigSAFA->quantum);
	log_info(logger, "	GRADO_MULTIPROGRAMACION: %d", datosConfigSAFA->gradoMultiprog);
	log_info(logger, "	RETARDO: %d", datosConfigSAFA->retardo);

	log_info(logger, "Fin de lectura");

	config_destroy(archivo_Config);
	free(alg);
} // al final de esta funcion me queda la variable datosConfigSAFA con la config de SAFA

//CallableRemoteFunctions
void imprimirMensaje(socket_connection* socketInfo, char** msg){
	printf("Se ha recibido el siguiente mensaje del socket %d: %s",socketInfo->socket, msg[0]);
}
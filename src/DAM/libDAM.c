#include <stdio.h>
#include <stdlib.h>
#include "libDAM.h"

//LOG
void configure_logger() {

	char * nombrePrograma = "DAM.log";
	char * nombreArchivo = "DAM";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se genero log de DAM");
}

void close_logger() {
	log_info(logger, "Cierro log de DAM");
	log_destroy(logger);
}

//CONFIG
void read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo DAM.config");

	t_config* archivo_Config = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);
	}

	datosConfigDAM = malloc(sizeof(t_config_DAM));
	
	

	datosConfigDAM->puertoEscucha = config_get_int_value(archivo_Config, "DAM_PUERTO");
	strcpy(datosConfigDAM->IPSAFA, config_get_string_value(archivo_Config, "S-AFA_IP"));
	datosConfigDAM->puertoSAFA = config_get_int_value(archivo_Config, "S-AFA_PUERTO");
	strcpy(datosConfigDAM->IPMDJ, config_get_string_value(archivo_Config, "MDJ_IP"));
	datosConfigDAM->puertoMDJ = config_get_int_value(archivo_Config, "MDJ_PUERTO");
	//strcpy(datosConfigDAM->IPFM9, config_get_string_value(archivo_Config, "FM9_IP"));
	datosConfigDAM->puertoFM9 = config_get_int_value(archivo_Config, "FM9_PUERTO");
	datosConfigDAM->transferSize = config_get_int_value(archivo_Config, "TRANSFER_SIZE");

	log_info(logger, "	PUERTO ESCUCHA: %d", datosConfigDAM->puertoEscucha);
	log_info(logger, "	IP DE S-AFA: %s", datosConfigDAM->IPSAFA);
	log_info(logger, "	PUERTO DE S-AFA: %d", datosConfigDAM->puertoSAFA);
	log_info(logger, "	IP DE MDJ: %d", datosConfigDAM->IPMDJ);
	log_info(logger, "	PUERTO DE MDJ: %d", datosConfigDAM->puertoMDJ);
	//log_info(logger, "	IP DE FM9: %d", datosConfigDAM->IPFM9);
	log_info(logger, "	PUERTO DE FM9: %d", datosConfigDAM->puertoFM9);
	log_info(logger, "	TRANSFER SIZE: %d", datosConfigDAM->transferSize);

	log_info(logger, "Fin de lectura");

	config_destroy(archivo_Config);

}

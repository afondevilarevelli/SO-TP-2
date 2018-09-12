#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
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
	
	char *IPSAFA = string_new() ;
	char *IPMDJ = string_new() ;
	char *IPFM9 = string_new() ;


	_datosDAM->puertoEscucha = config_get_int_value(archivo_Config, "DAM_PUERTO");
	string_append(&IPSAFA, config_get_string_value(archivo_Config, "S-AFA_IP"));
	_datosDAM->puertoSAFA = config_get_int_value(archivo_Config, "S-AFA_PUERTO");
	string_append(&IPMDJ, config_get_string_value(archivo_Config, "MDJ_IP"));
	_datosDAM->puertoMDJ = config_get_int_value(archivo_Config, "MDJ_PUERTO");
	string_append(&IPFM9, config_get_string_value(archivo_Config, "FM9_IP"));
	_datosDAM->puertoFM9 = config_get_int_value(archivo_Config, "FM9_PUERTO");
	_datosDAM->transferSize = config_get_int_value(archivo_Config, "TRANSFER_SIZE");

	log_info(logger, "	PUERTO ESCUCHA: %d", _datosDAM->puertoEscucha);
	log_info(logger, "	IP DE S-AFA: %s", _datosDAM->IPSAFA);
	log_info(logger, "	PUERTO DE S-AFA: %d", _datosDAM->puertoSAFA);
	log_info(logger, "	IP DE MDJ: %d", _datosDAM->IPMDJ);
	log_info(logger, "	PUERTO DE MDJ: %d", _datosDAM->puertoMDJ);
	log_info(logger, "	IP DE FM9: %d", _datosDAM->IPFM9);
	log_info(logger, "	PUERTO DE FM9: %d", _datosDAM->puertoFM9);
	log_info(logger, "	TRANSFER SIZE: %d", _datosDAM->transferSize);


	log_info(logger, "Fin de lectura");

	config_destroy(archivo_Config);

}

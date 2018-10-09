#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "libDAM.h"

typedef enum {existente,inexistente} estadoArchivo;
int estado;

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


t_config_DAM* read_and_log_config(char* path) {
//CONFIG

	log_info(logger, "Voy a leer el archivo DAM.config");

	t_config* archivo_Config = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);
	}

       _datosDAM = malloc(sizeof(t_config_DAM));
 	_datosDAM->puertoEscucha = config_get_int_value(archivo_Config, "PUERTO");
        char *IPSAFA = string_new();
	string_append(&IPSAFA, config_get_string_value(archivo_Config, "IP_SAFA"));
         _datosDAM->IPSAFA = IPSAFA;
	_datosDAM->puertoSAFA = config_get_int_value(archivo_Config, "PUERTO_SAFA");
        char * IPMDJ = string_new();
	string_append(&IPMDJ, config_get_string_value(archivo_Config, "IP_MDJ"));
         _datosDAM->IPMDJ = IPMDJ;
	_datosDAM->puertoMDJ = config_get_int_value(archivo_Config, "PUERTO_MDJ");
        char *IPFM9 = string_new();
	string_append(&IPFM9, config_get_string_value(archivo_Config, "IP_FM9"));
         _datosDAM->IPFM9 = IPFM9;
	_datosDAM->puertoFM9 = config_get_int_value(archivo_Config, "PUERTO_FM9");
	_datosDAM->transferSize = config_get_int_value(archivo_Config, "TRANSFER_SIZE");

	log_info(logger, "	PUERTO ESCUCHA: %d", _datosDAM->puertoEscucha);
	log_info(logger, "	IP DE S-AFA: %s", _datosDAM->IPSAFA);
	log_info(logger, "	PUERTO DE S-AFA: %d", _datosDAM->puertoSAFA);
	log_info(logger, "	IP DE MDJ: %s", _datosDAM->IPMDJ);
	log_info(logger, "	PUERTO DE MDJ: %d", _datosDAM->puertoMDJ);
	log_info(logger, "	IP DE FM9: %s", _datosDAM->IPFM9);
	log_info(logger, "	PUERTO DE FM9: %d", _datosDAM->puertoFM9);
	log_info(logger, "	TRANSFER SIZE: %d", _datosDAM->transferSize);


	log_info(logger, "Fin de lectura");
	config_destroy(archivo_Config);
        free(IPSAFA);
        free(IPMDJ);
        free(IPFM9);
	return _datosDAM;
}

//TODO ORDENAR
void FM9_DAM_handshake(socket_connection * connection, char ** args){
	log_info(logger, "Handshake con FM9");
}
void SAFA_DAM_handshake(socket_connection * connection, char ** args){
	log_info(logger, "Handshake con SAFA");
}
void MDJ_DAM_handshake(socket_connection * connection, char ** args){
	log_info(logger, "Handshake con MDJ");
}
void CPU_DAM_handshake(socket_connection * connection, char ** args) {
	runFunction(connection->socket,"DAM_CPU_handshake",0);
	log_info(logger, "Handshake con CPU");
}
void connectionNew(socket_connection* socketInfo) {
	log_info(logger, "Se ha conectado CPU con ip %s en socket n°%d",
			socketInfo->ip, socketInfo->socket);
}

void MDJ_DAM_existeArchivo(socket_connection* socketInf,char ** args){
estado =atoi( args[0]);
if(estado ==  existente)
{
log_info(logger," El MDJ informa archivo existente");
}
else
{
log_info(logger,"El MDJ informa archivo inexistente");
}
}

void existeArchivo(char * pathFile){
 runFunction(socketMDJ,"validarArchivo",1,pathFile);

}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "libDAM.h"

int estadoValidacion;
int estadoCreacion;
int estadoBorrado;

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

    t_config_DAM* datosDAM = malloc(sizeof(t_config_DAM));

 	datosDAM->puertoEscucha = config_get_int_value(archivo_Config, "PUERTO");

    datosDAM->IPSAFA = malloc( strlen(config_get_string_value(archivo_Config, "IP_SAFA")) + 1);
	strcpy(datosDAM->IPSAFA, config_get_string_value(archivo_Config, "IP_SAFA"));

	datosDAM->puertoSAFA = config_get_int_value(archivo_Config, "PUERTO_SAFA");

    datosDAM->IPMDJ = malloc( strlen(config_get_string_value(archivo_Config, "IP_MDJ")) + 1);
	strcpy(datosDAM->IPMDJ, config_get_string_value(archivo_Config, "IP_MDJ"));

	datosDAM->puertoMDJ = config_get_int_value(archivo_Config, "PUERTO_MDJ");

    datosDAM->IPFM9 = malloc( strlen(config_get_string_value(archivo_Config, "IP_FM9")) + 1);
	strcpy(datosDAM->IPFM9, config_get_string_value(archivo_Config, "IP_FM9"));

	datosDAM->puertoFM9 = config_get_int_value(archivo_Config, "PUERTO_FM9");

	datosDAM->transferSize = config_get_int_value(archivo_Config, "TRANSFER_SIZE");

	log_info(logger, "	PUERTO ESCUCHA: %d", datosDAM->puertoEscucha);
	log_info(logger, "	IP DE S-AFA: %s", datosDAM->IPSAFA);
	log_info(logger, "	PUERTO DE S-AFA: %d", datosDAM->puertoSAFA);
	log_info(logger, "	IP DE MDJ: %s", datosDAM->IPMDJ);
	log_info(logger, "	PUERTO DE MDJ: %d", datosDAM->puertoMDJ);
	log_info(logger, "	IP DE FM9: %s", datosDAM->IPFM9);
	log_info(logger, "	PUERTO DE FM9: %d", datosDAM->puertoFM9);
	log_info(logger, "	TRANSFER SIZE: %d", datosDAM->transferSize);


	log_info(logger, "Fin de lectura");
	config_destroy(archivo_Config);
	return datosDAM;
}

void identificarProceso(socket_connection * connection ,char** args){	
    log_info(logger,"Se ha conectado %s en el socket NRO %d  con IP %s,  PUERTO %d\n", args[0],connection->socket,connection->ip,connection-> port);   
}

//Comunicacion entre CPU-DAM para Cargar GDT
void solicitudCargaGDT(socket_connection* connection, char ** args){

	int idGDT = atoi(args[0]);
	char* rutaScript = args[1];

	log_trace(logger, "Voy a Buscar: %s Para PID: %d",rutaScript, idGDT);
	//(Para crear despues) runFunction(socketMDJ, "DAM_MDJ_buscarElArchivo",1, rutaScript);
}

//esta funcion le avisa al SAFA el resultado de la carga del DTBDummy,
//es llamada por el MDJ
//algun parametro de args[] debería ser "ok" ó "error"
void MDJ_DAM_avisarSAFAResultadoDTBDummy(socket_connection* socketInf,char ** args){
	//algo

	//if(todoBien){
		runFunction(socketSAFA, "avisoDamDTBDummy", 1, "ok"); //puede llevar mas parametros
	//} else{
	//	runFuncion(socketSAFA, "avisoDamDTBDummy", 1, "error"); //puede llevar mas parametros
	//}
}

void MDJ_DAM_existeArchivo(socket_connection* socketInf,char ** args){
estadoValidacion =atoi( args[0]);
if(estadoValidacion ==  1)
{
log_info(logger," El MDJ informa archivo existente");
}
else if (estadoValidacion ==  0)
{
log_info(logger,"El MDJ informa archivo inexistente");
}
else 
{
log_error(logger,"Ocurrio un error al verificar si existe el archivo");
}
}

//Parametros cambiados para ver que hacer si se encuentra creado o no
void MDJ_DAM_verificarArchivoCreado(/*socket_connection* conenction,char ** args*/char* pam1, char* pam2){

estadoCreacion = atoi( pam1);

if(estadoCreacion ==  1)
{
log_info(logger," El MDJ informa que se creo el archivo");
runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, pam2);

}
else
{
log_error(logger,"Ocurrio un error al querer crear el archivo");
runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, pam2);
}
}

void MDJ_DAM_verificameSiArchivoFueBorrado(/*socket_connection * connection,char ** args*/char* pam1, char* pam2){

estadoBorrado = atoi(pam1);

if(estadoBorrado ==  1)
{
log_info(logger," El MDJ informa que se borro el archivo");
runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, pam2);
}
else
{
log_error(logger,"Ocurrio un error al querer borrar el archivo");
runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, pam2);
}
}

void crearArchivo(socket_connection* connection, char** args){

	int	idDTB = atoi(args[0]);

	char* rutaArchivo = args[1];
	char* cantidadLineas = args[2];

	//Como todavia falta desarrollo del MDJ envio por parametro los posibles resultados con el DTB a buscar
	//runFunction(socketMDJ,"crearArchivo",2, rutaArchivo, cantidadLineas);
	MDJ_DAM_verificarArchivoCreado("1", args[0]);

}

void borrarArchivo(socket_connection* connection, char** args){

	int	idDTB = atoi(args[0]);

	char* rutaArchivo = args[1];

	//Idem Crear Archivo
	//runFunction(socketMDJ,"borrarArchivo",1, rutaArchivo);
	MDJ_DAM_verificameSiArchivoFueBorrado("0", args[0]);

}

void existeArchivo(socket_connection* socketMDJ, char * pathFile){
 runFunction(socketMDJ,"validarArchivo",1,pathFile);
}

/*
void solicitudDeFlush(char *pathDelArchivo)
{
	socket_connection* socketMDJ;
	socket_connection* socketFM9;
	char *buffer;
	
	runFunction(socketFM9,"obtenerArchivo",2,pathDelArchivo,buffer);
	runFunction(socketMDJ,"guardarArchivo",2,pathDelArchivo,buffer);
}
*/
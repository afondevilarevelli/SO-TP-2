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

void MDJ_DAM_verificarArchivoCreado(socket_connection* conenction,char ** args){
estadoCreacion =atoi( args[0]);
if(estadoCreacion ==  1)
{
log_info(logger," El MDJ informa que se creo el archivo");
}
else if (estadoCreacion == 0)
{
log_info(logger,"El MDJ informa que el archivo ya estaba creado");
}
else
{
log_error(logger,"Ocurrio un error al querer crear el archivo");
}
}

void MDJ_DAM_verificameSiArchivoFueBorrado(socket_connection * connection,char ** args){
estadoBorrado =atoi( args[0]);
if(estadoBorrado ==  1)
{
log_info(logger," El MDJ informa que se borro el archivo");
}
else if ( estadoBorrado == 0)
{
log_info(logger,"El MDJ informa que el archivo no existia,por lo que no se podia borrar");
}
else
{
log_error(logger,"Ocurrio un error al querer borrar un archivo");
}
}


void existeArchivo(socket_connection* socketMDJ, char * pathFile){
 runFunction(socketMDJ,"validarArchivo",1,pathFile);
}

void verificarArchivoCreado(socket_connection* socketMDJ, char * pathFile)
{
 runFunction(socketMDJ,"crearArchivo",1,pathFile);
}

void verificameSiArchivoFueBorrado(socket_connection* socketMDJ, char * pathFile)
{
runFunction(socketMDJ,"borrarArchivo",1,pathFile);
}
/*
void abrirArchivo(char *pathDelArchivo,char *buffer,DTB* d)
  {
   FILE *archivo;
   socket_connection* socketMDJ;
   socket_connection* socketSAFA;
   socket_connection* socketFM9;
   runFunction(socketMDJ,"validarArchivo",1,pathDelArchivo);
   if(estadoCreacion==0)
   {log_info(logger, "abriendo el archivo");
   archivo = fopen(pathDelArchivo, "r+");
   while(fread(buffer,sizeof(buffer),1,archivo) != EOF );
   runFunction(socketSAFA,"encolarDTB",2,colaBloqueados,d);
   runFunction(socketFM9,"cargarArchivo",1,buffer);
   runFunction(socketSAFA,"desencolarDTB",1,colaBloqueados);
   }
  }

void solicitudDeFlush(char *pathDelArchivo)
{
	socket_connection* socketMDJ;
	socket_connection* socketFM9;
	char *buffer;
	
	runFunction(socketFM9,"obtenerArchivo",2,pathDelArchivo,buffer);
	runFunction(socketMDJ,"guardarArchivo",2,pathDelArchivo,buffer);
}*/

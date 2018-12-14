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

//esta funcion le avisa al SAFA el resultado de la carga del DTBDummy,
//es llamada por el MDJ
//args[0]: idGDT, args[1]: estadoValidacion (1 => existe archivo, 0 => NO existe archivo), args[2]:path
void MDJ_DAM_avisarResultadoDTB(socket_connection* socketInf,char ** args){
	estadoValidacion =atoi( args[1]);
	if(estadoValidacion ==  1){ 
		log_info(logger," El MDJ informa archivo existente");
		runFunction(socketFM9, "DAM_FM9_cargarArchivo", 1, args[0]);
	}
	else if (estadoValidacion ==  0){ 
		log_info(logger,"El MDJ informa archivo inexistente");
		runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
	}
	else{ 
		log_error(logger,"Ocurrio un error al verificar si existe el archivo");
	}

}

//lamada por el FM9
//args[0]: idGDT, args[1]: Por ahora ok o error, args[2]: path, args[3]: 1(Dummy) ó 0(no Dummy)
void archivoCargadoCorrectamente(socket_connection* connection, char** args){

	char* estadoCarga = args[1];

	if(strcmp(estadoCarga, "ok") == 0 ){
		if(strcmp(args[3], "1") == 0)
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "ok");
		else{ 
			runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB", 1, args[0]);
			runFunction(socketSAFA, "aperturaArchivo", 2, args[0], args[2]);
		}
	}
	else{//error
		if(strcmp(args[3], "1") == 0)
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
		else
			runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit", 1, args[0]);
	}

}


//args[0]: idGDT, args[1]: 0(yaCreado), 1(recienCreado), -1(errorCreado), args[2]: arch
void MDJ_DAM_verificarArchivoCreado(socket_connection* conenction,char ** args){
	estadoCreacion = atoi(args[1]);

	if(estadoCreacion ==  1)
	{	
		log_trace(logger,"El MDJ informa que se ha creado el archivo %s por el GDT %s",args[2], args[0]);
		runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, args[0]);
	}
	else
	{
		if(estadoCreacion == 0)
			log_error(logger,"Archivo %s ya creado",args[2]);
		else
			log_error(logger,"Error al crear el archivo %s",args[2]);

		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, args[0]);
	}	
}

//args[0]: idGDT, args[1]: 0(yaCreado), 1(recienCreado), -1(errorCreado), args[2]: arch
void MDJ_DAM_verificameSiArchivoFueBorrado(socket_connection * connection,char ** args/*char* pam1, char* pam2*/){
	estadoBorrado = atoi(args[1]);

	if(estadoBorrado == 0)
	{
		log_trace(logger,"El MDJ informa que se ha borrado el archivo %s por el GDT %s",args[2], args[0]);
		runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, args[0]);
	}
	else if (estadoBorrado == -1)
	{
		log_error(logger,"El archivo %s es inexistente",args[2]);
		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1,args[0]);
	}
	else
	{
		log_error(logger,"Ocurrio un error al querer borrar el archivo %s",args[2]);
		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, args[0]);	
	}
}

//args[0]: idGDT, args[1]: nomArch, args[2]: cantLineas
void crearArchivo(socket_connection* connection, char** args){
	log_info(logger,"Envio al MDJ la solicitud de creacion del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"crearArchivo",3, args[0], args[1], args[2]);
}

//args[0]: idGDT, args[1]: arch
void borrarArchivo(socket_connection* connection, char** args){
	log_info(logger,"Envio al MDJ la solicitud de borrado del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"borrarArchivo",2, args[0], args[1]);
}

void MDJ_DAM_obtenemeLosDatos(socket_connection * connection,char ** args){
int idGDT = atoi(args[0]); 
int cantBytes = atoi(args[1]);
int estado = atoi(args[2]);
char * path = args[3];
if (estado == -1)
{
log_error(logger,"Archivo %s inexistente",path);
}
else
{
log_trace(logger,"Se obtuvieron %i bytes  del archivo %s",cantBytes,path);	
}
}

//Comunicacion entre CPU-DAM para Cargar GDT
//args[0]: idGDT, args[1]: path, args[2]: 1(Dummy) ó 0(no Dummy)
void solicitudCargaGDT(socket_connection* connection, char ** args){
	char string_socket[2];
	sprintf(string_socket, "%i", connection->socket);
	log_trace(logger, "Voy a buscar el archivo %s para el GDT de id %s",args[1], args[0]);
	if(strcmp(args[2], "1") == 0)
		runFunction(socketMDJ, "validarArchivo",4, args[0], args[1],"0",args[2]);
	else
		runFunction(socketMDJ, "validarArchivo",4, args[0], args[1],string_socket,args[2]);
}

//args[0]: idGDT, args[1]: path, args[2]: 1(Dummy) ó 0(no Dummy)
void existeArchivo(socket_connection* socket, char** args){
	char string_socket[2];
	sprintf(string_socket, "%i", socket->socket);
	log_info(logger, "Archivo %s recibido", args[1]);
	runFunction(socketMDJ,"validarArchivo",4,args[0],args[1], string_socket, args[2]);
}

//args[0]: 1 -> si existe,   args[1]: socketCPU, args[2]: path, args[3]:idGDT, args[4]: 1(Dummy) ó 0(no Dummy)
//        -1 -> si no existe
void MDJ_DAM_existeArchivo(socket_connection* socket, char** args){
	int socketCPU = atoi(args[1]); // HAY MUCHAS CPUs
	log_info(logger, "Continua Su Ejecucion");
	if(socketCPU != 0)
		runFunction(socketCPU,"CPU_DAM_continuacionExistenciaAbrir",1,args[0]);
	else
		runFunction(socketFM9,"FM9_DAM_solicitudCarga",3,args[3], args[2], args[4]); // o cómo se llame, y le paso los argumentos que necesite
}

//args[0]: idGDT, args[1]: rutaArchivo
void solicitudDeFlush(socket_connection* connection, char** args)
{
	int idGDT = atoi(args[0]);
	char* rutaArchivo = args[1];
	
	char* string_transferSize;
	sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);

	runFunction(socketFM9,"DAM_FM9_obtenerArchivo",2, args[0], string_transferSize);
}

//args[0]: idGDT, args[1]: rutaArchivo, ...
void respuestaFlushFM9(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	char* rutaArchivo = args[1];

	//no sé qué funcion llama del MDJ
	runFunction(socketMDJ,"guardarDatos",2, args[0], args[1]);//más argumentos faltan
}

//args[0]: idGDT, args[1]: 1(ok) ó 0(error)
void respuestaFlushMDJ(socket_connection* connection, char** args){
	if(strcmp(args[1],"1") == 0)
		runFunction(socketSAFA,"DAM_SAFA_desbloquearDTB",1, args[0]);
	else
		runFunction(socketSAFA,"DAM_SAFA_pasarDTBAExit",1, args[0]);
}
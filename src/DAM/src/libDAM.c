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
void CPU_DAM_crearArchivo(socket_connection* connection, char** args){
	log_info(logger,"Envio al MDJ la solicitud de creacion del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"crearArchivo",3, args[0], args[1], args[2]);
}

//args[0]: idGDT, args[1]: arch
void CPU_DAM_borrarArchivo(socket_connection* connection, char** args){
	log_info(logger,"Envio al MDJ la solicitud de borrado del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"borrarArchivo",2, args[0], args[1]);
}

//Comunicacion entre CPU-DAM para Cargar GDT
//args[0]: idGDT, args[1]: path, args[2]: 1(Dummy) ó 0(noDummy)
void CPU_DAM_solicitudCargaGDT(socket_connection* connection, char ** args){
	pthread_t hilo;
	parametros* params = malloc(sizeof(parametros));
	params->idGDT = atoi(args[0]);
	strcpy(params->path, args[1]);
	strcpy(params->dummy, args[2]);
	log_trace(logger, "Voy a intentar cargar el archivo %s para el GDT de id %s",args[1], args[0]);	
	pthread_create(&hilo, NULL, (void*)&hiloCarga, params);
}

void hiloCarga(parametros* params){
	pthread_mutex_lock(&m_carga);
	ruta = malloc(strlen(params->path) + 1);
	strcpy(ruta, params->path);
	runFunction(socketMDJ, "obtenerDatos", 6, params->idGDT, params->path, "0", datosConfigDAM->transferSize, params->dummy, "1");
}

//args[0]: idGDT, args[1]: bytesLeidos, args[2]: estado, args[3]:path, args[4]: dummy, args[5]: primera o no
void MDJ_DAM_respuestaCargaGDT(socket_connection * connection,char ** args){
	int cantBytesLeidos = atoi(strlen(args[1]) + 1);
	int idGDT = atoi(args[0]); 
	char* bytes = args[1];
	int estado = atoi(args[2]);
	char * path = args[3];
	offsetAcumulado = cantBytesLeidos;

	if (estado == -1){
		log_error(logger,"Archivo %s inexistente",path);
		if(strcmp(args[4], "0") == 0)
			runFunction(socketSAFA,"DAM_SAFA_pasarDTBAExit",1, args[0]);
		else
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
	}
	else{
		log_trace(logger,"Se obtuvieron %i bytes  del archivo %s del MDJ",cantBytesLeidos, path);
		log_trace(logger,"Bytes: %s",bytes);
		if(cantBytesLeidos < datosConfigDAM->transferSize){
			runFunction(socketFM9,"DAM_FM9_cargarArchivo",5, args[0], bytes, "ultima", args[4],args[5]);
		}
		else{
			runFunction(socketFM9,"DAM_FM9_cargarArchivo",5, args[0], bytes, "sigue", args[4],args[5]);
		}	
	}
}

//lamada por el FM9
//args[0]: idGDT, args[1]: Por ahora ok o error, args[2]: 1(Dummy) ó 0(no Dummy), args[3]:"ultima"
//																						  "sigue"
//args[4]: primera o no, args[5]: pagina, args[6]:baseSegmento, args[7]:despl
void FM9_DAM_archivoCargadoCorrectamente(socket_connection* connection, char** args){

	char* estadoCarga = args[1];

	if(strcmp(args[4], "1") == 0){
		pagina = malloc(strlen(args[5]) + 1);
		strcpy(pagina, args[5]);
		baseSegmento = malloc(strlen(args[6]) + 1);
		strcpy(baseSegmento, args[6]);
		desplazamiento = malloc(strlen(args[7]) + 1);
		strcpy(desplazamiento, args[7]);
	}

	if(strcmp(estadoCarga, "ok") == 0 ){
		if(strcmp(args[3], "ultima") == 0){
			if(strcmp(args[2], "1") == 0)
				runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "ok", pagina, baseSegmento, desplazamiento);
			else{ 
				runFunction(socketSAFA, "aperturaArchivo", 2, args[0], ruta, pagina, baseSegmento, desplazamiento);
				runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB", 1, args[0]);
			}
			pthread_mutex_unlock(&m_carga);
		}
		else{
			char string_offset[2];
			sprintf(string_offset, "%i", offsetAcumulado);
			log_trace(logger,"Se guardaron los bytes del archivo %s en el FM9", ruta);
			runFunction(socketMDJ, "obtenerDatos", 5, args[0], args[2], string_offset, datosConfigDAM->transferSize, args[3]);
		}
	}
	else{//error
		if(strcmp(args[2], "1") == 0)
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
		else
			runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit", 1, args[0]);
		pthread_mutex_unlock(&m_carga);
	}
}

//args[0]: idGDT, args[1]: path
void CPU_DAM_existeArchivo(socket_connection* socket, char** args){
	char string_socket[2];
	sprintf(string_socket, "%i", socket->socket);
	log_info(logger, "Archivo %s recibido", args[1]);
	runFunction(socketMDJ,"validarArchivo",3,args[0],args[1], string_socket);
}

//args[0]: 1 -> si existe,   args[1]: socketCPU, args[2]: path, args[3]:idGDT
//		  -1 -> si NO existe
void MDJ_DAM_existeArchivo(socket_connection* socket, char** args){
	int socketCPU = atoi(args[1]); // HAY MUCHAS CPUs
	log_info(logger, "El GDT %s continua su ejecucion", args[3]);
	runFunction(socketCPU,"CPU_DAM_continuacionExistenciaAbrir",1,args[0]);
}

//args[0]: idGDT, args[1]: rutaArchivo
void CPU_DAM_solicitudDeFlush(socket_connection* connection, char** args)
{
	int idGDT = atoi(args[0]);
	char* rutaArchivo = args[1];
	
	char* string_transferSize;
	sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);

	runFunction(socketFM9,"DAM_FM9_obtenerArchivo",2, args[0], string_transferSize);
}

//args[0]: idGDT, args[1]: rutaArchivo, ...
void FM9_DAM_respuestaFlush(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	char* rutaArchivo = args[1];

	//no sé qué funcion llama del MDJ
	runFunction(socketMDJ,"guardarDatos",2, args[0], args[1]);//más argumentos faltan
}

//args[0]: idGDT, args[1]: 1(ok) ó 0(error)
void MDJ_DAM_respuestaFlush(socket_connection* connection, char** args){
	if(strcmp(args[1],"1") == 0)
		runFunction(socketSAFA,"DAM_SAFA_desbloquearDTB",1, args[0]);
	else
		runFunction(socketSAFA,"DAM_SAFA_pasarDTBAExit",1, args[0]);
}
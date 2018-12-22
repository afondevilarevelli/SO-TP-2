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

//args[0]: idGDT, args[1]: 0(yaCreado), 1(recienCreado), -1(errorCreado), args[2]: arch, args[3]: socketCPU
void MDJ_DAM_resultadoCreacionArchivo(socket_connection* conenction,char ** args){
	estadoCreacion = atoi(args[1]);
	int socketCPU = atoi(args[3]);

	if(estadoCreacion ==  1)
	{	
		log_trace(logger,"El MDJ informa que se ha creado el archivo %s por el GDT %s",args[2], args[0]);
		runFunction(socketCPU, "avisarTerminoClock", 0);
		runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, args[0]);
	}
	else
	{
		if(estadoCreacion == 0)
			log_error(logger,"Archivo %s previamente existia, el GDT %s va a ser abortado",args[2], args[0]);
		else
			log_error(logger,"Error al crear el archivo %s, el GDT %s va a ser abortado",args[2], args[0]);
		runFunction(socketCPU, "avisarTerminoClock", 0);
		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, args[0]);
	}	
}

//args[0]: idGDT, args[1]: 0(recienBorrado), -1(noBorrado), -2(errorBorrado), args[2]: arch,  args[3]: socketCPU
void MDJ_DAM_resultadoBorradoArchivo(socket_connection * connection,char ** args){
	estadoBorrado = atoi(args[1]);
	int socketCPU = atoi(args[3]);
	if(estadoBorrado == 0)
	{
		log_trace(logger,"El MDJ informa que se ha borrado el archivo %s por el GDT %s",args[2], args[0]);
		runFunction(socketCPU, "avisarTerminoClock", 0);
		runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB",1, args[0]);
	}
	else if (estadoBorrado == -1)
	{
		log_error(logger,"El archivo %s es inexistente, el GDT %s va a ser abortado",args[2], args[0]);
		runFunction(socketCPU, "avisarTerminoClock", 0);
		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1,args[0]);
	}
	else
	{
		log_error(logger,"Error al querer borrar el archivo %s, el GDT %s va a ser abortado",args[2], args[0]);
		runFunction(socketCPU, "avisarTerminoClock", 0);
		runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit",1, args[0]);	
	}
}

//args[0]: idGDT, args[1]: nomArch, args[2]: cantBytes
void CPU_DAM_crearArchivo(socket_connection* connection, char** args){
	char* string_socket = string_itoa(connection->socket);
	log_info(logger,"Envio al MDJ la solicitud de creacion del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"crearArchivo",4, args[0], args[1], args[2],string_socket);
}

//args[0]: idGDT, args[1]: arch
void CPU_DAM_borrarArchivo(socket_connection* connection, char** args){
	char* string_socket = string_itoa(connection->socket);
	log_info(logger,"Envio al MDJ la solicitud de borrado del archivo %s por parte del GDT de id %s",args[1], args[0]);
	runFunction(socketMDJ,"borrarArchivo",3, args[0], args[1], string_socket);
}

//Comunicacion entre CPU-DAM para Cargar GDT
//args[0]: idGDT, args[1]: path, args[2]: 1(Dummy) ó 0(noDummy)
void CPU_DAM_solicitudCargaGDT(socket_connection* connection, char ** args){
	pthread_t hilo;
	parametrosCarga* params = malloc(sizeof(parametrosCarga));
	strcpy(params->idGDT, args[0]);
	strcpy(params->path, args[1]);
	params->dummy[0] = *(args[2]);
	int socketCPU = connection->socket;
	params->socketCPU = string_itoa(socketCPU);
	log_trace(logger, "Voy a intentar cargar el archivo %s para el GDT de id %s",args[1], args[0]);	
	pthread_create(&hilo, NULL, (void*)&hiloCarga, params);
}

void hiloCarga(parametrosCarga* params){
	pthread_mutex_lock(&m_pedido);
	offsetAcumulado = 0;
	ruta = malloc(strlen(params->path) + 1);
	strcpy(ruta, params->path);
	char string_transferSize[3];
	sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);
	runFunction(socketMDJ, "obtenerDatos", 7, params->idGDT, params->path, "0", string_transferSize, params->dummy, "1",params->socketCPU);
	free(params);
}

//args[0]: idGDT, args[1]: bytesLeidos, args[2]: estado, args[3]:path, args[4]: dummy, args[5]: primera o no
//args[6]: socketCPU, args[7]: ultimaLectura(1) ó no(0)
void MDJ_DAM_respuestaCargaGDT(socket_connection * connection,char ** args){
	int cantBytesLeidos = strlen(args[1]) + 1;
	int idGDT = atoi(args[0]); 
	char* bytes = args[1];
	char * path = args[3];
	offsetAcumulado += cantBytesLeidos - 1;
	int socketCPU = atoi(args[6]);
	int estado = atoi(args[2]);

	if (estado < 0){
		log_error(logger,"Archivo %s inexistente",path);
		if(strcmp(args[4], "0") == 0)
			runFunction(socketSAFA,"DAM_SAFA_pasarDTBAExit",1, args[0]);
		else
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
	}
	else{
		log_trace(logger,"Se obtuvieron %i bytes  del archivo %s del MDJ",cantBytesLeidos-1, path);
		log_trace(logger,"Bytes: %s",bytes);
		if(strcmp(args[7], "1") == 0){
			runFunction(socketFM9,"DAM_FM9_cargarBuffer",6, args[0], bytes, "ultima", args[4],args[5],args[6]);
		}
		else{
			char* string_offset = string_itoa(offsetAcumulado);
			char string_transferSize[2];
			sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);
			runFunction(socketFM9,"DAM_FM9_cargarBuffer",6, args[0], bytes, "sigue", args[4],args[5], args[6]);
			runFunction(socketMDJ, "obtenerDatos", 7, args[0], ruta, string_offset, string_transferSize, args[4], "0",args[6]);
		}	
	}
}

//lamada por el FM9
//args[0]: idGDT, args[1]: Por ahora ok o error, args[2]: 1(Dummy) ó 0(no Dummy), 
// args[3]: pagina, args[4]:baseSegmento, args[5]:despl, args[6]: cantLineas, args[7]socketCPU
void FM9_DAM_archivoCargadoCorrectamente(socket_connection* connection, char** args){

	char* estadoCarga = args[1];
	int socketCPU = atoi(args[7]);

	if(strcmp(estadoCarga, "ok") == 0 ){

		if(*(args[2]) == '1')
			runFunction(socketSAFA, "avisoDamDTB", 6, args[0], "ok", args[3], args[4], args[5], args[6]);
		else{ 
			runFunction(socketSAFA, "aperturaArchivo", 6, args[0], ruta, args[3], args[4], args[5], args[6]);
			sleep(1);
			runFunction(socketCPU, "avisarTerminoClock", 0);
			runFunction(socketSAFA, "DAM_SAFA_desbloquearDTB", 1, args[0]);
			
		}
		if(ruta != NULL)
			free(ruta);
		pthread_mutex_unlock(&m_pedido);

	}
	else{//error
		if(*(args[2]) == '1')
			runFunction(socketSAFA, "avisoDamDTB", 2, args[0], "error");
		else{ 
			runFunction(socketSAFA, "DAM_SAFA_pasarDTBAExit", 1, args[0]);
			runFunction(socketCPU, "avisarTerminoClock", 0);
		}
		if(ruta != NULL)
			free(ruta);
		pthread_mutex_unlock(&m_pedido);
	}
}

//args[0]: idGDT, args[1]: path
void CPU_DAM_existeArchivo(socket_connection* socket, char** args){
	char string_socket[2];
	sprintf(string_socket, "%i", socket->socket);
	log_info(logger, "Archivo %s recibido", args[1]);
	runFunction(socketMDJ,"validarArchivo",3,args[0],args[1], string_socket);
}

//args[0]: 0 -> si existe,   args[1]: socketCPU, args[2]: path, args[3]:idGDT
//		  -1 -> si NO existe
void MDJ_DAM_existeArchivo(socket_connection* socket, char** args){
	int socketCPU = atoi(args[1]); // HAY MUCHAS CPUs
	log_info(logger, "El GDT %s continua su ejecucion", args[3]);
	runFunction(socketCPU,"CPU_DAM_continuacionExistenciaAbrir",1,args[0]);
}

//args[0]: idGDT, args[1]: pagina, args[2]:baseSegmento, args[3]:despl, args[4]: cantLineas, args[5]: arch
void CPU_DAM_solicitudDeFlush(socket_connection* connection, char** args)
{
	pthread_t hilo;
	parametrosFlush* params = malloc(sizeof(parametrosFlush));
	strcpy(params->idGDT, args[0]);
	strcpy(params->pagina, args[1]);
	strcpy(params->segmento, args[2]);
	strcpy(params->desplazamiento, args[3]);
	params->path = malloc(strlen(args[5]) + 1);
	strcpy(params->path, args[5]);
	strcpy(params->cantLineas, args[4]);
	
	pagina = malloc(strlen(args[1]) + 1);
	strcpy(pagina, args[1]);
	baseSegmento = malloc(strlen(args[2]) + 1);
	strcpy(baseSegmento, args[2]);
	desplazamiento = malloc(strlen(args[3]) + 1);
	strcpy(desplazamiento, args[3]);
	cantidadDeLineas = malloc(strlen(args[4]) + 1);
	strcpy(cantidadDeLineas, args[4]);

	int socketCPU = connection->socket;
	params->socketCPU = string_itoa(socketCPU);
	log_trace(logger, "Se va a hacer flush del archivo %s para el GDT de id %s",args[5], args[0]);	
	pthread_create(&hilo, NULL, (void*)&hiloFlush, params);
}

//args[0]: idGDT, args[1]: bytesLeidos, args[2]: estado,
//args[3]: socketCPU, args[4]: 1(ultimo) ó 0 (sigue)
void FM9_DAM_respuestaFlush(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int cantBytesLeidos = strlen(args[1]);
	char* string_offset = string_itoa(offsetAcumulado);
	char* string_bytes = string_itoa(cantBytesLeidos);

	log_info(logger, "Bytes leidos desde FM9: '%s'",args[1]);
	runFunction(socketMDJ,"guardarDatos",7,ruta, string_offset, string_bytes, args[1], args[4], args[3],args[0]);
}

//args[0]: idGDT, args[1]: bytesGuardados, args[2]: ok(0), noExisteArch(-1) ó error(-2),
//args[3]: socketCPU, args[4]: 1(ultimo) ó 0 (sigue)
void MDJ_DAM_respuestaFlush(socket_connection* connection, char** args){
	int cantBytesLeidos = strlen(args[1]);
	offsetAcumulado += cantBytesLeidos;
	int estado = atoi(args[2]);
	log_info(logger, "Se guardaron los siguientes bytes en MDJ '%s'",args[1]);
	if(strcmp(args[4], "1") == 0){ 
		if(strcmp(args[2],"0") == 0){
			log_trace(logger, "Se ha finalizado la peticion de FLUSH para el GDT %s",args[0]);
			runFunction(socketSAFA,"DAM_SAFA_desbloquearDTB",1, args[0]);
		}	
		else{
			log_error(logger, "La peticion de FLUSH para el GDT %s ha finalizado con error",args[1]);
			runFunction(socketSAFA,"DAM_SAFA_pasarDTBAExit",1, args[0]);
		}
		free(pagina);
		free(baseSegmento);
		free(desplazamiento);
		free(cantidadDeLineas);
		free(ruta);
		pthread_mutex_unlock(&m_pedido);
	}
	else{
		char* string_offset = string_itoa(offsetAcumulado);
		char string_transferSize[3];
		sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);
		runFunction(socketFM9,"DAM_FM9_obtenerDatosFlush",8,args[0], 
														string_offset, 
													    string_transferSize,
														pagina,
														baseSegmento,
														desplazamiento,
														cantidadDeLineas,
														args[3]);
	}
} 

void hiloFlush(parametrosFlush* params){
	pthread_mutex_lock(&m_pedido);
	offsetAcumulado = 0;
	ruta = malloc(strlen(params->path) + 1);
	strcpy(ruta, params->path);
	char string_transferSize[3];
	sprintf(string_transferSize, "%i", datosConfigDAM->transferSize);

	runFunction(socketFM9,"DAM_FM9_obtenerDatosFlush",8,params->idGDT, 
														"0", 
													    string_transferSize,
														params->pagina,
														params->segmento,
														params->desplazamiento,
														params->cantLineas,
														params->socketCPU);
	free(params->path);
	free(params);
}
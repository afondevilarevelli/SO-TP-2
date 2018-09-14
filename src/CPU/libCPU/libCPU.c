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
t_config_CPU* read_and_log_config(char* path) {

	char* ipS;
	char* ipD;

	log_info(logger, "Verificando que exista el archivo CPU.config");
        archivo_Config  = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);   }
        else
      {
       log_info(logger,"Se verifico que existe CPU.config"); }


	log_info(logger, "Voy a leer del archivo CPU.config");
    datosCPU = malloc(sizeof(t_config_CPU));

    ipS = string_new();
    string_append(&ipS,  config_get_string_value(archivo_Config,"IP_SAFA"));
    datosCPU->ipS = ipS;

	datosCPU->puertoS = config_get_int_value(archivo_Config,"S-AFA_PUERTO");

	ipD = string_new();
	string_append(&ipD,  config_get_string_value(archivo_Config,"IP_DAM"));
	datosCPU->ipD = ipD;

	datosCPU->puertoD = config_get_int_value(archivo_Config,"DAM_PUERTO");

	datosCPU->retardo= config_get_int_value(archivo_Config,"RETARDO");

	log_info(logger, "Voy a cargar los datos");
	log_info(logger, "IP_SAFA: %s", datosCPU->ipS);
	log_info(logger, "S-AFA_PUERTO: %d", datosCPU->puertoS);
	log_info(logger, "IP_DAM: %s", datosCPU->ipD);
	log_info(logger, "DAM_PUERTO: %d", datosCPU->puertoD);
	log_info(logger, "RETARDO: %d", datosCPU->retardo);
	log_info(logger, "Todos los Datos Fueron Cargados");

	config_destroy(archivo_Config);
	return datosCPU;

}

void* intentandoConexionConSAFA(int* socket){

printf("\nEl Socket SAFA Dio : %d \n",*socket);
if(*socket == -1){
	saliendo_por_error(*socket, "No Se Pudo Conectar Con SAFA", NULL);
}

log_info(logger,"Voy a hacer un handshake con SAFA");

runFunction(*socket,"CPU_SAFA_handshake",0);

sleep(5);

}

void* intentandoConexionConDAM(int* socket){

printf("\nEl Socket DAM Dio : %d \n",*socket);
if(*socket == -1){
	saliendo_por_error(*socket, "No Se Pudo Conectar Con DAM", NULL);
}

log_info(logger,"Voy a hacer un handshake con DAM");

runFunction(*socket,"CPU_DAM_handshake",0);

sleep(5);

}

void SAFA_CPU_handshake(socket_connection * connection, char ** args) {
	log_info(logger, "Handshake con SAFA");
}

void DAM_CPU_handshake(socket_connection * connection, char ** args) {
	log_info(logger, "Handshake con DAM");
}

void saliendo_por_error(int socket, char* error, void* buffer)
{
	if(buffer != NULL)
	{
		free(buffer);
	}

	log_error(logger, error);
	exit_gracefully(1);

}



void disconnect(){
  log_info(logger,"..Desconectado..");
}


void exit_gracefully(int return_nr) {

	free(datosCPU);
	disconnect();
	log_destroy(logger);

	exit(return_nr);

}

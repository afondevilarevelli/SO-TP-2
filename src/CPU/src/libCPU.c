#include "libCPU.h"

operacion_t parse(char* line){
    operacion_t retorno;

	if(line == NULL || string_equals_ignore_case(line, "")){
		retorno.palabraReservada = BLANCO;
        retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
		return retorno;
	}

	char* auxLine = string_duplicate(line);
	string_trim(&auxLine);
	char** split = string_n_split(auxLine, 4, " ");

	char* palReservada = split[0];
	// split[1] = parametro1
    // split[2] = parametro2
    // split[3] = parametro3		
	
	if(string_equals_ignore_case(palReservada, "abrir")){
		retorno.palabraReservada = ABRIR;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "concentrar")){
		retorno.palabraReservada = CONCENTRAR;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "asignar")){
		retorno.palabraReservada = ASIGNAR;
		retorno.p1 = split[1];
        retorno.p2 = split[2];
        retorno.p3 = split[3];
	} else if(string_equals_ignore_case(palReservada, "wait")){
		retorno.palabraReservada = WAIT;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "signal")){
		retorno.palabraReservada = SIGNAL;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "flush")){
		retorno.palabraReservada = FLUSH;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "close")){
		retorno.palabraReservada = CLOSE;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "crear")){
		retorno.palabraReservada = CREAR;
		retorno.p1 = split[1];
        retorno.p2 = split[2];
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "borrar")){
		retorno.palabraReservada = BORRAR;
		retorno.p1 = split[1];
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else if(string_equals_ignore_case(palReservada, "#")){
		retorno.palabraReservada = NUMERAL;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} else{
        fprintf(stderr, "No se encontro la palabra reservada <%s>\n", palReservada);
		return;
    }

	free(auxLine);
	return retorno;
}

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
	char* ipF;

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

	ipF = string_new();
	string_append(&ipF,  config_get_string_value(archivo_Config,"IP_FM9"));
	datosCPU->ipF = ipF;

	datosCPU->puertoF = config_get_int_value(archivo_Config,"FM9_PUERTO");

	datosCPU->retardo= config_get_int_value(archivo_Config,"RETARDO");

	log_info(logger, "Voy a cargar los datos");
	log_info(logger, "IP_SAFA: %s", datosCPU->ipS);
	log_info(logger, "S-AFA_PUERTO: %d", datosCPU->puertoS);
	log_info(logger, "IP_DAM: %s", datosCPU->ipD);
	log_info(logger, "DAM_PUERTO: %d", datosCPU->puertoD);
	log_info(logger, "IP_FM9: %s", datosCPU->ipF);
	log_info(logger, "FM9_PUERTO: %d", datosCPU->puertoF);
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

log_info(logger,"me conecto al SAFA");

runFunction(*socket,"identificarProcesoEnSAFA",1,"CPU");

sleep(1);
}

void* intentandoConexionConDAM(int* socket){

printf("\nEl Socket DAM Dio : %d \n",*socket);
if(*socket == -1){
	saliendo_por_error(*socket, "No Se Pudo Conectar Con DAM", NULL);
}

log_info(logger,"me conecto al DAM");

runFunction(*socket,"identificarProcesoEnDAM",1,"CPU");

sleep(1);
}

void* intentandoConexionConFM9(int* socket){

printf("\nEl Socket FM9 Dio : %d \n",*socket);
if(*socket == -1){
	saliendo_por_error(*socket, "No Se Pudo Conectar Con FM9", NULL);
}

log_info(logger,"me conecto al FM9");

runFunction(*socket,"identificarProcesoEnFM9",1,"CPU");

sleep(1);
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

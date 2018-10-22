#include "libCPU.h"

operacion_t parse(char* line){
    operacion_t retorno;

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
	} else if(string_equals_ignore_case(&palReservada[0], "#")){
		retorno.palabraReservada = NUMERAL;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
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
	log_error(logger, "No Se Pudo Conectar Con SAFA");
}

log_info(logger,"me conecto al SAFA");

runFunction(*socket,"identificarProcesoEnSAFA",1,"CPU");

sleep(1);
}

void* intentandoConexionConDAM(int* socket){

printf("\nEl Socket DAM Dio : %d \n",*socket);
if(*socket == -1){
	log_error(logger, "No Se Pudo Conectar Con DAM");
}

log_info(logger,"me conecto al DAM");

runFunction(*socket,"identificarProcesoEnDAM",1,"CPU");

sleep(1);
}

void* intentandoConexionConFM9(int* socket){

printf("\nEl Socket FM9 Dio : %d \n",*socket);
if(*socket == -1){
	log_error(logger, "No Se Pudo Conectar Con FM9");
}

log_info(logger,"me conecto al FM9");

runFunction(*socket,"identificarProcesoEnFM9",1,"CPU");

sleep(1);
}


void disconnect(){
  log_info(logger,"..Desconectado..");
}

//callable remote functions
//args[0]: idGDT, args[1]: rutaScript, args[2]: PC, args[3]: flagInicializacionGDT
void permisoConcedidoParaEjecutar(socket_connection * connection ,char** args){
	int idGDT = atoi(args[0]);
	log_trace(logger,"Ejecutando el GDT de id %d\n",idGDT);
	//muestro todo lo que recibio
	char* rutaScript = args[1];
	int programCounter = atoi(args[2]);
	int flagInicializado = atoi(args[3]);
	char string_id[2]; 
	sprintf(string_id, "%i", idCPU);

	log_info(logger, "La ruta Script es: %s", rutaScript);
	log_info(logger, "El Program Counter se encuentra en: %d", programCounter);
	log_info(logger, "El flag con el que inicia es: %d", flagInicializado);


	if (flagInicializado == 0) { //DTB-Dummy
		log_trace(logger,"Preparando la inicializacion de ejecucion del DTB Dummy\n");
		runFunction(socketDAM, "CPU_DAM_solicitudCargaGDT", 2,args[0], rutaScript);
		runFunction(socketSAFA, "finalizacionProcesamientoCPU",3, string_id, args[0], "bloquear");
	}
	else{
		scriptGDT* scriptGdt = verificarSiYaSeAbrioElScript(idGDT, rutaScript);
		int sentenciasEjecutadas = 0;
		while(sentenciasEjecutadas < quantum){
			operacion_t sentencia = obtenerSentenciaParseada(scriptGdt->scriptf);
			switch(sentencia.palabraReservada){
				case ABRIR:
					//algo
					break;
				case CONCENTRAR:
					//algo
					break; 
				case ASIGNAR:
					//algo
					break;
				case WAIT:
					//algo
					break;
			    case SIGNAL:
					//algo
					break;
				case FLUSH:
					//algo
					break;
				case CLOSE:
					//algo
					break;
				case CREAR:
					//algo
					break;
				case BORRAR:
					//algo
					break;
				case NUMERAL:
					//algo
					break;
				case FIN:
					runFunction(connection->socket, "finalizacionProcesamientoCPU",3, string_id, args[0], "finalizar" );
					break;
			}
			sleep(datosCPU->retardo);
			sentenciasEjecutadas++;
		}

	//cuando finaliza de ejecutar dicho proceso, le avisa al SAFA

	//runFunction(connection->socket, "finalizacionProcesamientoCPU",3, string_id, args[0], "continuar" );
	}
}

void establecerQuantumYID(socket_connection * connection ,char** args){
	quantum = atoi( args[0] ); 
	idCPU = atoi (args[1] );
	log_trace(logger,"QUANTUM = %i",quantum);
	log_trace(logger,"ID de CPU =  %i",idCPU);
}

operacion_t obtenerSentenciaParseada(FILE* script){
	operacion_t sentenciaParseada;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

	if( read = getline(&line, &len, script) != -1){
		sentenciaParseada = parse(line);
	}
	else{
		sentenciaParseada.palabraReservada = FIN;
	}

    if (line){ 
		log_trace(logger, "Sentencia: %s", line);
        free(line);
	}
	return sentenciaParseada;
}

FILE * abrirScript(char * scriptFilename)
{
  FILE * scriptf = fopen(strcat("../../../Pto_Montaje/Scripts/",scriptFilename), "r");
  if (scriptf == NULL)
  {
    log_error(logger, "Error al abrir el archivo %s: %s", scriptFilename, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return scriptf;
}

scriptGDT* verificarSiYaSeAbrioElScript(int idGDT, char* ruta){
	pthread_mutex_lock(&m_busqueda);
	idGDTScriptABuscar = idGDT;
	scriptGDT* s = list_find(listaScriptsGDT, (void*)closureBusquedaScript);
	pthread_mutex_unlock(&m_busqueda);
	if(s == NULL){ //el script no está abierto
		scriptGDT* nuevo = malloc(sizeof(scriptGDT));
		nuevo->idGDT = idGDT;
		nuevo->scriptf = abrirScript(ruta);
		pthread_mutex_lock(&m_listaScriptsGDT);
		list_add(listaScriptsGDT, nuevo);
		pthread_mutex_unlock(&m_listaScriptsGDT);
		return nuevo;
	}
	else{
		return s;
	}
}

bool closureBusquedaScript(scriptGDT* el){
	return el->idGDT == idGDTScriptABuscar;
}
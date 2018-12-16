#include "libCPU.h"

void destruirOperacion(operacion_t op){
	if(op.p1 != NULL)
		free(op.p1);
	if(op.p2 != NULL)
		free(op.p2);
	if(op.p3 != NULL)
		free(op.p3);
}

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
	} else if( palReservada[0] =='#' ){
		retorno.palabraReservada = NUMERAL;
		retorno.p1 = NULL;
        retorno.p2 = NULL;
        retorno.p3 = NULL;
	} 

	retorno.ultimaSentencia = false;
	free(split[0]);
	free(split);
	free(auxLine);
	return retorno;
}

void configure_loggerCPU() {

	char * nombrePrograma = "CPU.log";
	char * nombreArchivo = "CPU";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se Creo El Archivo Log de CPU");

}

void close_loggerCPU() {
	log_info(logger, "Cierro log de CPU");
	log_destroy(logger);
}

//CONFIG
t_config_CPU* read_and_log_configCPU(char* path) {

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

void* intentandoConexionConSAFA(int socket){

printf("\nEl Socket SAFA Dio : %d \n",socket);
if(socket == -1){
	log_error(logger, "No Se Pudo Conectar Con SAFA");
}

log_info(logger,"me conecto al SAFA");

//importante
runFunction(socket,"identificarProcesoEnSAFA",1,"CPU");

sleep(1);
}

void* intentandoConexionConDAM(int socket){

printf("\nEl Socket DAM Dio : %d \n",socket);
if(socket == -1){
	log_error(logger, "No Se Pudo Conectar Con DAM");
}

log_info(logger,"me conecto al DAM");

runFunction(socket,"identificarProcesoEnDAM",1,"CPU");

sleep(1);
}

void* intentandoConexionConFM9(int socket){

printf("\nEl Socket FM9 Dio : %d \n",socket);
if(socket == -1){
	log_error(logger, "No Se Pudo Conectar Con FM9");
}

log_info(logger,"me conecto al FM9");

runFunction(socket,"identificarProcesoEnFM9",1,"CPU");

sleep(1);
}


void disconnect(){
  log_info(logger,"..Desconectado..");
}

void pausarPlanificacion(socket_connection * connection ,char** args){
	 pthread_mutex_trylock(&m_puedeEjecutar);
}

void continuarPlanificacion(socket_connection * connection ,char** args){
	pthread_mutex_unlock(&m_puedeEjecutar);
}

//args[0]: 1 ó 0
void ejecucionAbrir(socket_connection* connection, char** args){

	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo)
		archivoAbiertoAbrir = true;
	else
		archivoAbiertoAbrir = false;


	sem_post(&sem_esperaAbrir);
}

//args[0]: -1 -> si no existe, 
//			1 -> si existe
void ejecucionAbrirExistencia(socket_connection* connection, char** args){

	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo == 1)
		archivoExistente = true;
	else
		archivoExistente = false;

	sem_post(&sem_esperaAbrir);
}

//args[0]: 1 ó 0
void ejecucionAsignar(socket_connection* connection, char** args){
	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo)
		archivoAbiertoAsignar = true;
	else
		archivoAbiertoAsignar = false;

	sem_post(&sem_esperaEjecucion);
}

//args[0]: estadoArchivo
void ejecucionFlush(socket_connection* connection, char** args){

	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo == 1){
		archivoAbiertoFlush = true;
	}
	else{
		archivoAbiertoFlush = false;
	}

	sem_post(&sem_esperaEjecucion);

}

//args[0]: 1 ó 0
void ejecucionWait(socket_connection* connection, char** args){
	int resultado = atoi(args[0]);

	if(resultado)
		resultadoWaitOk = true;
	else
		resultadoWaitOk = false;

	sem_post(&sem_esperaEjecucion);
}

//args[0]: estadoArchivo
void ejecucionClose(socket_connection* connection, char** args){

	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo == 1){

		archivoAbiertoClose = true;
	}
	else {
		archivoAbiertoClose = false;
	}
	sem_post(&sem_esperaClose);

}

//El resultado del FM9
//args[1]: estadoArchivo
void finalizacionClose(socket_connection* connection, char** args){
	int estadoSituacionArchivo = atoi(args[0]);

	if(estadoSituacionArchivo)
		resultadoCloseOk = true;
	else	
		resultadoCloseOk = false;

	sem_post(&sem_esperaClose);
}

//callable remote functions
//args[0]: idGDT, args[1]: rutaScript, args[2]: PC, args[3]: flagInicializacionGDT, args[4]: quantum a ejecutar,
//args[5]: pagina, args[6]: segmento, args[7]: desplazamiento
void permisoConcedidoParaEjecutar(socket_connection * connection ,char** args){
	pthread_t hilo;
	parametros* params = malloc(sizeof(parametros));
	params->idGDT = atoi(args[0]);
	strcpy(params->rutaScript, args[1]);
	params->programCounter = atoi(args[2]);
	params->flagInicializado = atoi(args[3]);
	params->quantumAEjecutar = atoi(args[4]);
	params->pagina = atoi(args[5]);
	params->segmento = atoi(args[6]);
	params->desplazamiento = atoi(args[7]);
	pthread_create(&hilo, NULL, (void*)&permisoDeEjecucion, params);
	//pthread_detach(hilo);
}

void permisoDeEjecucion(parametros* params){
	pthread_t hiloAbrir;
	pthread_t hiloEjecucion;
	pthread_attr_t attr;
	int cantComentarios = 0;
	int idGDT = params->idGDT;
	int quantumAEjecutar = params->quantumAEjecutar;
	log_trace(logger,"Ejecutando el GDT de id %d\n",idGDT);
	//muestro todo lo que recibio
	char* rutaScript = params->rutaScript;
	int programCounter = params->programCounter;
	int flagInicializado = params->flagInicializado;
	int pagina = params->pagina;
	int segmento = params->segmento;
	int desplazamiento = params->desplazamiento;
	char string_id[2]; 
	sprintf(string_id, "%i", idCPU);
	char string_idGDT[2]; 
	sprintf(string_idGDT, "%i", idGDT);

	log_info(logger, "La ruta Script es: %s", rutaScript);
	log_info(logger, "El Program Counter se encuentra en: %d", programCounter);
	log_info(logger, "El flag de inicializacion es: %d", flagInicializado);
	log_info(logger, "El quantum a ejecutar es: %d", quantumAEjecutar);

	operacion_t sentencia;
	if (flagInicializado == 0) { //DTB-Dummy
		log_trace(logger,"Preparando la inicializacion de ejecucion del DTB Dummy\n");
		runFunction(socketDAM, "CPU_DAM_solicitudCargaGDT", 2,string_idGDT, rutaScript, "1");
		runFunction(socketSAFA, "finalizacionProcesamientoCPU",7, string_id, string_idGDT, "0", "bloquear", "0","0", "1");
		return;
	}
	else{
		int sentenciasEjecutadas = 0;
		while(sentenciasEjecutadas < quantumAEjecutar){
			sleep(datosCPU->retardo);
			pthread_mutex_lock(&m_puedeEjecutar);
			sentencia = obtenerSentenciaParseada(rutaScript, programCounter);
			pthread_mutex_unlock(&m_puedeEjecutar);
			char string_sentEjecutadas[2];
			char string_quantumAEjecutar[2];

			switch(sentencia.palabraReservada){
				case ABRIR:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					runFunction(socketSAFA, "CPU_SAFA_verificarEstadoArchivo", 4, string_id, string_idGDT, "abrir", sentencia.p1);
					runFunction(socketDAM, "CPU_DAM_existeArchivo", 2,string_idGDT, rutaScript);
					pthread_attr_init(&attr);
    				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
					pthread_create(&hiloAbrir, &attr, (void*)&funcionHiloAbrir, NULL);
					pthread_attr_destroy(&attr);
					pthread_join(hiloAbrir, NULL);

					if(!archivoExistente){

						log_trace(logger,"El archivo %s no existe y se va a abortar el GDT", rutaScript);
						runFunction(socketSAFA, "terminoClock", 1, string_id);
						runFunction(socketSAFA, "CPU_SAFA_pasarDTBAExit", 1, string_idGDT);
						destruirOperacion(sentencia);
						pthread_detach(hiloAbrir);
						return;
					}
					else{
						if(!archivoAbiertoAbrir){

							sentenciasEjecutadas++;
							sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);
							log_trace(logger,"El archivo no se encuentra abierto por el GDT");
							if(!sentencia.ultimaSentencia)
								runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7, string_id, string_idGDT, string_sentEjecutadas, "bloquear", "1", "1", "0");
							else
								runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7, string_id, string_idGDT, string_sentEjecutadas, "finalizar", "1", "1", "0");
							runFunction(socketDAM, "CPU_DAM_solicitudCargaGDT", 2,string_idGDT, rutaScript, "0");
							runFunction(socketSAFA, "terminoClock", 1, string_id);
							destruirOperacion(sentencia);
							pthread_detach(hiloAbrir);
							return;
						}
						runFunction(socketSAFA, "terminoClock", 1, string_id);
					}

					break;
				case CONCENTRAR:
					sleep(datosCPU->retardo);
					break; 
				case ASIGNAR:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					pthread_attr_init(&attr);
    				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
					runFunction(socketSAFA, "CPU_SAFA_verificarEstadoArchivo",4,string_id, string_idGDT, "asignar", sentencia.p1);
					pthread_create(&hiloEjecucion, &attr, (void*)&funcionHilo, "ASIGNAR");
					pthread_attr_destroy(&attr);
					pthread_join(hiloEjecucion, NULL);

					if(archivoAbiertoAsignar){
						log_trace(logger,"Se Enviaran Los Datos Necesarios A FM9 para actualizar los datos del archivo %s", sentencia.p1);
						runFunction(socketFM9, "CPU_FM9_actualizarLosDatosDelArchivo", 4, string_idGDT, sentencia.p1,
																			   					        sentencia.p2,
																			                            sentencia.p3);	
						runFunction(socketSAFA, "terminoClock", 1, string_id);
						pthread_detach(hiloEjecucion);
					//No se bloquea ya que el enunciado dice que cada vez que interfiera el DAM se lo bloquea al proceso
					//Al ser la comunicacion entre CPU y FM9 nadie interviene
					}
					else{
						log_error(logger, "El archivo %s no se encuentra abierto por el GDT %s, en consecuencia se va a abortar a dicho GDT", sentencia.p1, string_idGDT);
						runFunction(socketSAFA, "terminoClock", 1, string_id);
						runFunction(socketSAFA, "CPU_SAFA_pasarDTBAExit", 1,string_idGDT);
						destruirOperacion(sentencia);
						pthread_detach(hiloEjecucion);
						return;
					}
					break;
				case WAIT:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					pthread_attr_init(&attr);
    				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
					//args[0] idGDT para Bloquear
					pthread_create(&hiloEjecucion, &attr, (void*)&funcionHilo, "WAIT");
					runFunction(socketSAFA, "waitRecurso",3,string_id, string_idGDT, sentencia.p1);
					pthread_attr_destroy(&attr);
					//Intento Con el JOIN pero sigue bloqueado
					pthread_join(hiloEjecucion, NULL);

					if(!resultadoWaitOk){
						log_trace(logger,"Se va a bloquear al GDT");
						sentenciasEjecutadas++;
						sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);
						if(!sentencia.ultimaSentencia)
							runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7, string_id, string_idGDT, string_sentEjecutadas, "bloquear", "0", "0", "0");
						else
							runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7, string_id, string_idGDT, string_sentEjecutadas, "finalizar", "0", "0", "0");
						destruirOperacion(sentencia);
						runFunction(socketSAFA, "terminoClock", 1, string_id);
						pthread_detach(hiloEjecucion);
						return ;
					}	
					runFunction(socketSAFA, "terminoClock", 1, string_id);
					log_trace(logger,"Recurso asignado");
					break;	
			    case SIGNAL:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					runFunction(socketSAFA, "signalRecurso",3,string_id, string_idGDT, sentencia.p1);
					log_trace(logger,"Recurso liberado");
					runFunction(socketSAFA, "terminoClock", 1, string_id);	
					break;		
				case FLUSH:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					sprintf(string_quantumAEjecutar, "%i", quantumAEjecutar);
					runFunction(socketSAFA, "CPU_SAFA_verificarEstadoArchivo", 4, string_id, string_idGDT, "flush", sentencia.p1);

					pthread_attr_init(&attr);
    				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
					pthread_create(&hiloEjecucion, &attr, (void*)&funcionHilo, "FLUSH");
					pthread_attr_destroy(&attr);
					pthread_join(hiloEjecucion, NULL);

					if(archivoAbiertoFlush){
						sentenciasEjecutadas++;
						sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);

						if(!sentencia.ultimaSentencia)
							runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7,string_id, string_idGDT, string_sentEjecutadas ,"bloquear", "0", "1", "0");
						else
							runFunction(socketSAFA, "finalizacionProcesamientoCPU", 7,string_id, string_idGDT, string_sentEjecutadas ,"finalizar", "0", "1", "0");
						runFunction(socketDAM, "CPU_DAM_solicitudDeFlush", 2, string_idGDT, rutaScript);
						destruirOperacion(sentencia);
						pthread_detach(hiloEjecucion);
					}
					else{
						log_error(logger, "El Archivo Solicitado: %s No Se Encuentra Abierto", rutaScript);
						runFunction(socketSAFA, "CPU_SAFA_pasarDTBAExit", 1, string_idGDT);
						destruirOperacion(sentencia);
						pthread_detach(hiloEjecucion);
					}
					runFunction(socketSAFA, "terminoClock", 1, string_id);
					return;
				case CLOSE:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					sprintf(string_quantumAEjecutar, "%i", quantumAEjecutar);
					runFunction(socketSAFA, "CPU_SAFA_verificarEstadoArchivo", 4, string_id, string_idGDT, "close", sentencia.p1);
					runFunction(socketFM9, "CPU_FM9_cerrarElArchivo", 2, string_idGDT, sentencia.p1);

					sentenciasEjecutadas++;
					sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);

					pthread_attr_init(&attr);
    				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
					pthread_create(&hiloEjecucion, &attr, (void*)funcionHiloClose, NULL);
					pthread_attr_destroy(&attr);
					pthread_join(hiloEjecucion, NULL);

					if(!archivoAbiertoClose){
						log_error(logger, "El Archivo Solicitado: %s No Se Encuentra Abierto", sentencia.p1);
						runFunction(socketSAFA, "CPU_SAFA_pasarDTBAExit", 1, string_idGDT);
						destruirOperacion(sentencia);
						pthread_detach(hiloEjecucion);
						runFunction(socketSAFA, "terminoClock", 1, string_id);
						return;
					}
					else{
						if(resultadoCloseOk){ 
							log_trace(logger,"Se ha cerrado el archivo %s correctamente", sentencia.p1);
							pthread_detach(hiloEjecucion); 
							runFunction(socketSAFA, "terminoClock", 1, string_id);
						}
						else{
							log_error(logger, "Error al cerrar el archivo %s", sentencia.p1);
							runFunction(socketSAFA, "CPU_SAFA_pasarDTBAExit", 1, string_idGDT);
							destruirOperacion(sentencia);
							pthread_detach(hiloEjecucion);
							runFunction(socketSAFA, "terminoClock", 1, string_id);
							return;
						}
					}

					break;
				case CREAR:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					//Aca se incrementa el PC y SE, al final para ver si mas adelante continua o aborta
					sentenciasEjecutadas++;
					sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);
					//args[0] idGDT para Bloquear
					if(!sentencia.ultimaSentencia)
						runFunction(socketSAFA, "finalizacionProcesamientoCPU",7,string_id, string_idGDT,string_sentEjecutadas,"bloquear", "1","1", "0");
					else
						runFunction(socketSAFA, "finalizacionProcesamientoCPU",7,string_id, string_idGDT,string_sentEjecutadas,"finalizar", "1","1", "0");
					runFunction(socketDAM, "CPU_DAM_crearArchivo", 3, string_idGDT,sentencia.p1, sentencia.p2);
					destruirOperacion(sentencia);
					runFunction(socketSAFA, "terminoClock", 1, string_id);
					return ;
				case BORRAR:
					runFunction(socketSAFA, "inicioClock", 1, string_id);
					sentenciasEjecutadas++;
					string_sentEjecutadas[2];
					sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);
					//args[0] idGDT para Bloquear
					if(!sentencia.ultimaSentencia)
						runFunction(socketSAFA, "finalizacionProcesamientoCPU",7,string_id, string_idGDT,string_sentEjecutadas,"bloquear", "1", "1", "0"); 
					else
						runFunction(socketSAFA, "finalizacionProcesamientoCPU",7,string_id, string_idGDT,string_sentEjecutadas,"finalizar", "1", "1", "0"); 
					runFunction(socketDAM, "CPU_DAM_borrarArchivo", 2, string_idGDT,sentencia.p1);
					destruirOperacion(sentencia);
					runFunction(socketSAFA, "terminoClock", 1, string_id);
					return ;
			}

			programCounter++;
			if(sentencia.ultimaSentencia){
				log_trace(logger, "El GDT de id %d FINALIZA", idGDT);			
				runFunction(socketSAFA, "finalizacionProcesamientoCPU",7, string_id, string_idGDT,"0", "finalizar", "0", "0", "0");
				break;
			}

			if(sentencia.palabraReservada == NUMERAL)
				cantComentarios++;
			else
				sentenciasEjecutadas++;
					
		}

		if(!sentencia.ultimaSentencia){ 
			log_trace(logger, "El GDT de id %d finaliza su procesamiento en la CPU", idGDT);
			char string_sentEjecutadas[2];
			sprintf(string_sentEjecutadas, "%i", sentenciasEjecutadas+cantComentarios);
			runFunction(socketSAFA, "finalizacionProcesamientoCPU",7, string_id,string_idGDT, string_sentEjecutadas, "continuar", "0", "0", "0");
		}
		
	}
}

void establecerQuantumYID(socket_connection * connection ,char** args){
	quantum = atoi( args[0] ); 
	idCPU = atoi (args[1] );
	log_trace(logger,"QUANTUM = %i",quantum);
	log_trace(logger,"ID de CPU =  %i",idCPU);
}

//SOLO PARA LOS SCRIPTS QUE CREAMOS NOSOTROS
operacion_t obtenerSentenciaParseada(char* script,int programCounter){
	operacion_t sentenciaParseada;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
	int i;


	FILE* arch = abrirScript(script);
	for(i=0; i<programCounter; i++){
		getline(&line, &len, arch);
	}
	
	if( read = getline(&line, &len, arch) != -1){
		log_trace(logger, "Sentencia: %s", line);
		sentenciaParseada = parse(line);
		int prevPos = ftell(arch);
		
		read = getline(&line, &len, arch);
		if( read == -1){
			sentenciaParseada.ultimaSentencia = true;
		}
			fseek(arch, prevPos, SEEK_SET);
	}
	else{
		sentenciaParseada.palabraReservada == 500;
		log_error(logger, "El GDT ya no tiene mas sentencias ");
	}

    if (line){ 
        free(line);
	}
	fclose(arch);
	return sentenciaParseada;
}

FILE * abrirScript(char * scriptFilename)
{
  char* ruta = malloc(100*sizeof(char));
  strcpy(ruta, "../../Scripts/");
  strcat(ruta,scriptFilename);
  
  FILE * scriptf = fopen(ruta, "r");
  if (scriptf == NULL)
  {
    log_error(logger, "Error al abrir el archivo %s", scriptFilename);
    exit(EXIT_FAILURE);
  }
  
  free(ruta);
  return scriptf;
} 

void funcionHiloAbrir(){
	sem_wait(&sem_esperaAbrir);
	sem_wait(&sem_esperaAbrir);
	log_trace(logger, "Continuando Ejecucion Abrir");
}

void funcionHiloClose(){
	sem_wait(&sem_esperaClose);
	sem_wait(&sem_esperaClose);
	log_trace(logger, "Continuando Ejecucion Close");
}

void funcionHilo(char* tipoEjecucion){

	sem_wait(&sem_esperaEjecucion);
	log_trace(logger, "Continuando Ejecucion: %s", tipoEjecucion);
}

//Esta funcion es la posta, pero para los Scripts creados por nosotros usamos la funcion obtenerSentenciaParseada
//que está más arriba
/*
operacion_t obtenerSentenciaParseada(char* nomArch,int pc){
	operacion_t sentenciaParseada;
	int i = 0;
	int indice = 0;
	int indLinea = 0;
	char linea[100];
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	int bloques[50];
	int j = 0;
	int k;
	char bloque[3];
	int cant = 0;
	char scriptCompleto[1000];
	int contadorScript = 0;
	int c;
	char* ruta;

	FILE* arch = abrirArchivoScript(nomArch);
	getline(&line, &len, arch);
	if( read = getline(&line, &len, arch) != -1){
		while(line[i] != '['){			
			i++;
		}
		i++;

		while(line[i] != ']'){ 
			while(line[i] != ','){
				if(line[i] == ']'){
					break;
				}
				else{
					bloque[j] = line[i];
					i++;
					j++;
				}	
			}
			if(line[i] != ']'){ 
				bloques[cant] = atoi(bloque);
				bloque[0] = '\0';
				bloque[1] = '\0';
				bloque[2] = '\0';
				cant++;
				j = 0;
				i++;
			}
		}

		bloques[cant] = atoi(bloque);
	} // bloques ya obtenidos, tiene ( cant+1 ) bloques

	if (line){ 
        free(line);
	}
	fclose(arch);

	for(k = 0; k <= cant; k++){
		FILE* bf = abrirArchivoBloque(bloques[k]);
		if(bf!=NULL){
			while( ( c=fgetc(bf) ) != EOF){
				scriptCompleto[contadorScript] = c;
				contadorScript++;
			}
		fclose(bf);
		}
	}
	i=0;
	while(i<pc){
		while(scriptCompleto[indice] != '\n'){
			indice++;
		}
		indice++;
		i++;
	}

	while(scriptCompleto[indice] != '\n'){
		linea[indLinea] = scriptCompleto[indice];
		indLinea++;
		indice++;
	}

	linea[indLinea] = '\0';

	if(linea[0] != '\n'){
		log_trace(logger, "Sentencia: %s", linea);
		sentenciaParseada = parse(linea);
		if(scriptCompleto[indice+1] != '#' && scriptCompleto[indice+1] != 'a' && scriptCompleto[indice+1] != 'c' &&
		   scriptCompleto[indice+1] != 'w' && scriptCompleto[indice+1] != 's' && scriptCompleto[indice+1] != 'f' &&
		   scriptCompleto[indice+1] != 'b'){
			sentenciaParseada.ultimaSentencia = true;
		}
		else{
			sentenciaParseada.ultimaSentencia = false;
		}
	}
	else{
		sentenciaParseada.palabraReservada = 500;
	}
	return sentenciaParseada;
}
*/

FILE * abrirArchivoScript(char * nomArchivo)
{
  char* ruta = malloc(100*sizeof(char));
  strcpy(ruta, "../../fifa-entrega-Scripts/Archivos/scripts/");
  strcat(ruta,nomArchivo);
  
  FILE * scriptf = fopen(ruta, "r");
  if (scriptf == NULL)
  {
	  printf("Error al abrir archivo script\n");
    //log_error(logger, "Error al abrir el archivo %s", nomArchivo);
    exit(EXIT_FAILURE);
  }
  
  free(ruta);
  return scriptf;
}

FILE * abrirArchivoBloque(int numBloque)
{
  char bloque[5];
  sprintf(bloque, "%i", numBloque);
  char* ruta = malloc(100*sizeof(char));
  char* ext = ".bin";
  strcpy(ruta, "../../fifa-entrega-Scripts/Bloques/");
  strcat(ruta,bloque);
  strcat(ruta, ext);
  
  FILE * scriptf = fopen(ruta, "r");
  if (scriptf == NULL)
  {
	  printf("Error al abrir archivo bloque\n");
    //log_error(logger, "Error al abrir el archivo del bloque %d", numBloque);
    exit(EXIT_FAILURE);
  }
  
  free(ruta);
  return scriptf;
}
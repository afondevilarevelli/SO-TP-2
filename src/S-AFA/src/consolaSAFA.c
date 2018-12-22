#include "consolaSAFA.h"

void consolaSAFA(){
	generadorDeIds = 1;
	linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE

	do{

		free(linea);
		linea = readline(">");
		char* p1 = strtok(linea,espaBlan);    // token que apunta al primer parametro ( la palabra reservada )
		char* p2 = strtok(NULL,espaBlan);	 // token que apunta al segundo parametro

		if( p1 == NULL )
			continue;
		else
		if(strcmp(p1,"ejecutar") == 0 && p2 != NULL)
		{		
			ejecutar(p2);
		}
		else
		if(strcmp(p1,"finalizar") == 0 && p2 != NULL)
		{
            int id = atoi(p2); //convertir p2 a int
			finalizar(id);
		}
		else
		if(strcmp(p1,"status") == 0 )
		{     
            if(p2 == NULL){
                log_trace(logger,"Estado de las colas:\n");
                status(0);
            }
            else{
                int id = atoi(p2);	//convertir p2 a int
			    log_trace(logger,"Datos del DTB del G.DT de id = %d\n", id);
			    status(id);
            }
		}
		else
		if(strcmp(p1,"metricas") == 0 )
		{
			log_trace(logger,"Se detallaran las metricas:\n");
			if(p2 == NULL){ 
                metricas(0);
            }
            else{
                int id = atoi(p2);	//convertir p2 a int
			    metricas(id);
            }
		}
		else if(strcmp(p1,"pausar") == 0 && p2 == NULL){
			pausarPlanificacion();
			log_trace(logger,"Se ha pausado la planificacion");
		}
		else if(strcmp(p1,"continuar") == 0 && p2 == NULL){
			continuarPlanificacion();
			log_trace(logger,"Se va a continuar con la planificacion");
		}
		else
		{
			debeContinuar = strcmp(linea, "salir");
			if(debeContinuar) log_trace(logger,"comando no reconocido\n");
		}

	}while(debeContinuar);

	cerrarPrograma();

	exit(0);

	return;
}

void pausarPlanificacion(){
	list_iterate(listaCPUs, (void*)&iteracionPausarCPUs);
	pthread_mutex_trylock(&m_puedePlanificar);
}

void continuarPlanificacion(){
	pthread_mutex_unlock(&m_puedePlanificar);
	list_iterate(listaCPUs, (void*)&iteracionContinuarCPUs);
}

void iteracionPausarCPUs(CPU* cpu){
	runFunction(cpu->socket,"SAFA_CPU_pausarPlanificacion",0);
}

void iteracionContinuarCPUs(CPU* cpu){
	runFunction(cpu->socket,"SAFA_CPU_continuarPlanificacion",0);
}

void ejecutar(char* rutaSc){	
	if(estadoCorrupto) {
		log_error(logger,"SAFA en estado corrupto, no se admiten nuevos GDT´S");
	}else{
		char* nom = string_new();
		string_append(&nom, "/scripts/");
		string_append(&nom, rutaSc);
		log_trace(logger,"Se hace ingreso del script escriptorio de la ruta %s para su futura ejecucion\n", nom);
		archivo* archScript = malloc(sizeof(archivo));
		archScript->nombre = nom;
		archScript->pagina = -1;
		archScript->baseSegmento = -1;
		archScript->cantLineas = 0;
		archScript->desplazamiento = -1;
		DTB* dtb = malloc(sizeof(DTB));
    	dtb->id = generadorDeIds;
    	generadorDeIds++;
    	dtb->script = archScript;
    	dtb->PC = 0;
    	dtb->flagInicializado = 1;
    	dtb->archivosAbiertos = list_create();
    	dtb->status = NEW;
		dtb->quantumFaltante = 0;
		dtb->cantSentEsperadasEnNew = 0;
		dtb->cantIOs = 0;

		encolarDTB(colaNew, dtb, m_colaNew);
		sem_post(&cantProcesosEnNew);
	}			
}

void finalizar(int idGDT){	
	DTB * dtb = buscarDTB(listaEjecutando, idGDT);
	if(dtb != NULL){ //está ejecutando  
			dtb->status = FINISHED;
			log_info(logger,"Se ha finalizado el GDT de id %d", idGDT);
	} 
	else{ //no está ejecutando
		dtb = quitarDTBDeSuListaActual(idGDT);
		if(dtb != NULL){
			log_info(logger,"Se ha finalizado el GDT de id %d", idGDT);
			finalizarDTB(dtb);
		} else{
			printf("No se ha encontrado un proceso con el id %d en el sistema\n",idGDT);
		}
	}	
}

void status(int idGDT){ 

	if(idGDT == 0){ // funcion status sin parametro
	    log_trace(logger,"La Info De Los DTB En Cada Cola Son:\n");
	    log_trace(logger,"Cola New:");
		buscarDTBEnColasMostrandoInfo(colaNew);
		log_trace(logger,"Cola Ready:");
		buscarDTBEnColasMostrandoInfo(colaReady);
		log_trace(logger,"Cola Blocked:");
		buscarDTBEnColasMostrandoInfo(colaBloqueados);
		log_trace(logger,"Cola Finished:");
		buscarDTBEnColasMostrandoInfo(colaFinalizados);

	    }
	    else{ //funcion status con parametro

	    	statusDTB(idGDT);
	    	//return 0;
	    }
}

//FALTA IMPLEMENTAR
void metricas(int idGDT){	
	float tiempoRespProm = tiempoRespuestaPromedio();
	float porcSentDiego;
	float cantExit = calcularMetricaExit();
	if(cantSentenciasEjecutadas != 0)
		porcSentDiego = cantSentConDiego*100/cantSentenciasEjecutadas;
	else
		porcSentDiego = 0;
	log_info(logger,"Cant.de sentencias ejecutadas prom. del sistema que usaron a El Diego: %d", cantSentConDiego);
	log_info(logger,"Porcentaje de las sentencias ejecutadas promedio que fueron a El Diego: %.2f por ciento", porcSentDiego);
	log_info(logger,"Cant. de sentencias ejecutadas prom. del sistema para que un DTB termine en la cola EXIT: %.2f", cantExit);
	log_info(logger,"Tiempo de Respuesta promedio del Sistema: %.9f segs"/**10^9 segs"*/,tiempoRespProm);
	 
	if(idGDT != 0){ 
		DTB* dtb = buscarDTBEnElSistema(idGDT);
		if(dtb != NULL)
			log_trace(logger,"Cant. de sentencias ejecutadas que esperó el DTB de id %d en la cola NEW: %d", idGDT, dtb->cantSentEsperadasEnNew);
		else
			log_info(logger, "No existe un GDT con id %d en el sistema", idGDT);
	}
}

float tiempoRespuestaPromedio(){
	float time = 0;
	int i;
	int tamanio = list_size(tiemposDeRespuestas);
	pthread_mutex_lock(&m_tiempoRespuesta);
	for(i=0; i<tamanio; i++){
		float* t = (float*) list_get(tiemposDeRespuestas, i);
		time += *t;
	}
	pthread_mutex_unlock(&m_tiempoRespuesta);

	if(time < 0)
		time *= -1;
		
	if(tamanio != 0)
		return time/tamanio;
	else
		return 0;
}

float calcularMetricaExit(){
	int cantPC = 0;
	int tamanio = list_size(colaFinalizados->elements);
	for(int i = 0; i < tamanio; i++){
		DTB* dtb = (DTB*) list_get(colaFinalizados->elements, i);
		if(dtb != NULL)
			cantPC += dtb->PC;
	}
	if(tamanio != 0)
		return cantPC/tamanio;
	else
		return 0;
}
#include "consolaSAFA.h"

void consolaSAFA(/* mas adelante ver si lleva parametros*/){
	generadorDeIds = 1;
	char* linea=NULL;
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
			log_trace(logger,"se ha finalizado el G.DT con id = %d\n", id);
			finalizar(id);
		}
		else
		if(strcmp(p1,"status") == 0 )
		{
            int id;
            if(p2 == NULL){
                id = 0;
                log_trace(logger,"Estado de las colas:\n");
                status(0);
            }
            else{
                id = atoi(p2);	//convertir p2 a int
			    log_trace(logger,"Datos del DTB del G.DT de id = %d\n", id);
			    status(id);
            }
		}
		else
		if(strcmp(p1,"metricas") == 0 && p2 == NULL)
		{
			log_trace(logger,"Se detallaran las metricas del sistema:\n");
            metricas();
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

	free(linea);

	exit(0);

	return;
}

void pausarPlanificacion(){
	pthread_mutex_trylock(&m_puedePlanificar);
}

void continuarPlanificacion(){
	pthread_mutex_unlock(&m_puedePlanificar);
}

void ejecutar(char* rutaSc){	
	if(estadoCorrupto) {
		log_error(logger,"SAFA en estado corrupto, no se admiten nuevos GDT´S");
	}else{
		log_trace(logger,"Se hace ingreso del script escriptorio de la ruta %s para su futura ejecucion\n", rutaSc);
		DTB* dtb = malloc(sizeof(DTB));
    	dtb->id = generadorDeIds;
    	generadorDeIds++;
    	dtb->rutaScript = malloc(strlen(rutaSc)+1);
    	strcpy(dtb->rutaScript, rutaSc);
    	dtb->PC = 0;
    	dtb->flagInicializado = 1;
    	list_create(dtb->archivosAbiertos);
    	dtb->status = NEW;
		dtb->quantumFaltante = 0;

		encolarDTB(colaNew, dtb, m_colaNew);
		sem_post(&cantProcesosEnNew);
	}			
}

void finalizar(int idGDT){	
	DTB * dtb = buscarDTB(listaEjecutando, idGDT);
	if(dtb != NULL){ //está ejecutando  
			dtb->status = FINISHED;
	} 
	else{ //no está ejecutando
		dtb = quitarDTBDeSuListaActual(idGDT);
		if(dtb != NULL){
			finalizarDTB(dtb);
		} else{
			printf("No se ha encontrado un proceso con el id %d en el sistema\n",idGDT);
		}
	}	
}

void status(int idGDT){ 

	if(idGDT == 0){ // funcion status sin parametro
		int index;
	    DTB* datoDTB;

	    log_trace(logger,"La Info De Los DTB En Cada Cola Son:\n");
	    log_trace(logger,"Cola New:");
		buscarDTBEnColas(idGDT, colaNew);
		log_trace(logger,"Cola Ready:");
		buscarDTBEnColas(idGDT, colaReady);
		log_trace(logger,"Cola Blocked:");
		buscarDTBEnColas(idGDT, colaBloqueados);
		log_trace(logger,"Cola Finished:");
		buscarDTBEnColas(idGDT, colaFinalizados);

	    }
	    else{ //funcion status con parametro

	    	buscarIdGdtAsociado(idGDT);
	    	//return 0;
	    }
}

void metricas(){

}

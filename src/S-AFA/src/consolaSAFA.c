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
			log_trace(logger,"Se va a ejecutar el script escriptorio de la ruta %s\n", p2);
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

void ejecutar(char* rutaSc){
	if(estadoCorrupto){
		log_error(logger, "S-AFA en estado corrupto, no acepta ingreso de Scripts a ejecutar");
	}
	else{ 
		DTB* dtb = malloc(sizeof(DTB));
    	dtb->id = generadorDeIds;
    	generadorDeIds++;
    	dtb->rutaScript = malloc(strlen(rutaSc)+1);
    	strcpy(dtb->rutaScript, rutaSc);
    	dtb->PC = 0;
    	dtb->flagInicializado = 0;
    	list_create(dtb->archivosAbiertos);
    	dtb->status = NEW;

		encolarDTB(colaNew, dtb, m_colaNew);
		sem_post(&cantProcesosEnNew);
	}
}

void finalizar(int inGDT){
	
}

void status(int idGDT){ 

	if(idGDT == 0){ // funcion status sin parametro
		int index;
	    DTB* datoDTB;

	    log_trace(logger,"La Info De Los DTB En Cada Cola Son:\n");
	    log_trace(logger,"Cola New:\n");
		buscarDTBEnColas(idGDT, colaNew);
		log_trace(logger,"Cola Ready:\n");
		buscarDTBEnColas(idGDT, colaReady);
		log_trace(logger,"Cola Blocked:\n");
		buscarDTBEnColas(idGDT, colaBloqueados);
		log_trace(logger,"Cola Finished:\n");
		buscarDTBEnColas(idGDT, colaFinalizados);

	    }
	    else{ //funcion status con parametro

	    	buscarIdGdtAsociado(idGDT);
	    	return 0;
	    }
}

void metricas(){

}

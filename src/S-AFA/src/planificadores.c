#include "planificadores.h"

void planificadorLargoPlazo(char* rutaSc){
    DTB* dtb = malloc(sizeof(DTB));
    dtb->id = generadorDeIds;
    generadorDeIds++;
    dtb->rutaScript = malloc(strlen(rutaSc)+1);
    strcpy(dtb->rutaScript, rutaSc);
    dtb->PC = 0;
    dtb->flagInicializado = 0;
    list_create(dtb->archivosAbiertos);
    dtb->status = NEW;

    sem_wait(entradaGDT);
    queue_push();
    sem_post(cantProcesosEnReady);
}

void planificarSegunRR(int quantum){
    DTB* dtbAEjecutar;
        while(1){
			if(queue_size(colaReady) == 0){
				log_trace(pLog, "Se espera a que haya GDT's en la cola Ready");
			}
			sem_wait(cantProcesosEnReady);
			sleep(1);

			dtbAEjecutar = obtenerDTBAEjecutarSegunRR();
			log_trace(pLog, "Segun RR el DTB a ejecutar ahora es el de id = %d", dtbAEjecutar->id);
					
	}

}


void planificarSegunVRR(int quantum){
    
}

DTB* obtenerDTBAEjecutarSegunRR(){ // como FIFO
    DTB* dtb;
    pthread_mutex_lock(&m_colaReady);
    dtb = queue_pop(colaReady);
    pthread_mutex_unlock(&m_colaReady);
    return dtb;
}
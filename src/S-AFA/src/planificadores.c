#include "planificadores.h"

void planificadorLargoPlazo(){
    sem_wait(&sem_estadoCorrupto);
    while(1){
        sem_wait(&cantProcesosEnNew);
        DTB* dtb = desencolarDTB(colaNew, m_colaNew);
        dtb->status = READY;
        encolarDTB(colaReady, dtb, m_colaReady);
        sem_post(&cantProcesosEnReady);
    }
}

void planificarSegunRR(int quantum){
    DTB* dtbAEjecutar;
        while(1){
			if(queue_size(colaReady) == 0){
				log_trace(logger, "Se espera a que haya GDT's en la cola Ready");
			}
			sem_wait(&cantProcesosEnReady);
			sleep(1);

			dtbAEjecutar = obtenerDTBAEjecutarSegunRR();
			log_trace(logger, "Segun RR el DTB a ejecutar ahora es el de id = %d", dtbAEjecutar->id);
            //hacer lo que tenga que hacer...( runFunction de alguna funcion de CPU ) 
			
	    }

}


void planificarSegunVRR(int quantum){
    
}

DTB* obtenerDTBAEjecutarSegunRR(){ // como FIFO
    DTB* dtb = desencolarDTB(colaReady, m_colaReady);
    return dtb;
}

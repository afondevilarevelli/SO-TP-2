#include "planificadores.h"

void planificadorLargoPlazo(){
    while(1){
        sem_wait(&cantProcesosEnNew);
        sem_wait(&puedeEntrarAlSistema);
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

            pthread_mutex_lock(&m_puedePlanificar);
			dtbAEjecutar = obtenerDTBAEjecutarSegunRR();
            pthread_mutex_unlock(&m_puedePlanificar);
			log_trace(logger, "Segun RR el DTB a ejecutar ahora es el de id = %d", dtbAEjecutar->id);
            //hacer lo que tenga que hacer...( runFunction de alguna funcion de CPU ) 
			

            //cuando un dtb va a finalizados...
            sem_post(&puedeEntrarAlSistema);
	    }

}


void planificarSegunVRR(int quantum){
    
}

DTB* obtenerDTBAEjecutarSegunRR(){ // como FIFO
    DTB* dtb = desencolarDTB(colaReady, m_colaReady);
    return dtb;
}

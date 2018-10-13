#include "planificadores.h"

void planificadorLargoPlazo(){
    while(1){
        sem_wait(&cantProcesosEnNew);
        sem_wait(&puedeEntrarAlSistema);
        DTB* dtbDummy = desencolarDTB(colaNew, m_colaNew);
        dtbDummy->status = READY;
        dtbDummy->flagInicializado = 0;
        encolarDTB(colaReady, dtbDummy, m_colaReady);
        sem_post(&cantProcesosEnReady);
    }
}

//al quantum accedo por datosConfigSAFA->quantum
void planificarSegunRR(CPU* cpu){
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

            char* string_id = dtbAEjecutar->id + '0';
            char* string_flagInicializacion = dtbAEjecutar->flagInicializado + '0';  
            char* string_quantum = datosConfigSAFA->quantum + '0';
            runFunction(cpu->socket,"ejecutarCPU",3, string_id, 
                                                     string_flagInicializacion, 
                                                     dtbAEjecutar->rutaScript);    

            sem_wait(&cpu->aviso);   
			
	    }

}


void planificarSegunVRR(CPU* cpu){
    
}

void planificarSegunAlgoritmoPropio(CPU* cpu){
    
}

DTB* obtenerDTBAEjecutarSegunRR(){ // como FIFO
    DTB* dtb = desencolarDTB(colaReady, m_colaReady);
    return dtb;
}

//callable remote function
void  identificarProceso(socket_connection * connection ,char** args)
{
     log_info(logger,"Se ha conectado %s en el socket NRO %d  con IP %s,  PUERTO %d\n", args[0],connection->socket,connection->ip,connection-> port);
	 if(strcmp(args[0], "CPU") == 0){		 
		 CPU* cpu = malloc(sizeof(CPU));
		 cpu->socket = connection->socket;
		 cpu->id = generadorDeIdsCPU;
		 sem_init(&cpu->aviso, 0, 0);
		 list_add(listaCPUs, cpu);
         
         char stringQuantum[2];
         char idCPU[1];
         itoa(datosConfigSAFA->quantum, stringQuantum,10);
         itoa(generadorDeIdsCPU, idCPU,10);
	
		 runFunction(cpu->socket, "establecerQuantumYID",2, stringQuantum, idCPU);
         //free(stringQuantum);
         //free(idCPU);
		 generadorDeIdsCPU++;

		 pthread_t hiloPCP;
		 if( strcmp( datosConfigSAFA->algoritmoPlanif, "RR") == 0 ){
			pthread_create(&hiloPCP, NULL, (void*)&planificarSegunRR, cpu);
		 } else if( strcmp( datosConfigSAFA->algoritmoPlanif, "VRR") == 0 ){
			 pthread_create(&hiloPCP, NULL, (void*)&planificarSegunVRR, cpu);
		 } else{
			 pthread_create(&hiloPCP, NULL, (void*)&planificarSegunAlgoritmoPropio, cpu);
		 }
		 pthread_detach(hiloPCP);
		 list_add(hilos, &hiloPCP);
	 }
} 
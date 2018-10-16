#include "planificadores.h"

void planificadorLargoPlazo(){
    while(1){
        sem_wait(&cantProcesosEnNew);
        sem_wait(&puedeEntrarAlSistema);
        DTB* dtbDummy = desencolarDTB(colaNew, m_colaNew);
        dtbDummy->status = READY;
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

            char string_id[2];
            sprintf(string_id, "%i", dtbAEjecutar->id);

            char string_flagInicializacion[2];
            sprintf(string_flagInicializacion, "%i", dtbAEjecutar->flagInicializado); 

            char string_pc[2];
                     sprintf(string_pc, "%i", dtbAEjecutar->PC);

            char string_quantum[2];
            sprintf(string_quantum, "%i", datosConfigSAFA->quantum);

            char* string_status;
            //string_status = stringFromState(dtbAEjecutar->status);

            pthread_mutex_lock(&m_listaEjecutando);
            list_add(listaEjecutando, dtbAEjecutar);
            pthread_mutex_unlock(&m_listaEjecutando);

            runFunction(cpu->socket,"ejecutarCPU",5, string_id,
            										 dtbAEjecutar->rutaScript,
													 string_pc,
                                                     string_flagInicializacion, 
													 //string_status,
													 string_quantum);

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
         generadorDeIdsCPU++;
		 sem_init(&cpu->aviso, 0, 0);
		 list_add(listaCPUs, cpu);
         
         char stringQuantum[2];
         sprintf(stringQuantum, "%i", datosConfigSAFA->quantum);
         char string_idCPU[2];   
         sprintf(string_idCPU, "%i", cpu->id);
	
		 runFunction(connection->socket, "establecerQuantumYID",2, stringQuantum, string_idCPU);
         

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

#include <stdio.h>
#include <stdlib.h>
#include "libSAFA.h"

void encolarDTB(t_queue* c, DTB* d, pthread_mutex_t m){
	pthread_mutex_lock(&m);
    queue_push(c, d);
    pthread_mutex_unlock(&m);
}

DTB* desencolarDTB(t_queue* c, pthread_mutex_t m){
	pthread_mutex_lock(&m);
    DTB* d = queue_pop(c);
	d->status = READY;
    pthread_mutex_unlock(&m);
	return d;
}

CPU* buscarCPU(int id){
	pthread_mutex_lock(&m_busqueda);
	idCpuABuscar = id;
	CPU* cpu = list_find(listaCPUs, (void*)closureIdCPU);
	pthread_mutex_unlock(&m_busqueda);
	return cpu;
}

bool closureIdCPU(CPU* cpu){
	return cpu->id == idCpuABuscar;
}

DTB* buscarYRemoverDTB(t_list* list, pthread_mutex_t mutex, int id){
	pthread_mutex_lock(&m_busqueda);
	idDtbABuscar = id;
	DTB* dtb = list_find(list, (void*)closureIdDTB);
	pthread_mutex_lock(&mutex);
	list_remove_by_condition(list, (void*)closureIdDTB);
	pthread_mutex_unlock(&m_busqueda);
	pthread_mutex_unlock(&mutex);
	return dtb;
}

DTB* buscarDTB(t_list* list, int id){
	pthread_mutex_lock(&m_busqueda);
	idDtbABuscar = id;
	DTB* dtb = list_find(list, (void*)closureIdDTB);
	pthread_mutex_unlock(&m_busqueda);
	return dtb;
}

bool closureIdDTB(DTB* dtb){
	return idDtbABuscar == dtb->id;
}

//LOG
void configure_logger() {

	char * nombrePrograma = "SAFA.log";
	char * nombreArchivo = "SAFA";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se genero log de S-AFA");
}

void close_logger() {
	log_info(logger, "Cierro log de S-AFA");
	log_destroy(logger);
}

//CONFIG
t_config_SAFA * read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo S-AFA.config");

	t_config* archivo_Config = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);
	}

	datosConfigSAFA = malloc(sizeof(t_config_SAFA));

	datosConfigSAFA->puerto = config_get_int_value(archivo_Config, "S-AFA_PUERTO");
	char* alg = string_new();
	string_append(&alg, config_get_string_value(archivo_Config, "ALGORITMO_PLANIF"));
	datosConfigSAFA->algoritmoPlanif = alg;
	datosConfigSAFA->quantum = config_get_int_value(archivo_Config, "QUANTUM");
	datosConfigSAFA->gradoMultiprog = config_get_int_value(archivo_Config, "GRADO_MULTIPROG");
	datosConfigSAFA->retardo = config_get_int_value(archivo_Config, "RETARDO");

	log_info(logger, "	PUERTO: %d", datosConfigSAFA->puerto);
	log_info(logger, "	ALGORITMO_PLANIFICACION: %s", datosConfigSAFA->algoritmoPlanif);
	log_info(logger, "	QUANTUM: %d", datosConfigSAFA->quantum);
	log_info(logger, "	GRADO_MULTIPROGRAMACION: %d", datosConfigSAFA->gradoMultiprog);
	log_info(logger, "	RETARDO: %d", datosConfigSAFA->retardo);

	log_info(logger, "Fin de lectura");

	config_destroy(archivo_Config);
	free(alg);
       return datosConfigSAFA;
 
} // al final de esta funcion me queda la variable datosConfigSAFA con la config de SAFA

//CallableRemoteFunctions

//es llamada por CPU y DAM cuando se conectan, para poder manejar el estado corrupto
void newConnection(socket_connection* socketInfo, char** msg){
	if(estadoCorrupto){
		if(strcmp(msg[0], "CPU") == 0 ){
			unCpuConectado = true;
		}
		if(strcmp(msg[0], "DAM") == 0 ){
			damConectado = true;
		}
		estadoCorrupto = !unCpuConectado || !damConectado;		
	}	
}



//args[0]: idCPU, args[1]: idGDT, args[2]:"finalizar" ó "continuar"
//								  "finalizar" => finalizo GDT,
//								  "continuar" => NO finalizo GDT
void finalizacionProcesamientoCPU(socket_connection* socketInfo, char** msg){
	int idCPU = msg[0];
	int idDTB = msg[1];
	CPU* cpu = buscarCPU(idCPU);
	DTB* dtb = buscarYRemoverDTB(listaEjecutando, m_listaEjecutando, idDTB);
	
	if( strcmp( msg[2], "finalizar") == 0){
		finalizarDTB(dtb);
	} else{ //continuar		
		encolarDTB(colaReady, dtb, m_colaReady);
		sem_post(&cantProcesosEnReady);
	}

	sem_post(&cpu->aviso);
}


//FIN callable remote functions
void finalizarDTB(DTB* dtb){
	log_info(logger,"se va a finalizar el GDT de id %d",dtb->id);
	queue_push(colaFinalizados, dtb);
	sem_post(&puedeEntrarAlSistema);
}

static inline char *stringFromState(status_t status) { //Agarra un estado del enum y retorna el valor como String
    static const char *strings[] = { "NEW", "READY", "BLOCKED", "RUNNING", "FINISHED"};

    return strings[status];
}

void * buscarIdGdtAsociado(int idGDT){ //Idea de Buscar Por Cada Cola Hasta Encontrar El Id Especifico

	buscarDTBEnColas(idGDT, colaNew);
	buscarDTBEnColas(idGDT, colaReady);
	buscarDTBEnColas(idGDT, colaBloqueados);
	buscarDTBEnColas(idGDT, colaFinalizados);

}

void * buscarDTBEnColas(int idDTB, t_queue* colaBusqueda) {

// Por Ahora agarra el primer elemento de la cola y se fija si es el buscado
// Habra que hacer despues que busque por cada nodo
	int index = 0;
	DTB* elemento;


	if(queue_size(colaBusqueda) == 0) {printf("La Cola Esta Vacia\n");return 0;}

    elemento = list_get(colaBusqueda->elements, index);

    while(index < queue_size(colaBusqueda)){

    if(idDTB == 0) {mostrarInformacionDTB(elemento);
					return 0;}

	if(idDTB != elemento->id) {

		printf("No Se Encontro En La Cola\n");
		index++;
	    elemento = list_get(colaBusqueda->elements, index);
	}

	else {

	mostrarInformacionDTB(elemento);
	return 0;
	}

   }

}

void mostrarInformacionDTB(DTB* unDTB){
		char* estado;

    	printf("Los Datos Almacenados En El DTB Son:\n");
    	printf("Id DTB: %d\n", unDTB->id);
    	printf("Ruta del Escriptorio: %s\n", unDTB->rutaScript);
    	printf("Program Counter: %d\n", unDTB->PC);
    	printf("Flag Inicializado: %d\n", unDTB->flagInicializado);
    	estado = stringFromState(unDTB->status);
    	printf("Estado: %s\n", estado);

    }

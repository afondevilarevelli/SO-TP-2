#include <stdio.h>
#include <stdlib.h>
#include "libFM9.h"
#include <string.h>

//LOG
void configure_logger() {

	char * nombrePrograma = "FM9.log";
	char * nombreArchivo = "FM9";

	logger = log_create(nombrePrograma, nombreArchivo, true, LOG_LEVEL_TRACE);
	log_info(logger, "Se genero log de FM9");
}

void close_logger() {
	log_info(logger, "Cierro log de FM9");
	log_destroy(logger);
}

//CONFIG
t_config_FM9* read_and_log_config(char* path) {
	log_info(logger, "Voy a leer el archivo FM9.config");

	t_config* archivo_Config = config_create(path);
	if (archivo_Config == NULL) {
		log_error(logger, "No existe archivo de configuracion");
		exit(1);
	}

	t_config_FM9* _datosFM9 = malloc(sizeof(t_config_FM9));

	_datosFM9->puerto = config_get_int_value(archivo_Config, "PUERTO");
	_datosFM9->modo = malloc(strlen(config_get_string_value(archivo_Config, "MODO")) + 1);
	strcpy(_datosFM9->modo, config_get_string_value(archivo_Config, "MODO"));
	_datosFM9->tamanio = config_get_int_value(archivo_Config, "TAMANIO");
	_datosFM9->maximoLinea = config_get_int_value(archivo_Config, "MAX_LINEA");
	_datosFM9->tamanioPagina = config_get_int_value(archivo_Config,
			"TAM_PAGINA");

	log_info(logger, "	PUERTO: %d", _datosFM9->puerto);
	log_info(logger, "	MODO: %s", _datosFM9->modo);
	log_info(logger, "	TAMANIO: %d", _datosFM9->tamanio);
	log_info(logger, "	MAX_LINEA: %d", _datosFM9->maximoLinea);
	log_info(logger, "	TAM_PAGINA: %d", _datosFM9->tamanioPagina);

	log_info(logger, "Fin de lectura");

	config_destroy(archivo_Config);
	//free(modo);

	return _datosFM9;
}

//SOCKETS
void identificarProceso(socket_connection * connection, char** args) {
	log_info(logger,
			"Se ha conectado %s en el socket NRO %d  con IP %s,  PUERTO %d\n",
			args[0], connection->socket, connection->ip, connection->port);
	if(strcmp(args[0], "DAM") == 0)
		socketDAM = connection->socket;
}

void disconnect(socket_connection* socketInfo) {
	log_info(logger, "El socket n°%d se ha desconectado.", socketInfo->socket);
}

//SEGMENTACION PURA
void inicializarMemoriaConSegmentacion(){
	log_info(logger, "Voy a reservar espacio para guardar los procesos y la tabla de segmentos");
	memoria = calloc(1,datosConfigFM9->tamanio);
	lista_tabla_segmentos = list_create();
	log_info(logger, "Espacio reservado y tabla generada con exito");
}

//SEGMENTACION PAGINADA
/*
void inicializarMemoriaConSegmentacionPaginada(){
	log_info(logger, "Voy a reservar espacio para guardar los procesos y la tabla de segmentos");
	memoria = calloc(1,datosConfigFM9->tamanio);
	lista_tabla_segmentos = list_create();
	log_info(logger, "Espacio reservado y tabla generada con exito");
} */

//PAGINACION INVERTIDA
void inicializarMemoriaConPaginacionInvertida(){
	int cantMarcos = datosConfigFM9->tamanio / datosConfigFM9->tamanioPagina;
	log_info(logger, "Hay %d marcos", cantMarcos);
	log_info(logger, "Voy a reservar espacio para guardar los procesos y la tabla de paginas invertidas");
	memoria = calloc(1,datosConfigFM9->tamanio);
	tabla_paginasInvertidas = list_create();
	setearNumerosMarcos(cantMarcos);
	log_info(logger, "Espacio reservado y tabla generada con exito");
}

void setearNumerosMarcos(int cantMarcos){
	int i;
	for(i=0; i<cantMarcos; i++){
		t_PaginasInvertidas* marco = malloc(sizeof(t_PaginasInvertidas));
		marco->marco = i;
		marco->libre = true;
		marco->pagina = -1;
		marco->PID = -1;
		marco->tamanioOcupado = 0;
		marco->siguiente = NULL;
		list_add(tabla_paginasInvertidas, marco);
	}
}

//Busco entre un nodo y el siguiente si hay espacio para guardar. 
//TODO La lista tiene que estar ordenada
int devolverPosicionNuevoSegmento(int tamanioAPersistir){
	int pos = 0;
	t_tabla_segmentos* auxNodo;
	t_tabla_segmentos* auxNodoSiguiente;
	//si no hay elementos en la tabla devuelvo 0
	if(list_size(lista_tabla_segmentos) == 0){
		return pos;
	}

	for(int i = 0; i<list_size(lista_tabla_segmentos); i++){
		auxNodo = list_get(lista_tabla_segmentos, i);
		//si hay espacio antes del primer nodo
		if(auxNodo->base > tamanioAPersistir){
			return pos;
		}
		//Si no hay nodo siguiente y no supero el maximo de memoria devuelvo la posicion siguiente al ultimo nodo
		if(list_size(lista_tabla_segmentos)== i+1 && ( (auxNodo->base + auxNodo->limite + tamanioAPersistir) <= datosConfigFM9->tamanio) ){
			return (auxNodo->base + auxNodo->limite);
		}
		auxNodoSiguiente = list_get(lista_tabla_segmentos, i+1);
		//Si hay espacio para persistir los datos entre nodos, devuelvo la posicion entre un nodo y el otro
		if((auxNodoSiguiente->base - (auxNodo->base + auxNodo-> limite)) >= tamanioAPersistir)	{
			return (auxNodo->base + auxNodo->limite);
		}

	}
	//devuelvo error
	return -1;
}

retornoCargaTPI cargarArchivoTPI(char* arch, int idGDT){
	retornoCargaTPI retorno;
	int i, pagMasAlta = 0;
	int tamanioArchivo = strlen(arch) + 1;
	int tamanioRestante = tamanioArchivo;
	int cantLineas = cantidadDeLineas(arch);
	int lineasRestantes, lineasParaCargar;
	t_PaginasInvertidas* marcoAnterior;
	for(i=0; i<list_size(tabla_paginasInvertidas); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
		if(unMarco->PID == idGDT){ 
			if(pagMasAlta < unMarco->pagina)
				pagMasAlta = unMarco->pagina;
		}
	}

	for(i=0; i<list_size(tabla_paginasInvertidas); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
		if(unMarco->PID == idGDT && unMarco->tamanioOcupado < datosConfigFM9->tamanioPagina && unMarco->pagina == pagMasAlta){
			retorno.pagina = unMarco->pagina;
			retorno.desplazamiento = unMarco->tamanioOcupado; 
			if( ((datosConfigFM9->tamanioPagina - unMarco->tamanioOcupado) / datosConfigFM9->maximoLinea) >= cantLineas){
				guardarDatosPorLinea(arch, unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado);
				unMarco->tamanioOcupado += cantLineas*datosConfigFM9->maximoLinea;
				tamanioRestante = 0;
				retorno.cargaOK = true;
				return retorno;
			}
			else{
				lineasParaCargar = datosConfigFM9->tamanioPagina / datosConfigFM9->maximoLinea - unMarco->tamanioOcupado / datosConfigFM9->maximoLinea;
				if(lineasParaCargar != 0){ 
				guardarDatosDeLineas(arch, 
									unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado,
									0,
									lineasParaCargar);
				}
				unMarco->tamanioOcupado += lineasParaCargar * datosConfigFM9->maximoLinea;
				lineasRestantes = cantLineas - lineasParaCargar;
				marcoAnterior = unMarco;
			}
		}
	}

	if(lineasRestantes != 0){ 
		for(i=0; i<list_size(tabla_paginasInvertidas); i++){
			t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
			if(unMarco->libre){ 
				guardarDatosDeLineas(arch, 
									unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado,
									lineasParaCargar,
									lineasRestantes);
				unMarco->PID = idGDT;
				unMarco->pagina = pagMasAlta + 1;
				unMarco->libre = false;
				unMarco->tamanioOcupado += lineasRestantes * datosConfigFM9->maximoLinea;
				marcoAnterior->siguiente = unMarco;
				if(cantLineas == lineasRestantes){ 
					retorno.pagina = pagMasAlta + 1;
					retorno.desplazamiento = 0;
				}
				retorno.cargaOK = true;
				return retorno;
			}
		}
	}
	retorno.cargaOK = false;
	return retorno;
}

//args[0]: idGDT; args[1]: datos, args[2]: "ultima" ó "sigue" (para ver si dsp el DAM le avisa al SAFA o sigue cargando)
//args[3]: 1(Dummy) ó 0(no Dummy), args[4]: primera o no, args[5]: socketCPU
void cargarBuffer(socket_connection* connection, char** args){
	if(strcmp(args[4], "1") == 0){ 
		bufferAuxiliar = malloc(2000);
		tamanioOcupadoBufferAux = 0;
	}
	if(strcmp(args[2], "ultima") == 0){
		memcpy(bufferAuxiliar + tamanioOcupadoBufferAux, args[1], strlen(args[1]) + 1);
		bufferArchivoACargar = malloc(strlen(bufferAuxiliar) + 1);
		strcpy(bufferArchivoACargar, bufferAuxiliar);
		free(bufferAuxiliar);
		cargarArchivo(args[0], args[3], args[5]);
	}
	else{
		memcpy(bufferAuxiliar + tamanioOcupadoBufferAux, args[1], strlen(args[1]) + 1);
		tamanioOcupadoBufferAux += strlen(args[1]);
	}
}

//args[0]: idGDT; args[1]: datos, args[2]: "ultima" ó "sigue" (para ver si dsp el DAM le avisa al SAFA o sigue cargando)
//args[3]: 1(Dummy) ó 0(no Dummy), args[4]: primer pedido o no
void cargarArchivo(char* idGDT, char* esDummy, char* cpuSocket)
{
	int pid = atoi(idGDT);
	int tamanioArchivo = strlen(bufferArchivoACargar)+1;
	log_info(logger, "Voy a persistir: '%s' cuyo tamanio es %d", bufferArchivoACargar, tamanioArchivo-1);
	if(strcmp(datosConfigFM9->modo,"SEG")==0){ 	
		int tamanioReal;
		int cantLineas = cantidadDeLineas(bufferArchivoACargar);	
		tamanioReal = cantLineas * datosConfigFM9->maximoLinea;
		log_info(logger, "Se van a guardar %d lineas",cantLineas);
		log_info(logger, "Voy a buscar una posicion para almacenar los datos");
		int pos = devolverPosicionNuevoSegmento(tamanioReal);
		log_info(logger, "La posicion es %d", pos);
		if (pos != -1)
		{
			guardarDatosPorLinea(bufferArchivoACargar, pos);
			
			log_info(logger, "Persisti el contenido");

			log_info(logger, "Voy a actualizar tabla de segmentos");
			t_tabla_segmentos *nuevoSegmento = malloc(sizeof(t_tabla_segmentos));

			nuevoSegmento->pid = pid;
			nuevoSegmento->base = pos;
			nuevoSegmento->limite = tamanioReal;
			list_add(lista_tabla_segmentos, nuevoSegmento);
			list_sort(lista_tabla_segmentos, (void*)&ordenarTablaSegmentosDeMenorBaseAMayorBase);
			log_info(logger, "Se actualizo correctamente la tabla de segmentos");

			char string_segmento[2];
			sprintf(string_segmento, "%i", pos);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 7, idGDT, "ok", esDummy, "-1", string_segmento, "0", cpuSocket);

		}else{
			log_error(logger, "No se encontro una posicion dentro de la tabla de segmentos");
			log_error(logger, "No se pudo persistir los datos");
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 7, idGDT, "error", esDummy, "-1", "-1", "-1", cpuSocket);
		}
	}
	else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		retornoCargaTPI cargado = cargarArchivoTPI( bufferArchivoACargar, pid);
		if(cargado.cargaOK){ 
			log_info(logger, "Persisti el contenido %s para el GDT %d", bufferArchivoACargar, pid);
			char string_despl[3];
			sprintf(string_despl, "%i", cargado.desplazamiento);
			char string_pagina[3];
			sprintf(string_pagina, "%i", cargado.pagina);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 7, idGDT, "ok", esDummy, string_pagina, "-1",string_despl,cpuSocket);
			
		}else{
			log_info(logger, "Error al persistir el contenido %s para el GDT %d",bufferArchivoACargar, pid);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 7, idGDT, "error", esDummy, "-1", "-1", "-1", cpuSocket);
		}
	}
	else{ //SEGMENTACION PAGINADA
		printf("No esta implementado papu\n");
		free(bufferArchivoACargar);
	}
}
/*
t_PaginasInvertidas* obtenerUltimoMarcoDeGDT(int idGDT){
	int i;
	t_PaginasInvertidas* ultimo;
	for(i=0; i<list_size(tabla_paginasInvertidas); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
		if(unMarco->PID == idGDT && ){ 
			numMarco = unMarco->marco;
			unMarco->PID = idGDT;
			unMarco->pagina = pagMasAlta;
			unMarco->libre = false;
			unMarco->tamanioOcupado += tamanioArchivo;
			break;
		}
	}
	return ultimo;
} */

//para segmentacion
void guardarDatosPorLinea(char* datos, int pos){
	char datosLineas[datosConfigFM9->maximoLinea];
	char datosAux[datosConfigFM9->maximoLinea];
	int tamanioArchivo = strlen(datos) + 1;
	int posActual = pos;
	int i = 0;
	while(1){ 
		int j = 0;
		while( datos[i] != '\n'){
			if(i + 1 == tamanioArchivo)
				break;
			datosAux[j] = datos[i];
			i++;
			j++;
		}
		if(i + 1 == tamanioArchivo){
			if( datos[i - 1] == '\n' || datos[i] == '\n'){ 
				datosLineas[0] = '\0';
				pthread_mutex_lock(&m_memoria);
				memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
				pthread_mutex_unlock(&m_memoria);
			}
			else{ 
				datosAux[j] = '\0';
				strcpy(datosLineas, datosAux);
				pthread_mutex_lock(&m_memoria);
				memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
				pthread_mutex_unlock(&m_memoria);
			}
			break;
		}
		datosAux[j] = '\n';
		datosAux[j+1] = '\0';
		i++;
		strcpy(datosLineas, datosAux);
		pthread_mutex_lock(&m_memoria);
		memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
		pthread_mutex_unlock(&m_memoria);
		posActual += datosConfigFM9->maximoLinea;
	}
}

//lineaInicial == 0 -> primeraLinea
//cantLineasEnAdelante (la linea inicial incluida)
void guardarDatosDeLineas(char* datos, int pos, int lineaInicial, int cantLineasEnAdelante){
	char* datosFiltrados[cantLineasEnAdelante];//array de lineas filtradas
	char lineaFiltrada[datosConfigFM9->maximoLinea];
	int posFiltrado = 0, cantLineasAux = 0;

	while(cantLineasAux < lineaInicial){ 
		while( datos[posFiltrado] != '\n'){
			posFiltrado++;
		}
		posFiltrado++;
		cantLineasAux++;
	}
	if(datos[posFiltrado] != '\0'){
		for(int q = 0; q < cantLineasEnAdelante; q++){
			datosFiltrados[q] = malloc(datosConfigFM9->maximoLinea);
			int u = 0;
			while( datos[posFiltrado] != '\n'){
				lineaFiltrada[u] = datos[posFiltrado];
				u++;
				posFiltrado++;
			}
			lineaFiltrada[u] = '\n';
			lineaFiltrada[u+1] = '\0';
			posFiltrado++;
			strcpy(datosFiltrados[q], lineaFiltrada);
		}
	}	
	else{
		lineaFiltrada[0] = '\0';
		pthread_mutex_lock(&m_memoria);
		memcpy(memoria + pos, lineaFiltrada, datosConfigFM9->maximoLinea);
		pthread_mutex_unlock(&m_memoria);
		return;
	}

	for(int m = 0; m < cantLineasEnAdelante; m++){
		pthread_mutex_lock(&m_memoria);
		memcpy(memoria + pos + m * datosConfigFM9->maximoLinea, datosFiltrados[m], datosConfigFM9->maximoLinea);
		pthread_mutex_unlock(&m_memoria);
	}
}

int cantidadDeLineas(char* datos){
	int tamanioArchivo = strlen(datos) + 1; 
	int cantLineas = 1;
	int m = 0;
	for(m = 0; m < tamanioArchivo; m++){ 
		if( datos[m] == '\n'){
			cantLineas++;
		}
	}	
	return cantLineas;
}

//args[0]: idGDT, args[1]: pag, args[2]:baseSeg, args[3]:despl, args[4]: numLinea, args[5]: datos
void actualizarDatosDTB(socket_connection* connection, char** args){ //La primer linea es la 1!!!

	int idGDT = atoi(args[0]);
	int pagina = atoi(args[1]);
	int desplazamiento = atoi(args[3]);
	int base = atoi(args[2]);
	int linea = atoi(args[4]);
	char* datos = args[5];
	int socketCPU = connection->socket;

	log_info(logger, "Del GDT: %d, recibi los siguientes Datos:", idGDT);
	log_info(logger, "	Base: %d, Linea: %d, Datos: %s",base, linea, datos);
	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		//Me fijo si existe en la tabla de segmentos
		t_tabla_segmentos* datosSegmento = list_find(lista_tabla_segmentos, _buscarSegmento);

		if(datosSegmento == NULL){
			log_error(logger, "No se pueden actualizar los datos, no existe dicho segmento");
			return;
		}

		if( datosSegmento->limite / datosConfigFM9->maximoLinea >= linea){ //La primer linea es la 1!!!
			char datosLinea[datosConfigFM9->maximoLinea];
			strcpy(datosLinea, datos);
			pthread_mutex_lock(&m_memoria);
			memcpy(memoria+base+(linea-1)*datosConfigFM9->maximoLinea, datosLinea, datosConfigFM9->maximoLinea);
			pthread_mutex_unlock(&m_memoria);
			log_trace(logger, "Se ha actualizado la linea %d con el valor: %s",linea, datos);
		}else{ 
			//intenta escribir en una linea que no existe del archivo
			log_error(logger, "La linea que se desea actualizar no existe");
		}
	}
	else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		log_error(logger, "TODO: no implementado actualizar para TPI");
	}
	else{
		log_error(logger, "TODO: no implementado Seg Pag");
	}
	runFunction(socketCPU, "avisarTerminoClock", 0);
}

//args[0]: idGDT, args[1]:Linea (0 es la primer linea), args[2]: pag, args[3]: baseSeg, args[4]: despl
void obtenerDatosCPU(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int base = atoi(args[3]);
	int numLinea = atoi(args[1]);
	int pagina = atoi(args[2]);
	int despl = atoi(args[4]);

	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		t_tabla_segmentos* segmento = list_find(lista_tabla_segmentos, _buscarSegmento);
		if(segmento == NULL || (segmento->limite / datosConfigFM9->maximoLinea) < numLinea){
			runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, "0", "", "0");
		}
		else{ 
			char* linea = malloc(datosConfigFM9->maximoLinea);
			pthread_mutex_lock(&m_memoria);
			memcpy(linea, memoria + segmento->base + numLinea*datosConfigFM9->maximoLinea, datosConfigFM9->maximoLinea);
			log_info(logger, "Se obtuvieron los siguientes datos: %s", linea);
			if( *(memoria + segmento->base + numLinea*datosConfigFM9->maximoLinea + datosConfigFM9->maximoLinea) == '\0' )
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 3, "1", linea, "1");
			else
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 3, "1", linea, "0");
			pthread_mutex_unlock(&m_memoria);
			free(linea);
		}
	} else if(strcmp(datosConfigFM9->modo,"TPI")==0){

	}
	else{ //SPA

	}
}

//args[0]: idGDT, args[1]: pagina, args[2]: baseSegmento, args[3]: desplazamiento
void DAM_FM9_obtenerDatosFlush(socket_connection* connection, char** args){

}

//args[0]: idGDT, args[1]: pagina, args[2]: baseSegmento, args[3]: desplazamiento
void cerrarArchivoDeDTB(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int baseSegmento = atoi(args[2]);
	int pagina = atoi(args[1]);
	int despl = atoi(args[3]);

	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &baseSegmento);	
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		t_tabla_segmentos* segmento = list_remove_by_condition(lista_tabla_segmentos, &_buscarSegmento);
		if(segmento == NULL){
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 , "0");
		}
		else{ 
			free(segmento);
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 ,"1");
		}
	} else if(strcmp(datosConfigFM9->modo,"TPI")==0){

	}
	else{ //SPA

	}
	//Realizar Operacion de Cerrado

}

bool ordenarTablaSegmentosDeMenorBaseAMayorBase(t_tabla_segmentos* unNodo, t_tabla_segmentos* nodoSiguiente){
	return unNodo->base < nodoSiguiente->base;
}

bool buscarSegmento(t_tabla_segmentos* unNodo, int* pid, int* base){
	return (unNodo->pid ==  *pid && unNodo->base == *base);
}


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
		if(auxNodo->base > tamanioAPersistir && i== 0){
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
	int cantLineas = cantidadDeLineas(arch);
	int lineasCargadas = 0;
	int maximoLineasPagina = datosConfigFM9->tamanioPagina / datosConfigFM9->maximoLinea;
	t_PaginasInvertidas* marcoAnterior = NULL;

	bool _marcoLibre(void* elemento){
		return ((t_PaginasInvertidas*) elemento)->libre ;
	}
	bool _pagsGDT(void* elemento){
		return ((t_PaginasInvertidas*) elemento)->PID == idGDT;
	}

	t_list* listaPagsGDT = list_filter(tabla_paginasInvertidas, &_pagsGDT);
	for(i=0; i<list_size(listaPagsGDT); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(listaPagsGDT, i);
		if(pagMasAlta < unMarco->pagina)
			pagMasAlta = unMarco->pagina;	
	}

	for(i=0; i<list_size(listaPagsGDT); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(listaPagsGDT, i);
		if(unMarco->tamanioOcupado < datosConfigFM9->tamanioPagina && unMarco->pagina == pagMasAlta){
			retorno.pagina = unMarco->pagina;
			retorno.desplazamiento = unMarco->tamanioOcupado; 
			retorno.marco = unMarco->marco;
			if( ((datosConfigFM9->tamanioPagina - unMarco->tamanioOcupado) / datosConfigFM9->maximoLinea) >= cantLineas){
				guardarDatosPorLinea(arch, unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado);
				unMarco->tamanioOcupado += cantLineas*datosConfigFM9->maximoLinea;
				lineasCargadas = cantLineas;
				retorno.cargaOK = true;
				return retorno;
			}
			else{
				lineasCargadas = datosConfigFM9->tamanioPagina / datosConfigFM9->maximoLinea - unMarco->tamanioOcupado / datosConfigFM9->maximoLinea;
				if(lineasCargadas != 0){ 
				guardarDatosDeLineas(arch, 
									unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado,
									0,
									lineasCargadas);
				}
				retorno.desplazamiento = unMarco->tamanioOcupado;
				unMarco->tamanioOcupado += lineasCargadas * datosConfigFM9->maximoLinea;
				marcoAnterior = unMarco;
				retorno.pagina = unMarco->pagina;
				
				retorno.marco = unMarco->marco;
			}
			break;
		}
	}
	
	int lineasParaCargar;
	int contadorPagina = 0;
	while(lineasCargadas < cantLineas){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_find(tabla_paginasInvertidas, &_marcoLibre);
		lineasParaCargar = cantLineas - lineasCargadas;
		if(unMarco != NULL){ 
			if( ((datosConfigFM9->tamanioPagina - unMarco->tamanioOcupado) / datosConfigFM9->maximoLinea) >= lineasParaCargar){
				guardarDatosDeLineas(arch, 
									unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado,
									lineasCargadas,
									lineasParaCargar);
				unMarco->PID = idGDT;
				unMarco->pagina = pagMasAlta + 1 + contadorPagina;
				unMarco->libre = false;
				unMarco->tamanioOcupado += lineasParaCargar * datosConfigFM9->maximoLinea;
				if(marcoAnterior != NULL)
					marcoAnterior->siguiente = unMarco;
				if(contadorPagina == 0 && lineasCargadas == 0){ 
					retorno.pagina = pagMasAlta + 1;
					retorno.desplazamiento = 0;
					retorno.marco = unMarco->marco;
				}
				retorno.cargaOK = true;
				return retorno;
			}
			else{
				guardarDatosDeLineas(arch, 
									unMarco->marco*datosConfigFM9->tamanioPagina + unMarco->tamanioOcupado,
									lineasCargadas,
									maximoLineasPagina);
				if(contadorPagina == 0 && lineasCargadas == 0){ 
					retorno.pagina = pagMasAlta + 1;
					retorno.desplazamiento = 0;
					retorno.marco = unMarco->marco;
				}
				lineasCargadas += maximoLineasPagina;
				unMarco->PID = idGDT;
				unMarco->pagina = pagMasAlta + 1 + contadorPagina;
				unMarco->libre = false;
				unMarco->tamanioOcupado += maximoLineasPagina * datosConfigFM9->maximoLinea;
				if(marcoAnterior != NULL)
					marcoAnterior->siguiente = unMarco;
				marcoAnterior = unMarco;
				contadorPagina++;
			}
		}
		else{
			retorno.cargaOK = false;
			break;
		}
	}

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
		log_info(logger,"Agrego '%s' al buffer",args[1]);
		pthread_mutex_lock(&m_buffer);
		memcpy(bufferAuxiliar + tamanioOcupadoBufferAux, args[1], strlen(args[1]) + 1);
		bufferArchivoACargar = malloc(strlen(bufferAuxiliar) + 1);
		strcpy(bufferArchivoACargar, bufferAuxiliar);
		pthread_mutex_unlock(&m_buffer);
		free(bufferAuxiliar);
		cargarArchivo(args[0], args[3], args[5]);
	}
	else{
		log_info(logger,"Agrego '%s' al buffer",args[1]);
		pthread_mutex_lock(&m_buffer);
		memcpy(bufferAuxiliar + tamanioOcupadoBufferAux, args[1], strlen(args[1]) + 1);
		tamanioOcupadoBufferAux += strlen(args[1]);
		pthread_mutex_unlock(&m_buffer);
	}
}

//args[0]: idGDT; args[1]: datos, args[2]: "ultima" ó "sigue" (para ver si dsp el DAM le avisa al SAFA o sigue cargando)
//args[3]: 1(Dummy) ó 0(no Dummy), args[4]: primer pedido o no
void cargarArchivo(char* idGDT, char* esDummy, char* cpuSocket)
{
	int pid = atoi(idGDT);
	int tamanioArchivo = strlen(bufferArchivoACargar)+1;
	int cantLineas = cantidadDeLineas(bufferArchivoACargar);
	int pagina;
	log_trace(logger, "Voy a persistir: '%s' cuyo tamanio es %d", bufferArchivoACargar, tamanioArchivo-1);
	bool _buscarPaginaInvertida(void* elemento){
		return buscarPaginaInvertida(elemento, &pid, &pagina);
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){ 	
		int tamanioReal;	
		tamanioReal = cantLineas * datosConfigFM9->maximoLinea;
		log_info(logger, "Se van a guardar %d lineas",cantLineas);
		log_info(logger, "Voy a buscar una posicion para almacenar los datos");
		pthread_mutex_lock(&m_listaSegmentos);
		int pos = devolverPosicionNuevoSegmento(tamanioReal);
		pthread_mutex_unlock(&m_listaSegmentos);
		log_info(logger, "La posicion es %d", pos);
		if (pos != -1)
		{
			pthread_mutex_lock(&m_memoria);
			guardarDatosPorLinea(bufferArchivoACargar, pos);
			pthread_mutex_unlock(&m_memoria);
			
			log_trace(logger, "Persisti el contenido para el GDT %d en la posicion %d", pid, pos);

			log_info(logger, "Voy a actualizar tabla de segmentos");
			t_tabla_segmentos *nuevoSegmento = malloc(sizeof(t_tabla_segmentos));

			nuevoSegmento->pid = pid;
			nuevoSegmento->base = pos;
			nuevoSegmento->limite = tamanioReal;
			pthread_mutex_lock(&m_listaSegmentos);
			list_add(lista_tabla_segmentos, nuevoSegmento);
			list_sort(lista_tabla_segmentos, (void*)&ordenarTablaSegmentosDeMenorBaseAMayorBase);
			pthread_mutex_unlock(&m_listaSegmentos);
			log_info(logger, "Se actualizo correctamente la tabla de segmentos");

			char* string_segmentoImpostor = string_itoa(nuevoSegmento->base);
			char string_segmento[strlen(string_segmentoImpostor)];
			strcpy( string_segmento, string_segmentoImpostor);
			char string_cantLineas[2];
			sprintf(string_cantLineas, "%i", cantLineas);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, idGDT, "ok", esDummy, "-1", string_segmento, "-1", string_cantLineas, cpuSocket);

		}else{
			log_error(logger, "No se encontro una posicion dentro de la tabla de segmentos");
			log_error(logger, "No se pudo persistir los datos");
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, idGDT, "error", esDummy, "-1", "-1", "-1","0", cpuSocket);
		}
	}
	else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		log_info(logger, "Se van a guardar %d lineas",cantLineas);
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		pthread_mutex_lock(&m_memoria);
		retornoCargaTPI cargado = cargarArchivoTPI( bufferArchivoACargar, pid);
		pthread_mutex_unlock(&m_memoria);
		pthread_mutex_unlock(&m_listaPaginasInvertidas);
		if(cargado.cargaOK){
			if(cargado.desplazamiento == datosConfigFM9->maximoLinea){
				pagina = cargado.pagina;
				pthread_mutex_lock(&m_listaPaginasInvertidas);
				t_PaginasInvertidas* paginaInvertida = list_find(tabla_paginasInvertidas, &_buscarPaginaInvertida);
				pthread_mutex_unlock(&m_listaPaginasInvertidas);
				if(paginaInvertida != NULL){
					if (paginaInvertida->siguiente != NULL){
						cargado.pagina = (paginaInvertida->siguiente)->pagina;
						cargado.marco = (paginaInvertida->siguiente)->marco;
						cargado.desplazamiento = 0;
					}
				}
			} 
			log_trace(logger, "Persisti el contenido para el GDT %d", pid);
			log_trace(logger, "Pagina: %d   Marco: %d   Desplazamiento: %d", cargado.pagina,cargado.marco, cargado.desplazamiento);
			char* string_despl = string_itoa(cargado.desplazamiento);
			char* string_pagina = string_itoa(cargado.pagina);
			char string_cantLineas[2];
			sprintf(string_cantLineas, "%i", cantLineas);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, idGDT, "ok", esDummy, string_pagina, "-1",string_despl,string_cantLineas,cpuSocket);
			
		}else{
			log_info(logger, "Error al persistir el contenido %s para el GDT %d",bufferArchivoACargar, pid);
			free(bufferArchivoACargar);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, idGDT, "error", esDummy, "-1", "-1", "-1","0", cpuSocket);
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
				memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
			}
			else{ 
				datosAux[j] = '\0';
				strcpy(datosLineas, datosAux);
				memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
			}
			break;
		}
		datosAux[j] = '\n';
		datosAux[j+1] = '\0';
		i++;
		strcpy(datosLineas, datosAux);
		memcpy(memoria + posActual, datosLineas, datosConfigFM9->maximoLinea);
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
				if(datos[posFiltrado] == '\0')
					break;
				lineaFiltrada[u] = datos[posFiltrado];
				u++;
				posFiltrado++;
			}
			if(datos[posFiltrado] != '\0'){ 
				lineaFiltrada[u] = '\n';
				lineaFiltrada[u+1] = '\0';
			}
			else{
				lineaFiltrada[u] = '\0';
			}
			posFiltrado++;
			strcpy(datosFiltrados[q], lineaFiltrada);
		}
	}	
	else{
		lineaFiltrada[0] = '\0';
		memcpy(memoria + pos, lineaFiltrada, datosConfigFM9->maximoLinea);
		return;
	}

	for(int m = 0; m < cantLineasEnAdelante; m++){
		memcpy(memoria + pos + m * datosConfigFM9->maximoLinea, datosFiltrados[m], datosConfigFM9->maximoLinea);
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

//args[0]: idGDT, args[1]: pag, args[2]:baseSeg, args[3]:despl, args[4]: cantLineas, args[5]:numLinea, 
//args[6]: datos
void actualizarDatosDTB(socket_connection* connection, char** args){ //La primer linea es la 1!!!

	int idGDT = atoi(args[0]);
	int pagina = atoi(args[1]);
	int desplazamiento = atoi(args[3]);
	int base = atoi(args[2]);
	int linea = atoi(args[5]);
	char* datos = args[6];
	int cantLineasArchivo = atoi(args[4]);
	int socketCPU = connection->socket;

	log_trace(logger, "Solicitud para actualizar por parte del GDT %d", idGDT);

	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}
	bool _buscarPaginaInvertida(void* elemento){
		return buscarPaginaInvertida(elemento, &idGDT, &pagina);
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		log_info(logger, "Del GDT: %d, recibi los siguientes Datos:", idGDT);
		log_info(logger, "	Base: %d, Linea: %d, Datos: %s",base, linea, datos);
		//Me fijo si existe en la tabla de segmentos
		pthread_mutex_lock(&m_listaSegmentos);
		t_tabla_segmentos* datosSegmento = list_find(lista_tabla_segmentos, _buscarSegmento);
		pthread_mutex_unlock(&m_listaSegmentos);

		if(datosSegmento == NULL || linea > cantLineasArchivo){
			log_error(logger, "Acceso denegado al GDT %d para actualizar datos", idGDT);
			return;
		}

		if( datosSegmento->limite / datosConfigFM9->maximoLinea >= linea){ //La primer linea es la 1!!!
			char datosLinea[datosConfigFM9->maximoLinea];
			strcpy(datosLinea, datos);
			datosLinea[strlen(datos)] = '\n';
			datosLinea[strlen(datos) + 1] = '\0';
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
		int desplLineaNuevaPag = 0;
		log_info(logger, "Del GDT: %d, recibi los siguientes Datos:", idGDT);
		log_info(logger, "	Pagina: %d, Desplazamiento: %d, Datos: %s",pagina, desplazamiento, datos);
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		t_PaginasInvertidas* paginaInvertida = list_find(tabla_paginasInvertidas, &_buscarPaginaInvertida);
		pthread_mutex_unlock(&m_listaPaginasInvertidas);
		if(paginaInvertida == NULL || linea > cantLineasArchivo){
			log_error(logger, "Acceso denegado al GDT %d para actualizar datos", idGDT);
			return;
		}

		int sumaPag = desplazamiento;
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		for(int m = 0; m<(linea-1); m++){
			int j = m + sumaPag;
			if( j*datosConfigFM9->maximoLinea >= datosConfigFM9->tamanioPagina){
				paginaInvertida = paginaInvertida->siguiente;
				if(paginaInvertida == NULL){
					log_error(logger, "Acceso denegado al GDT %d para actualizar datos", idGDT);
					return;
				}
				sumaPag = m;
				desplLineaNuevaPag = 0;
			}
			else{ 
				desplLineaNuevaPag++;
			}
		}
		
		//salgo del bucle con el puntero a la pagina (paginaInvertida) y
		//el desplazamiento dentro la misma para escribir (desplLineaNuevaPag),
		//siendo desplLineaNuevaPag = 0 la primer linea de la pagina.
		char datosLinea[datosConfigFM9->maximoLinea];
		strcpy(datosLinea, datos);
		datosLinea[strlen(datos)] = '\n';
		datosLinea[strlen(datos) + 1] = '\0';
		pthread_mutex_lock(&m_memoria);
		memcpy(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + desplLineaNuevaPag*datosConfigFM9->maximoLinea,
			   datosLinea,
			   datosConfigFM9->maximoLinea);
		pthread_mutex_unlock(&m_memoria);
		pthread_mutex_unlock(&m_listaPaginasInvertidas);
		log_trace(logger, "Se ha actualizado la linea con el valor: %s", datos);
	}
	else{
		log_error(logger, "TODO: no implementado Seg Pag");
	}
	runFunction(socketCPU, "avisarTerminoClock", 0);
}

//args[0]: idGDT, args[1]:Linea (0 es la primer linea), args[2]: pag, args[3]: baseSeg, args[4]: despl, args[5]: cantLineas
void obtenerDatosCPU(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int base = atoi(args[3]);
	int numLinea = atoi(args[1]);
	int pagina = atoi(args[2]);
	int despl = atoi(args[4]);
	int cantLineas = atoi(args[5]);

	log_info(logger,"La CPU me pide la sentencia %s para el GDT %s",args[1], args[0]);
	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}
	bool _buscarPaginaInvertida(void* elemento){
		return buscarPaginaInvertida(elemento, &idGDT, &pagina);
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		log_info(logger,"Base del segmento: %s",args[3]);
		pthread_mutex_lock(&m_listaSegmentos);
		t_tabla_segmentos* segmento = list_find(lista_tabla_segmentos, _buscarSegmento);
	
		if(segmento == NULL || (segmento->limite / datosConfigFM9->maximoLinea) < numLinea){
			runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, " ", "0");
		}
		else{ 
			char* linea = malloc(datosConfigFM9->maximoLinea);
			pthread_mutex_lock(&m_memoria);
			memcpy(linea, memoria + segmento->base + numLinea*datosConfigFM9->maximoLinea, datosConfigFM9->maximoLinea);
			pthread_mutex_unlock(&m_listaSegmentos);
			log_info(logger, "Se obtuvieron los siguientes datos de sentencia: '%s'", linea);
			if( cantLineas - 3 == numLinea ){ 
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, linea, "1");
			}else{ 
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, linea, "0");
			}
			pthread_mutex_unlock(&m_memoria);
			free(linea);
		}
	} else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		log_info(logger,"Pagina: %s   Desplazamiento: %s",args[2], args[4]);
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		t_PaginasInvertidas* paginaInvertida = list_find(tabla_paginasInvertidas, _buscarPaginaInvertida);
		pthread_mutex_unlock(&m_listaPaginasInvertidas);
		if(paginaInvertida->PID != idGDT){
			runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, " ", "0");
		}
		else{
			int j = 0;
			int numLineaALeer = 0;
			pthread_mutex_lock(&m_listaPaginasInvertidas);
			for(int i = 0; i < numLinea; i++){
				if( (despl/datosConfigFM9->maximoLinea+j+1) == datosConfigFM9->tamanioPagina/datosConfigFM9->maximoLinea ){
					paginaInvertida = paginaInvertida->siguiente;
					if(paginaInvertida->PID != idGDT){
						runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, " ", "0");
						return;
					}
					numLineaALeer = 0;
					j = 0;
					despl = 0;
				}else{
					j++;
					numLineaALeer++;
				}		
			}
			char* linea = malloc(datosConfigFM9->maximoLinea);
			pthread_mutex_lock(&m_memoria);
			memcpy(linea, memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + numLineaALeer*datosConfigFM9->maximoLinea, datosConfigFM9->maximoLinea);
			log_info(logger, "Se obtuvieron los siguientes datos de sentencia: '%s'", linea);
			if( cantLineas - 3 == numLinea ){ 
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, linea, "1");
			}else{ 
				runFunction(connection->socket, "FM9_CPU_resultadoDatos", 2, linea, "0");
			}
			pthread_mutex_unlock(&m_listaPaginasInvertidas);
			pthread_mutex_unlock(&m_memoria);
			free(linea);
		}
	}
	else{ //SPA

	}
}

//args[0]: idGDT, args[1]: inicio, args[2]: cantBytes, args[3]: pag,
//args[4]:baseSeg, args[5]:despl, args[6]: cantLineas, args[7]: socketCPU
void DAM_FM9_obtenerDatosFlush(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int base = atoi(args[4]);
	int pagina = atoi(args[3]);
	int despl = atoi(args[5]);
	int tamanio = atoi(args[2]);
	int inicio = atoi(args[1]); 
	int cantLineas = atoi(args[6]);
	int contadorPosicion = 0;
	int numLinea = 0;
	int posicionLimite;
	char ultima[1] = "0";

	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}
	bool _buscarPaginaInvertida(void* elemento){
		return buscarPaginaInvertida(elemento, &idGDT, &pagina);
	}
	log_trace(logger, "Solicitud de flush por parte del GDT %s", args[0]);
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		pthread_mutex_lock(&m_listaSegmentos);
		t_tabla_segmentos* segmento = list_find(lista_tabla_segmentos, _buscarSegmento);
		pthread_mutex_unlock(&m_listaSegmentos);
		if(segmento == NULL){
			runFunction(connection->socket, "FM9_DAM_respuestaFlush", 5, args[0], "", "0",args[7], "ultimo");
		}
		else{ 
			posicionLimite = (int) (memoria + segmento->base + segmento->limite);
			char datosArchivo[tamanio + 1];
			pthread_mutex_lock(&m_memoria);
			while(contadorPosicion < inicio){
				if( *(memoria + segmento->base + contadorPosicion) == '\n'){ 
					numLinea++;
					inicio = numLinea * datosConfigFM9->maximoLinea + inicio - contadorPosicion - 1;
					contadorPosicion = numLinea * datosConfigFM9->maximoLinea;	
				}
				else
					contadorPosicion++;
			} //salgo con contadorPosicion en el índice donde quiero empezar a leer
			pthread_mutex_unlock(&m_memoria);
			int j = 0;
			int i;
			pthread_mutex_lock(&m_memoria);
			for(i=0; i<tamanio; i++){
				if( posicionLimite <= (int) (memoria + segmento->base + contadorPosicion + j) /*|| *(memoria + segmento->base + contadorPosicion + j) == '\0'*/){
					ultima[0] = '1';
					break;
				}
				if( *(memoria + segmento->base + contadorPosicion + j) == '\n'){
					datosArchivo[i] = *(memoria + segmento->base + contadorPosicion + j);
					numLinea++;
					contadorPosicion = numLinea * datosConfigFM9->maximoLinea;
					j = 0;
				}
				else{ 
					datosArchivo[i] = *(memoria + segmento->base + contadorPosicion + j);
					j++;
				}
			}

			if( *(memoria + segmento->base + contadorPosicion + j) == '\0'){
				ultima[0] = '1';
			}
			pthread_mutex_unlock(&m_memoria);

			datosArchivo[i] = '\0';
			log_trace(logger, "Se obtuvieron los siguientes datos: '%s' para el GDT %s", datosArchivo, args[0]);
			//en datosArchivo están los datos que leí
			runFunction(socketDAM, "FM9_DAM_respuestaFlush", 5 , args[0],
																 datosArchivo,
																 "1",
																 args[7],
																 ultima); 

		}
	} else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		int contadorAuxiliar = 0;
		int cantPaginasSiguientes = 0;
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		t_PaginasInvertidas* paginaInvertida = list_find(tabla_paginasInvertidas, _buscarPaginaInvertida);
		pthread_mutex_unlock(&m_listaPaginasInvertidas);

		if(paginaInvertida != NULL){
			pthread_mutex_lock(&m_listaPaginasInvertidas);
			while(contadorPosicion < inicio){//contadorPosicion dentro de una página
				if( (despl/datosConfigFM9->maximoLinea + numLinea)*datosConfigFM9->maximoLinea >= datosConfigFM9->tamanioPagina){
					cantPaginasSiguientes++;
					despl = 0;
					paginaInvertida = paginaInvertida->siguiente;
					contadorAuxiliar = 0;
					numLinea = 0;
				}
				if( *(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + contadorAuxiliar) == '\n'){ 
					numLinea++;
					inicio = numLinea * datosConfigFM9->maximoLinea + inicio - contadorPosicion -1;
					contadorPosicion = numLinea * datosConfigFM9->maximoLinea;	
					contadorAuxiliar = contadorPosicion;
				}
				else{ 
					contadorPosicion++;
					contadorAuxiliar++;
				}
			} 

			contadorPosicion = contadorAuxiliar;
			//salgo con contadorPosicion en el índice donde quiero empezar a leer dentro de una página

			char datosArchivo[tamanio + 1];
	
			paginaInvertida = list_find(tabla_paginasInvertidas, _buscarPaginaInvertida);
			for(int k = 0; k<cantPaginasSiguientes; k++){
				paginaInvertida = paginaInvertida->siguiente;
			}

			int i;
			pthread_mutex_lock(&m_memoria);
			for(i = 0; i<tamanio; i++){
				if( (despl/datosConfigFM9->maximoLinea + numLinea)*datosConfigFM9->maximoLinea >= datosConfigFM9->tamanioPagina){
					paginaInvertida = paginaInvertida->siguiente;
					contadorPosicion = 0;
					numLinea = 0;
					despl = 0;
				}
				
				if( *(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + contadorPosicion) == '\n'){ 
					datosArchivo[i] = *(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + contadorPosicion);
					numLinea++;
					contadorPosicion = numLinea * datosConfigFM9->maximoLinea;	
					//contadorAuxiliar = contadorPosicion;
				}
				else{ 
					datosArchivo[i] = *(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + contadorPosicion);
					contadorPosicion++;
				}	

				if(*(memoria + datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + contadorPosicion) == '\0'){
					ultima[0] = '1';
					break;
				}
			}
			pthread_mutex_unlock(&m_memoria);
			pthread_mutex_unlock(&m_listaPaginasInvertidas);
			if(i<tamanio)
				datosArchivo[i+1] = '\0';
			else
				datosArchivo[i] = '\0';

			log_trace(logger, "Se obtuvieron los siguientes datos: '%s' para el GDT %s", datosArchivo, args[0]);
			//en datosArchivo están los datos que leí
			runFunction(socketDAM, "FM9_DAM_respuestaFlush", 5 , args[0],
																 datosArchivo,
																 "1",
																 args[7],
																 ultima); 

		}

	}
	else{ //SPA

	}
}

//args[0]: idGDT, args[1]: pagina, args[2]: baseSegmento, args[3]: desplazamiento, args[4]:cantLineasArchivo
//args[5]: nomArchivo
void cerrarArchivoDeDTB(socket_connection* connection, char** args){
	int idGDT = atoi(args[0]);
	int baseSegmento = atoi(args[2]);
	int pagina = atoi(args[1]);
	int despl = atoi(args[3]);
	int cantLineasABorrar = atoi(args[4]);
	log_trace(logger, "Solicitud para cerrar el archivo %s por parte del GDT %s",args[5], args[0]);
	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &baseSegmento);	
	}
	bool _buscarPaginaInvertida(void* elemento){
		return buscarPaginaInvertida(elemento, &idGDT, &pagina);
	}
	void _iteracionLiberar(void* elemento){
		((t_PaginasInvertidas*)elemento)->pagina = 0;
		((t_PaginasInvertidas*)elemento)->PID = 0;
		((t_PaginasInvertidas*)elemento)->siguiente = NULL;
		((t_PaginasInvertidas*)elemento)->tamanioOcupado = 0;
		((t_PaginasInvertidas*)elemento)->libre = true;
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){
		pthread_mutex_lock(&m_listaSegmentos);
		t_tabla_segmentos* segmento = list_remove_by_condition(lista_tabla_segmentos, &_buscarSegmento);
		list_sort(lista_tabla_segmentos, (void*)&ordenarTablaSegmentosDeMenorBaseAMayorBase);
		pthread_mutex_unlock(&m_listaSegmentos);
		if(segmento == NULL){
			log_error(logger, "No se puede cerrar el archivo %s", args[5]);
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 , "0");
		}
		else{ 
			log_trace(logger, "Se ha cerrado el archivo %s y liberado la memoria que ocupaba",args[5]);
			char basura[segmento->limite];
			basura[0] = '\0';
			pthread_mutex_lock(&m_memoria);
			memcpy(memoria+segmento->base, basura, segmento->limite); //lleno el espacio con basura
			pthread_mutex_unlock(&m_memoria);
			free(segmento);
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 ,"1");
		}
	} else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		t_list* marcosLibres = list_create();
		pthread_mutex_lock(&m_listaPaginasInvertidas);
		t_PaginasInvertidas* paginaInvertida = list_find(tabla_paginasInvertidas, _buscarPaginaInvertida);
		if(paginaInvertida == NULL){
			log_error(logger, "No se puede cerrar el archivo %s", args[5]);
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 , "0");
			pthread_mutex_unlock(&m_listaPaginasInvertidas);
		}
		else{ 
			int contador = 0;
			pthread_mutex_lock(&m_memoria);
			for(int i=0; i<cantLineasABorrar; i++){
				int j = i - contador;
				if( despl + j*datosConfigFM9->maximoLinea < datosConfigFM9->tamanioPagina){//si no termina el marco 
					char basura[datosConfigFM9->maximoLinea];
					basura[0] = '\0';
					memcpy(memoria+datosConfigFM9->tamanioPagina*paginaInvertida->marco + despl + j*datosConfigFM9->maximoLinea,
					       basura, 
					       datosConfigFM9->maximoLinea); //lleno el espacio con basura
					paginaInvertida->tamanioOcupado -= datosConfigFM9->maximoLinea;
				}
				else{
					if(paginaInvertida->tamanioOcupado == 0){
						list_add(marcosLibres, (void*)paginaInvertida);
					}
					if ((paginaInvertida->siguiente)->PID == idGDT)
						paginaInvertida = paginaInvertida->siguiente;

					if(paginaInvertida->PID != idGDT){//Si no salió del bucle es porque falta leer
						//error
						log_error(logger, "Fallo de memoria, me pide cerrar un archivo cuyas lineas son menores a las solicitadas");
						runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 , "0");
						return;
					}
					else{
						contador = i;
						char basura[datosConfigFM9->maximoLinea];
						basura[0] = '\0';
						memcpy(memoria+datosConfigFM9->tamanioPagina*paginaInvertida->marco,
					   		   basura, 
					           datosConfigFM9->maximoLinea); //lleno el espacio con basura
						despl = 0;
						paginaInvertida->tamanioOcupado -= datosConfigFM9->maximoLinea;
						if(paginaInvertida->tamanioOcupado == 0){
							list_add(marcosLibres, (void*)paginaInvertida);
						}
					}
				}
			}
			pthread_mutex_unlock(&m_memoria);
			if(paginaInvertida->tamanioOcupado == 0){
				list_add(marcosLibres, (void*)paginaInvertida);
			}
			pthread_mutex_unlock(&m_listaPaginasInvertidas);
			log_trace(logger, "Se ha cerrado el archivo %s y liberado la memoria que ocupaba",args[5]);
			runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 1 ,"1");
		}
		list_iterate(marcosLibres, (void*)&_iteracionLiberar);
		list_destroy(marcosLibres);
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

bool buscarPaginaInvertida(t_PaginasInvertidas* pag, int* idGDT, int* pagina){
	return (pag->PID ==  *idGDT && pag->pagina == *pagina);
}


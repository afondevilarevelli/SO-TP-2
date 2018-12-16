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
		if(list_size(lista_tabla_segmentos)== i+1 && ( (auxNodo->base + auxNodo->limite + tamanioAPersistir) < datosConfigFM9->tamanio) ){
			return (auxNodo->base + auxNodo->limite);
		}
		auxNodoSiguiente = list_get(lista_tabla_segmentos, i+1);
		//Si hay espacio para persistir los datos entre nodos, devuelvo la posicion entre un nodo y el otro
		if((auxNodoSiguiente->base - (auxNodo->base + auxNodo-> limite)) > tamanioAPersistir)	{
			return (auxNodo->base + auxNodo->limite);
		}

	}
	//devuelvo error
	return -1;
}

retornoCargaTPI cargarArchivoTPI(int tamanioArchivo, char* arch, int idGDT){
	retornoCargaTPI retorno;
	int i, pagMasAlta = 0;
	int tamanioRestante = tamanioArchivo;
	
	for(i=0; i<list_size(tabla_paginasInvertidas); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
		if(unMarco->PID == idGDT){ 
			if(pagMasAlta < unMarco->pagina)
				pagMasAlta = unMarco->pagina;
		}
	}

	for(i=0; i<list_size(tabla_paginasInvertidas); i++){
		t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
		if(unMarco->PID == idGDT && unMarco->tamanioOcupado < datosConfigFM9->tamanioPagina){
			retorno.pagina = unMarco->pagina;
			retorno.desplazamiento = unMarco->tamanioOcupado; 
			if( (datosConfigFM9->tamanioPagina - unMarco->tamanioOcupado) >= tamanioArchivo){
				unMarco->tamanioOcupado += tamanioArchivo;
				memcpy(memoria + unMarco->marco*datosConfigFM9->tamanioPagina, arch, tamanioArchivo);
				tamanioRestante = 0;
				retorno.cargaOK = true;
				return retorno;
			}
			else{
				int tamanioParaCargar = tamanioArchivo - (datosConfigFM9->tamanioPagina - unMarco->tamanioOcupado);
				char auxBuffer[tamanioParaCargar];
				for(i=0; i<tamanioParaCargar; i++){
					auxBuffer[i] = *(arch + i);
				}
				unMarco->tamanioOcupado += tamanioParaCargar;
				memcpy(memoria + unMarco->marco*datosConfigFM9->tamanioPagina, auxBuffer, tamanioParaCargar);
				tamanioRestante = tamanioArchivo - tamanioParaCargar;
			}
		}
	}

	if(tamanioRestante != 0){ 
		for(i=0; i<list_size(tabla_paginasInvertidas); i++){
			t_PaginasInvertidas* unMarco = (t_PaginasInvertidas*) list_get(tabla_paginasInvertidas, i);
			if(unMarco->libre){ 
				char auxBuffer[tamanioRestante];
				for(i=0; i<tamanioRestante; i++){
					auxBuffer[i] = *(arch + tamanioArchivo - tamanioRestante + i);
				}
				unMarco->PID = idGDT;
				unMarco->pagina = pagMasAlta;
				unMarco->libre = false;
				unMarco->tamanioOcupado += tamanioRestante;
				memcpy(memoria + unMarco->marco*datosConfigFM9->tamanioPagina, auxBuffer, tamanioRestante);
				retorno.cargaOK = true;
				return retorno;
			}
		}
	}
	retorno.cargaOK = false;
	return retorno;
}

//args[0]: idGDT; args[1]: datos, args[2]: "ultima" ó "sigue" (para ver si dsp el DAM le avisa al SAFA o sigue cargando)
//args[3]: 1(Dummy) ó 0(no Dummy), args[4]: primer pedido o no
//Comunicacion para Desarrollar Cuando El DAM pida a FM9 cargar el archivo ya sea un DTB o el Dummy
void solicitudCargaArchivo(socket_connection *connection, char **args)
{
	int pid = atoi(args[0]);
	int tamanioArchivo = strlen(args[1])+1;
	log_info(logger, "Voy a persistir: '%s' cuyo tamanio es %d", args[1], strlen(args[1]));
	if(strcmp(datosConfigFM9->modo,"SEG")==0){ 
		

		int cantidadLineas = (tamanioArchivo/datosConfigFM9->maximoLinea)+1;
		log_info(logger, "Cantidad de lineas a utilizar: %d", cantidadLineas);
		int tamanioReal = cantidadLineas*datosConfigFM9->maximoLinea;
	
		log_info(logger, "Voy a buscar una posicion para almacenar los datos");
		int pos = devolverPosicionNuevoSegmento(tamanioReal);
		log_info(logger, "La posicion es %d", pos);
		if (pos != -1)
		{
			memcpy(memoria + pos, args[1], tamanioArchivo);
			log_info(logger, "Persisti el contenido");

			log_info(logger, "Voy a actualizar tabla de segmentos");
			t_tabla_segmentos *nuevoSegmento = malloc(sizeof(t_tabla_segmentos));

			nuevoSegmento->pid = pid;
			nuevoSegmento->base = pos;
			nuevoSegmento->limite = tamanioReal;
			list_add(lista_tabla_segmentos, nuevoSegmento);
			list_sort(lista_tabla_segmentos, (void*)&ordenarTablaSegmentosDeMenorBaseAMayorBase);
			log_info(logger, "Se actualizo correctamente la tabla de segmentos");
			if(strcmp(args[4],"0")==0)
				runFunction(socketDAM, "FM9_DAM_archivoCargado", 5, args[0], "ok", args[2], args[3], args[4]);
			else{ 
				char string_segmento[2];
				sprintf(string_segmento, "%i", pos);
				runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, args[0], "ok", args[2], args[3], args[4], "-1", string_segmento, "0");
			}
		}else{
			log_error(logger, "No se encontro una posicion dentro de la tabla de segmentos");
			log_error(logger, "No se pudo persistir los datos");
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 5, args[0], "error", args[2], args[3], args[4]);
		}
	}
	else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		int idGDT = atoi(args[0]);
		retornoCargaTPI cargado = cargarArchivoTPI(tamanioArchivo, args[1], idGDT);
		if(cargado.cargaOK){ 
			log_info(logger, "Persisti el contenido %s para el GDT %d", args[1], idGDT);
			if(strcmp(args[4],"0")==0)
				runFunction(socketDAM, "FM9_DAM_archivoCargado", 5, args[0], "ok", args[2], args[3], args[4]);
			else{
				char string_despl[3];
				sprintf(string_despl, "%i", cargado.desplazamiento);
				char string_pagina[3];
				sprintf(string_pagina, "%i", cargado.pagina);
				runFunction(socketDAM, "FM9_DAM_archivoCargado", 8, args[0], "ok", args[2], args[3], args[4], string_pagina, "-1",string_despl);
			}
		}else{
			log_info(logger, "Error al persistir el contenido %s para el GDT %d",args[1], idGDT);
			runFunction(socketDAM, "FM9_DAM_archivoCargado", 5, args[0], "error", args[2], args[3], args[4]);
		}
	}
	else{ //SEGMENTACION PAGINADA
		printf("No esta implementado papu\n");
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

//args[0]: idGDT, args[1]: base, args[2]:Linea, args[3]:Datos
void actualizarDatosDTB(socket_connection* connection, char** args){

	int idGDT = atoi(args[0]);
	int base = atoi(args[1]);
	int linea = atoi(args[2]);
	char* datos = args[3];

	log_info(logger, "Del GDT: %d, recibi los siguientes Datos: %d, %d, %s", idGDT, base, linea, datos);

	bool _buscarSegmento(void* elemento){
		return buscarSegmento(elemento, &idGDT, &base);	
	}

	//Me fijo si existe en la tabla de segmentos
	t_tabla_segmentos* datosSegmento = list_find(lista_tabla_segmentos, _buscarSegmento);

	if(datosSegmento == NULL){
		log_error(logger, "TODO: devolver RunFuction de errors: no existe el segmento pedido");
		return;
	}

	//me fijo si el espacio que tiene le alcanza y actualizo
	//sino busco un nuevo hueco donde persistir los datos
	if((datosSegmento->limite - linea)>strlen(datos)){
		memcpy(memoria+base+linea, datos, strlen(datos));
		log_error(logger, "TODO: devolver RunFuction de ok");
	}else{
		int cantidadLineas = ((datosSegmento->limite-(datosSegmento->limite-linea) + strlen(datos))/datosConfigFM9->maximoLinea)+1;
		log_info(logger, "Cantidad de lineas a utilizar: %d", cantidadLineas);
		int tamanioReal = cantidadLineas*datosConfigFM9->maximoLinea;
		log_info(logger, "Voy a buscar una posicion para almacenar los datos");
		int pos = devolverPosicionNuevoSegmento(tamanioReal);
		log_info(logger, "La posicion es %d", pos);

		if (pos != -1)
		{
			memcpy(memoria + pos, memoria+datosSegmento->base, datosSegmento->limite);
			memcpy(memoria+pos+linea, datos, strlen(datos));
			log_info(logger, "Persisti el contenido");

			log_info(logger, "Voy a actualizar tabla de segmentos");

			datosSegmento->base = pos;
			datosSegmento->limite = tamanioReal;
			list_sort(lista_tabla_segmentos, (void*)&ordenarTablaSegmentosDeMenorBaseAMayorBase);
			log_info(logger, "Se actualizo correctamente la tabla de segmentos");
			log_error(logger, "TODO: devolver RunFuction de ok con la nueva posicion en memoria");
		}
		else
		{
			log_error(logger, "No se encontro una posicion dentro de la tabla de segmentos");
			log_error(logger, "No se pudo persistir los datos");
			log_error(logger, "TODO: devolver RunFuction de errors");
		}


	}
	
}

//args[0]: idGDT, args[1]: rutaScript
void cerrarArchivoDelDTB(socket_connection* connection, char** args){

	//Realizar Operacion de Cerrado

	runFunction(connection->socket, "FM9_CPU_resultadoDeClose", 2 , args[0], "1");

}

bool ordenarTablaSegmentosDeMenorBaseAMayorBase(t_tabla_segmentos* unNodo, t_tabla_segmentos* nodoSiguiente){
	return unNodo->base < nodoSiguiente->base;
}

bool buscarSegmento(t_tabla_segmentos* unNodo, int* pid, int* base){
	return (unNodo->pid ==  *pid && unNodo->base == *base);
}


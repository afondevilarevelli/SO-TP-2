#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

//ESTRUCTURAS
typedef struct {
	int puerto;
	char* modo;
	int tamanio;
	int maximoLinea;
	int tamanioPagina;
} t_config_FM9;

//VARIABLES
t_log* logger;
t_config_FM9* datosConfigFM9;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//CONFIG
void read_and_log_config(char*);

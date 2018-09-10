#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>


//ESTRUCTURAS
typedef struct {
	int puerto;
	char* algoritmoPlanif;
	int quantum;
	int gradoMultiprog;
	int retardo;
} t_config_SAFA;

//VARIABLES
t_log* logger;
t_config_SAFA* datosConfigSAFA;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//CONFIG
void read_and_log_config(char*);
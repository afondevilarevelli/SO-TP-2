#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

//ESTRUCTURAS
typedef struct {
	int puertoEscucha;
	char* IPSAFA;
	int puertoSAFA;
    char* IPMDJ;
    int puertoMDJ;
    char* IPFM9;
    int puertoFM9;
    int transferSize;
} t_config_FM9;

//VARIABLES
t_log* logger;
t_config_FM9* datosConfigDAM;

//FUNCIONES
//LOGS
void configure_logger();
void close_logger();

//CONFIG
void read_and_log_config(char*);

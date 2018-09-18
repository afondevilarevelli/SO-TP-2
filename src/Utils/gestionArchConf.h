#ifndef GESTIONARCHCONF_H_
#define GESTIONARCHCONF_H_


#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <stdlib.h>
#include <errno.h>



int existeArchivoConf(char *);
void crearArchivoConfig(char * );
void  setKey(char *,char *);
void setValue(t_config *,char *,char *,char *);
char * obtenerString(t_config *,char *,char *);
int  obtenerInt(t_config *,char *, char *);
int configPoseeKey(t_config *,char *,char *);





#endif /* GESTIONARCHCONF_H_ */

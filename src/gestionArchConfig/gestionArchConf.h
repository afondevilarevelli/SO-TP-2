/*
 * gestionArchConfing.h
 *
 *  Created on: 3 sep. 2018
 *      Author: utnso
 */

#ifndef S_AFA_GESTIONARCHCONF_H_
#define S_AFA_GESTIONARCHCONF_H_


#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <stdlib.h>
#include <errno.h>



int existeArchivoConf(char *);
void crearArchivoConfig(char * );
void  setKey(char *,char *);
void setValue(char *,char *,char *);
char * obtenerString(char *,char *);
int  obtenerInt(char *, char *);
int configPoseeKey(char *,char *);





#endif /* S_AFA_GESTIONARCHCONF_H_ */

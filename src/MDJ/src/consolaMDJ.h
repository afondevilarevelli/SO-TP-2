#ifndef CONSOLA_MDJ_H
#define CONSOLA_MDJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <commons/string.h>
#include <dirent.h>

//definicion de funciones
void consolaMDJ();
void ls(char *);
void cd(char * );
void md5(char * );
void cat(char *);



#endif
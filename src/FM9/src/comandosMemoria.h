#ifndef SRC_COMANDOSMEMORIA_H_
#define SRC_COMANDOSMEMORIA_H_

#include <string.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "socket.h"
#include "libFM9.h"
#include <readline/readline.h>

typedef struct
{
	char * name;
	void(*fn)();
	int args;
	char * description;
} Command;


//Command* 	cmd;
t_list* 	commands;

void loadCommands();

void executeCommand(char*);

#endif /* SRC_COMANDOSMEMORIA_H_ */

bool isPid(t_tabla_segmentos*, int*);
bool pagInv(t_PaginasInvertidas* elemento, int* pid);
void mostrarDatosPid(t_tabla_segmentos*);
void consolaFM9();
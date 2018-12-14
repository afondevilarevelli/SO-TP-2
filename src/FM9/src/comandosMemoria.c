#include "comandosMemoria.h"

// Crea un nuevo comando
Command * newCommand(char * name, void (*fn)(), int args, char * description) {
	cmd = malloc(sizeof(Command));
	cmd->name = name;
	cmd->fn = fn;
	cmd->args = args;
	cmd->description = description;
	return cmd;
}

// devuelve el length de un char **
// todo esto funciona realmente?
int charArray_length(char ** array) {
	int i = 0;
	while(array[i] != NULL) {
		i++;
	}
	return i;
}


// Libera de memoria un char ** (testeado solo con generados de split)
void freeCharArray(char ** array) {
	int i;
	for(i = 0; i < charArray_length(array); i++) {
		free(array[i]);
	}
	free(array);
}


// Imprime menu de comandos
void printMenu() {
	int i;

	log_info(logger, "Listado de comandos disponibles:");
	for(i = 0; i < list_size(commands); i++)
	{
		cmd = list_get(commands, i);
		log_info(logger, "   %-20s (%d)   %s", cmd->name, cmd->args, cmd->description);
	}
	//puts("\n");
}

void dump(char** args)
{
	int* pid = atoi(args[1]);
	
	bool _isPid(void* elemento){
		return isPid(elemento, pid);
	}

	t_list* lista_segmentos_pid;
	lista_segmentos_pid = list_filter(lista_tabla_segmentos, _isPid);
	log_info(logger, "%d", list_size(lista_segmentos_pid));
	list_iterate(lista_segmentos_pid, mostrarDatosPid);

	list_destroy(lista_segmentos_pid);
}

// Carga de lista de comandos
void loadCommands()
{
  commands = list_create();
  list_add(commands, newCommand("help", &printMenu, 0, "()~$: Menu de ayuda."));
  list_add(commands, newCommand("dump", &dump, 1, "()~$: Hacer dump."));
}


// Valida y ejecuta comando especifico
void executeCommand(char * c){
  c[strlen(c)-1] = '\0'; //quito /n final

  if(string_is_empty(c) || c[0] == ' ' || c[strlen(c)-1] == ' ') {
		log_warning(logger, "Ingrese comando, escribe *help* para ver el menú");
    return;
  }

  char ** args = string_split(c, " ");
  char * name = args[0];

  int i;
  for(i = 0; i < name[i]; i++)
  name[i] = tolower(name[i]);

  bool _get(Command * cc) {
    return strcmp(cc->name, name) == 0;
  }

  cmd = list_find(commands,(void*) _get);

  if(cmd == NULL) {
		log_error(logger, "Comando inexistente.");
  } else if(charArray_length(args) - 1 != cmd->args) {
    log_error(logger, "La cantidad de parametros no coincide [%d/%d]", charArray_length(args) - 1, cmd->args);
  } else {
    cmd->fn(args);
  }

  freeCharArray(args);
}

bool isPid(t_tabla_segmentos* unNodo, int pidIngresado){
	return unNodo->pid == pidIngresado;
}

void mostrarDatosPid(t_tabla_segmentos* unNodo){
	log_info(logger, "PID: %d			base: %d			offset: %d", unNodo->pid, unNodo->base, unNodo->limite);
	char* datos = malloc(unNodo->limite);
	memcpy(datos, memoria + unNodo->base, unNodo->limite);
	log_info(logger, "Datos almacenados: %s", datos);
}
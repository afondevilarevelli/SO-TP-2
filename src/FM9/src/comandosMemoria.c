#include "comandosMemoria.h"

// Crea un nuevo comando
Command * newCommand(char * name, void (*fn)(), int args, char * description) {
	Command* cmd = malloc(sizeof(Command));
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
		Command* cmd = list_get(commands, i);
		log_info(logger, "   %-20s (%d)   %s", cmd->name, cmd->args, cmd->description);
	}
	//puts("\n");
}

void dump(char** args)
{
	int pid = atoi(args[1]);
	
	bool _isPid(void* elemento){
		return isPid(elemento, &pid);
	}
	bool _pagInv(void* elemento){
		return pagInv(elemento, &pid);
	}
	bool _sorted(void* elemento1,void* elemento2){
		return ( (t_PaginasInvertidas*) elemento1 )->pagina < ( (t_PaginasInvertidas*) elemento2 )->pagina;
	}
	if(strcmp(datosConfigFM9->modo,"SEG")==0){ 
		t_list* lista_segmentos_pid;
		lista_segmentos_pid = list_filter(lista_tabla_segmentos, _isPid);
		if(list_size(lista_segmentos_pid) != 0){ 
			log_info(logger, "%d", list_size(lista_segmentos_pid));
			list_iterate(lista_segmentos_pid, &mostrarDatosPid);
			list_destroy(lista_segmentos_pid);
		} else
			log_info(logger, "No hay datos");
	}
	else if(strcmp(datosConfigFM9->modo,"TPI")==0){
		t_list* lista_pagsInv;
		lista_pagsInv = list_filter(tabla_paginasInvertidas, _pagInv);
		if(list_size(lista_pagsInv) != 0){ 
			list_sort(lista_pagsInv, &_sorted);
			for(int j=0; j< list_size(lista_pagsInv); j++){
				t_PaginasInvertidas* pag = list_get(lista_pagsInv, j);

				int cantLineas = pag->tamanioOcupado / datosConfigFM9->maximoLinea;
				char* linea;
				linea = malloc(datosConfigFM9->maximoLinea);
				log_trace(logger, "Marco: %d   Pagina: %d   PID: %d", pag->marco, pag->pagina, pag->PID);
				log_info(logger, "Datos almacenados:");
				for(int i=0; i<cantLineas; i++){
					memcpy(linea, memoria + datosConfigFM9->tamanioPagina*pag->marco + i*datosConfigFM9->maximoLinea , datosConfigFM9->maximoLinea);
					log_info(logger, "	%s", linea);
				}
				free(linea);
			}
		}
		else	
			log_info(logger, "No hay datos");
		list_destroy(lista_pagsInv);
	} else{ //SPA

	}

}

bool isPid(t_tabla_segmentos* unNodo, int* pidIngresado){
	return unNodo->pid == *pidIngresado;
}

bool pagInv(t_PaginasInvertidas* elemento, int* pid){
	return elemento->PID == *pid;
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
    c[strlen(c)-1] = '\0'; //quito \n final

  if(string_is_empty(c) || c[0] == ' ' || c[strlen(c)-1] == ' ') {
		log_warning(logger, "Ingrese comando, escribe *help* para ver el menú");
    return;
  }

  char ** args = string_split(c, " ");
  char * name = args[0];

  int i;
  for(i = 0; i < strlen(name); i++)
  	name[i] = tolower(name[i]);

  bool _get(Command * cc) {
    return strcmp(cc->name, name) == 0;
  }

  Command* cmd = list_find(commands,(void*) _get);

  if(cmd == NULL) {
		log_error(logger, "Comando inexistente.");
  } else if(charArray_length(args) - 1 != cmd->args) {
    log_error(logger, "La cantidad de parametros no coincide [%d/%d]", charArray_length(args) - 1, cmd->args);
  } else {
    cmd->fn(args);
  }

  freeCharArray(args);
}

void mostrarDatosPid(t_tabla_segmentos* unNodo){
	int cantLineas = unNodo->limite / datosConfigFM9->maximoLinea;
	char* linea;
	linea = malloc(datosConfigFM9->maximoLinea);
	log_trace(logger, "PID: %d  Base: %d   Offset: %d", unNodo->pid, unNodo->base, unNodo->limite);
	log_info(logger, "Datos almacenados:");
	for(int i=0; i<cantLineas; i++){
		memcpy(linea, memoria + unNodo->base + i*datosConfigFM9->maximoLinea , datosConfigFM9->maximoLinea);
		log_info(logger, "	%s", linea);
	}
	free(linea);
}

void consolaFM9(){
	char *buffer;
	size_t bufsize = 1024;
	buffer = (char *)malloc(bufsize * sizeof(char));
	void _destructor(void* comando){
		free( ( (Command*) comando )->name );
		free( ( (Command*) comando )->description );
	}
	while(1) {
		log_info(logger, "Ingrese un Comando");
		getline(&buffer, &bufsize, stdin);
		while(buffer == NULL)
			getline(&buffer, &bufsize, stdin);
		if(strcmp(buffer, "salir\n") != 0)
			executeCommand(buffer);
		else
			break;
	}
	log_info(logger, "Se ha cerrado la consola");
	if(buffer != NULL)
		free(buffer);
	list_destroy_and_destroy_elements(commands, (void*)&_destructor);
}
#include "consolaSAFA.h"

void consolaSAFA(/* mas adelante ver si lleva parametros*/){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE

	do{

		free(linea);
		linea = readline(">");
		char* p1 = strtok(linea,espaBlan);    // token que apunta al primer parametro ( la palabra reservada )
		char* p2 = strtok(NULL,espaBlan);	 // token que apunta al segundo parametro

		if( p1 == NULL )
			continue;
		else
		if(strcmp(p1,"ejecutar") == 0 && p2 != NULL)
		{
			printf("Se va a ejecutar el script escriptorio de la ruta = %s\n", p2);
			ejecutar(p2);
		}
		else
		if(strcmp(p1,"finalizar") == 0 && p2 != NULL)
		{
            int id = atoi(p2); //convertir p2 a int
			printf("se ha finalizado el G.DT con id = %d\n", id);
			finalizar(id);
		}
		else
		if(strcmp(p1,"status") == 0 )
		{
            int id;
            if(p2 == NULL){
                id = 0;
                printf("Estado de las colas:\n");
                status(0);
            }
            else{
                id = atoi(p2);	//convertir p2 a int
			    printf("Datos del DTB del G.DT de id = %d\n", id);
			    status(id);
            }
		}
		else
		if(strcmp(p1,"metricas") == 0 && p2 == NULL)
		{
			printf("Se detallaran las metricas del sistema:\n");
            metricas();
		}
		else
		{
			debeContinuar = strcmp(linea, "salir");
			if(debeContinuar) printf("comando no reconocido\n");
		}

	}while(debeContinuar);

	free(linea);

	exit(0);

	return;
}

void ejecutar(char* rutaScript){

}

void finalizar(int inGDT){

}

void status(int idGDT){ 
    if(idGDT == 0){ // funcion status sin parametro

    }
    else{ //funcion status con parametro

    }
}

void metricas(){

}
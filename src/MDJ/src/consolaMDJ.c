#include "consolaMDJ.h"
#define MAXDIR 100

  char bufferDirAct[150];
   char hostname [150];
 

void consolaMDJ(){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE
        getlogin_r( hostname,sizeof( hostname ));
        getcwd( bufferDirAct, sizeof( bufferDirAct));

	do{

		free(linea);
               printf("%s",hostname);
		linea = readline(" >");
		char* p1 = strtok(linea,espaBlan);    // token que apunta al primer parametro ( la palabra reservada )
		char* p2 = strtok(NULL,espaBlan);	 // token que apunta al segundo parametro

		if( p1 == NULL )
			continue;
		else if(strcmp(p1,"ls") == 0)
		{
                 if (p2 != NULL){
                    printf( "%s:",bufferDirAct);
		     ls(p2);
                  printf("\n");
                  }
             else 
                { 
                getcwd( bufferDirAct, sizeof( bufferDirAct));
                 printf("%s:",bufferDirAct);           
                 ls(""); 
               printf("\n");
                }
                }
		else if(strcmp(p1,"cd") == 0 && p2 != NULL)
		{
                printf("%s :" ,hostname);
                cd(p2);
		}
		else if(strcmp(p1,"md5") == 0  && p2 != NULL )
		{
                md5(p2);
                }
		else if(strcmp(p1,"cat") == 0 && p2 != NULL)
		{	
            cat(p2);
		}
		else
		{
			debeContinuar = strcmp(linea, "exit");
			if(debeContinuar) printf("comando no reconocido\n");
		}

	}while(debeContinuar);

	free(linea);

	exit(0);

	return;
}

void ls(char * pathD){
//nombre del directorio
const char * dir_name;
//puntero al prox direc
DIR * dir_ptr = NULL;
//estructura de directorios
struct dirent * dirent_ptr;
//inicializo maxima cant de directorios a mostrar
int count = 0;
//nombre del directorio
dir_name =pathD;


//recursividad papa
// si no recibe parametro ,muestra el directorio actual
if (string_is_empty(pathD) == 1)
{
ls(".");
}
else if ( ( dir_ptr = opendir( dir_name) ) == NULL ) {
		printf( "No existe o no se pudo abrir el directorio '%s'\n", dir_name );
		return -1;
	}
else
{
	while ( count < MAXDIR  && ( dirent_ptr = readdir( dir_ptr ) ) != NULL ) {
                count++;
		printf( " %s" ,dirent_ptr->d_name );
	}
}
/* cierra el directorio */
	if ( dir_ptr != NULL ) closedir( dir_ptr );
        return 0;

}

void cd(char* pathD){
int retorno = chdir(pathD);
getcwd( bufferDirAct, sizeof( bufferDirAct));
if ( strcmp(pathD,".") == 0)  system("cd .");
if (strcmp(pathD,"..") == 0)  system("cd ..");
if (retorno == -1)
{
printf("No se encontro el directorio %s \n ",pathD );
}
else
{
printf("%s  \n",bufferDirAct);
}
}

void md5(char* pathA){ 

}

void cat(char* pathA){

}


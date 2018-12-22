#include "consolaMDJ.h"


#define MAXDIR 150
#define BUZZ_SIZE 1024
#define MAX_CMD_LEN  128
#define HISTORY_COUNT 20



void consolaMDJ(){
	char* linea=NULL;
	char espaBlan[4]=" \n\t";
	int debeContinuar = 1; //TRUE
        getcwd( bufferDirAct, sizeof( bufferDirAct));
        strcat(bufferDirAct,">");
 
	do{
               free(linea);
		linea = readline(bufferDirAct);
                // con uprow y downrow vuelve al comando anterior o posterior
                if(strlen(linea) > 0){
                add_history(linea);
                }
		char* p1 = strtok(linea,espaBlan);    // token que apunta al primer parametro ( la palabra reservada )
		char* p2 = strtok(NULL,espaBlan);	 // token que apunta al segundo parametro
		if( p1 == NULL )
	        continue;
		else if(strcmp(p1,"ls") == 0){
                 if (p2 != NULL){
                  printf("%s",bufferDirAct);
		  ls(p2);
                  printf("\n");
                  }
             else {       
               printf("%s",bufferDirAct);
                ls(""); 
                printf("\n");
                }
                }
		else if(strcmp(p1,"cd") == 0 && p2 != NULL){
                cd(p2);
		}
		else if(strcmp(p1,"md5") == 0  && p2 != NULL ){
                md5(p2);
                }
		else if(strcmp(p1,"cat") == 0 && p2 != NULL){	
            cat(p2);
		}
             else if (strcmp(p1,"clear") ==  0){
                system("clear");
                }
		else{
			debeContinuar = strcmp(linea, "exit");
			if(debeContinuar) printf("%s>comando no reconocido\n",bufferDirAct);
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
        // si no recibe parametro ,muestra el  contenido del directorio actual
        if (string_is_empty(pathD) == 1)
        {
                ls(".");
        }
        else if ( ( dir_ptr = opendir( dir_name) ) == NULL ) {
		printf( " No existe  el directorio '%s'\n",dir_name );
		return;
	     }
             else
             {
	        while ( count < MAXDIR  && ( dirent_ptr = readdir( dir_ptr ) ) != NULL ) {
                        count++;
		        printf( "\e[96m %s \e[0m" ,dirent_ptr->d_name );
	        }
             }
        /* cierra el directorio */
	if ( dir_ptr != NULL ) closedir( dir_ptr );
        return;
}

void cd(char* pathD){
        char bufferDirAnt[MAXDIR];
        int retorno = chdir(pathD);
        getcwd( bufferDirAct, sizeof( bufferDirAct));
        getcwd( bufferDirAnt, sizeof( bufferDirAct) - 1);
        strcat(bufferDirAct,">");
        strcat(bufferDirAnt,">");
        if ( strcmp(pathD,".") == 0)  chdir(bufferDirAct);  
        if (strcmp(pathD,"..") == 0)  chdir(bufferDirAnt);
        if (retorno == -1)
        {
                printf("%s>No se encontro el directorio %s \n ",bufferDirAct, pathD );
        }
}

void md5(char* pathA){ 
        //tamaño del hash generado
        unsigned char result[MD5_DIGEST_LENGTH];
        int file_descript;
        unsigned long file_size;
        //buffer donde guardo el contendio del archivo
        char* file_buffer;
        file_descript = open(pathA, O_RDONLY);
        log_info(logger,"Intentando generar hash md5 de %s",pathA);
        // si no hay nada para generar...
        if(file_descript < 0) { 
                log_error(logger,"No se pudo generar md5 de %s",pathA);
                printf("%s>No se pudo generar md5 de %s\n",bufferDirAct,pathA);
                //consolaMDJ();
        }
        file_size = get_size_by_fd(file_descript);
        //mapeo en memoria el archivo
        file_buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
        //genero el md5
        MD5((unsigned char*) file_buffer, file_size, result);
        //lo saco de la memoria
        munmap(file_buffer, file_size); 
        log_trace(logger,"Hash  de %s generado correctamente",pathA);
        printf("%s>MD5 %s:",bufferDirAct,pathA);
        print_md5_sum(result);
        printf("\n");
}


void cat(char* pathA){
char *buffer = NULL;
int string_size, read_size;
FILE * handler = fopen(pathA, "r");
log_info(logger,"Voy a intentar mostrar el contenido de %s",pathA);
 if (handler) {
       fseek(handler, 0, SEEK_END);
       //Ofsset desde el primer byte hasta el ultimo... el tam del archivo
       string_size = ftell(handler);
       //Hago que vuelva al principio
       rewind(handler);
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
       read_size = fread(buffer, sizeof(char), string_size, handler);
       buffer[string_size] = '\0';
       if (string_size != read_size)
       {
           buffer = NULL;
       }
       fclose(handler);
     log_trace(logger,"Archivo %s leido correctamente",pathA);  
     printf("\e[1m\e[37m%s\e[0m\e[0m", buffer);
    }
  else
   {
    log_error(logger,"No pudo leerse el archivo %s ",pathA);
    printf("%sNo pudo leerse el archivo %s\n ", bufferDirAct, pathA);
    
    }
    free(buffer);
}


/*
void cat(char* pathA){
char* pathArch = string_new();
pathArch = string_substring(bufferDirAct,0,strlen(bufferDirAct) - 1);
string_append(&pathArch, "/");
string_append(&pathArch, pathA);
int string_size;
FILE * handler = fopen(pathA, "r");
log_info(logger,"Voy a intentar mostrar el contenido de %s",pathA);
int lolete = 1;

int h;
for( h=0; h<strlen(bufferDirRaiz);h++){
        bufferDirRaizAux[h] = bufferDirRaiz[h];
}
bufferDirRaizAux[h] = '/';
bufferDirRaizAux[h+1] = '\0'; 

 if (handler) {
         char ** bloques = obtenerBloquesDesdeConsola(pathArch);
         fclose(handler);
         char rutaBloq[300];
         for(int i=0; i<cantElementos2(bloques); i++){
                 char buffer[fs->tamanio_bloques];
                 char* bufferDirRaizAux = malloc( strlen(bufferDirRaiz)+1 + strlen(datosConfMDJ->ptoMontaje)+1 + 2 + 10 );
                strcpy(bufferDirRaizAux, bufferDirRaiz);
                strcat(bufferDirRaizAux, "/");
                strcat(bufferDirRaizAux, datosConfMDJ->ptoMontaje);
                strcat(bufferDirRaizAux, "/Bloques/");

                 
                //rutaBloq = malloc(strlen(bufferDirRaizAux) + strlen(bloques[i]) + 7);
                 strcpy(rutaBloq, bufferDirRaizAux);              
                 strcat(rutaBloq, bloques[i]);
                 strcat(rutaBloq, ".bin");
                 for(int q=0; q<strlen(bufferDirRaizAux);q++){
                         rutaBloq[q] = *(bufferDirRaizAux + q);
                 }
                 for(int q=0; q<strlen(datosConfMDJ->ptoMontaje);q++){
                         rutaBloq[strlen(bufferDirRaiz)+q] = *(datosConfMDJ->ptoMontaje + q);
                 }
                 for(int r=0; r<strlen(bloquesString);r++){
                        rutaBloq[strlen(bufferDirRaiz)+strlen(bufferDirRaiz)+r] = *(bloquesString + r);
                 }
                  for(int r=0; r<strlen(bloques[i]);r++){
                        rutaBloq[strlen(bufferDirRaiz)+strlen(bufferDirRaiz)+strlen(bloquesString)+r] = *(bloques[i] + r);
                 }
                 for(int r=0; r<strlen(elbin);r++){
                        rutaBloq[strlen(bufferDirRaiz)+strlen(bufferDirRaiz)+strlen(bloquesString)+strlen(bloques[i])+r] = *(elbin + r);
                 }

                FILE * archBloque = fopen(rutaBloq, "r");
                fseek(archBloque, 0, SEEK_END);
                //Ofsset desde el primer byte hasta el ultimo... el tam del archivo
                string_size = ftell(archBloque);
                fseek(archBloque, 0, SEEK_SET);
                fread(buffer, sizeof(char), string_size, archBloque);
                log_trace(logger,"Bloque %s:", bloques[i]);
                printf("%s\n", buffer);
                fclose(archBloque);
                //free(rutaBloq);
                free(bufferDirRaizAux);
         }
     log_trace(logger,"Archivo %s leido correctamente",pathA);  
    }
  else
   {
    log_error(logger,"No pudo leerse el archivo %s ",pathA);
    printf("%sNo pudo leerse el archivo %s\n ", bufferDirAct, pathA);
    
    }
    
     
}*/

void print_md5_sum(unsigned char* md) {
    int i;
    for(i=0; i <MD5_DIGEST_LENGTH; i++) {
            printf("\e[33m%02x\e[0m",md[i]);
    }
}

unsigned long get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}



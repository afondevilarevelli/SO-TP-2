#include  "Interfaz.h"

int validarArchivo(char * path){
int fileState = open(path,O_RDONLY);
if(fileState == -1){
return 0;
log_error(logger,"No se econtro el archivo solicitado");
}
else{
return 1;
log_info(logger,"Se encontro el archivo solicitado");
}
}

void crearArchivo(char * path,size_t *   sizeText)
{

}

long  obtenerDatos(char * path,off_t  * offset,size_t  * size){

}

void guardarDatos(char * path,off_t  * offset,size_t *   size,char * buffer){

}

void borrarArchivo(char * path){


}



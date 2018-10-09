#include  "Interfaz.h"


void  validarArchivo(socket_connection * connection, char ** args){
char strEstado[10];
int estado;
int fileState = open(args[0],O_RDONLY);
if(fileState == -1){
estado =0;
}
else{
estado =1;
}
sprintf(strEstado, "%i", estado);
runFunction(connection->socket,"MDJ_DAM_existeArchivo",1,strEstado);
}

void crearArchivo(char * path,size_t *   sizeText)
{

}

size_t  obtenerDatos(char * path,off_t  * offset,size_t  * size){

}

void guardarDatos(char * path,off_t  * offset,size_t *   size,char * buffer){

}

void borrarArchivo(char * path){


}




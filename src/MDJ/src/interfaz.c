#include  "interfaz.h"

// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
int estado;
int file;
char strEstado[2];

aplicarRetardo(char *path)
{
t_config* fileConfig  = config_create(path);
int ret = config_get_int_value(fileConfig,"RETARDO");
sleep(ret);
config_destroy(fileConfig);
}

void  validarArchivo(socket_connection * connection,char ** args){
char * path = args[0];
file = verificarSiExisteArchivo(path);
if(file == noExiste){
estado =noExiste;
}
else {
estado =  existe;
}
sprintf(strEstado, "%i", estado);
aplicarRetardo("MDJ.config");
runFunction(connection->socket,"MDJ_DAM_existeArchivo",1,strEstado);
}

void crearArchivo(socket_connection * connection ,char * path,size_t *   sizeText)
{
file = verificarSiExisteArchivo(path);
if(file == existe)
{
estado = yaCreado;
}
else if (file == noExiste)
{
estado = recienCreado;
}
else
{
estado = noCreado;
}
sprintf(strEstado, "%i", estado);
aplicarRetardo("MDJ.config");
runFunction(connection->socket,"MDJ_DAM_verificarArchivoCreado",1,strEstado);
}

size_t  obtenerDatos(socket_connection * connection,char * path,off_t  * offset,size_t  * size){

}

void guardarDatos(socket_connection * connection ,char * path,off_t  * offset,size_t *   size,char * buffer){
}

void borrarArchivo(socket_connection* connection,char * path){
file = verificarSiExisteArchivo(path);
if (file == existe)
{
remove(path);
estado = recienBorrado;
}
else
{
estado = noBorrado;
}
sprintf(strEstado, "%i", estado);
aplicarRetardo("MDJ.config");
runFunction(connection->socket,"MDJ_DAM_verificameSiArchivoFueBorrado",1,strEstado);
}

int verificarSiExisteArchivo(char * path){
int fileState = open(path,O_RDONLY);
if(fileState == -1){
return noExiste;
}
else{
return existe;
}
}



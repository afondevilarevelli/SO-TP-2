#include  "interfaz.h"
//https://www.programacion.com.py/escritorio/c/archivos-en-c-linux
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
int estado;
t_archivo archivo;
char strEstado[2];

directorioMontaje = datosConfMDJ->ptoMontaje;

aplicarRetardo(char *path)
{
t_config* fileConfig  = config_create(path);
int ret = config_get_int_value(fileConfig,"RETARDO");
sleep(ret);
config_destroy(fileConfig);
}

void  validarArchivo(socket_connection * connection,char ** args){
archivo->path = args[0];
archivo->fd =  verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
estado =noExiste;
}
else {
estado =  existe;
}
sprintf(strEstado, "%i", estado);
aplicarRetardo("MDJ.config");
runFunction(connection->socket,"MDJ_DAM_existeArchivo",1,strEstado);
}

//void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

void crearArchivo(socket_connection * connection ,char * path,size_t sizeText)
{
archivo->path = path;
archivo->size = sizeText;
archivo->fd = verificarSiExisteArchivo(path);
if(archivo->fd == existe)
{
estado = yaCreado;
}
else if (archivo->fd == noExiste)
{
archivo->mem_ptro = mmap(void,sizeText,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,archivo->fd,0);
write(archivo->fd,"",archivo->sizeText);
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

//off_t lseek(int fildes, off_t offset, int whence);
size_t  obtenerDatos(socket_connection * connection,char * path,off_t  * offset,size_t  * size){

}

void guardarDatos(socket_connection * connection ,char * path,off_t  * offset,size_t *   size,char * buffer){
}

void borrarArchivo(socket_connection* connection,char * path){
archivo->path = path;
archivo->fd  = verificarSiExisteArchivo(path);
if (archivo->fd  == existe)
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
archivo->path = path;
archivo->fd = open(path,O_RDONLY|0_CREAT);
if(archivo->fd  == -1){
return noExiste;
}
else{
return existe;
}
}



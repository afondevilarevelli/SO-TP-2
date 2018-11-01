#include  "interfaz.h"
#include "libMDJ.h"
//https://www.programacion.com.py/escritorio/c/archivos-en-c-linux
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
t_archivo *  archivo;
char strEstado[2];

char * obtenerPtoMontaje()
{
t_config* fileConfig  = config_create("MDJ.config");
char * ptoMontaje = config_get_string_value(fileConfig,"ptoMontaje");
config_destroy(fileConfig);
return ptoMontaje;
}

aplicarRetardo()
{
t_config* fileConfig  = config_create("MDJ.config");
int ret = config_get_int_value(fileConfig,"RETARDO");
sleep(ret);
config_destroy(fileConfig);
}

void  validarArchivo(socket_connection * connection,char ** args){
archivo->path = args[0];
archivo->fd =  verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
archivo->estado =noExiste;
}
else {
archivo->estado=  existe;
}
sprintf(strEstado, "%i", archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_existeArchivo",1,strEstado);
}

//void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

//args[0]: rutaDelArchivo, args[1]: cantidadDeBytes
void crearArchivo(socket_connection * connection ,char** args)
{
	char* path = args[0];
	size_t sizeText = args[1];
	archivo->path = path;
	archivo->size = sizeText;
	archivo->fd = verificarSiExisteArchivo(path);

if(archivo->fd == existe)
{
archivo->estado = yaCreado;
}
else if (archivo->fd == noExiste)
{
flock(archivo->fd,LOCK_EX);	
archivo->mem_ptro = mmap((void *)NULL,archivo->size,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,archivo->fd,0);
char nVeces = string_repeat("\n",archivo->size);
strcpy(archivo->mem_ptro,nVeces);
archivo->estado = recienCreado;
flock(archivo->fd,LOCK_UN);
}
else
{
archivo->estado = noCreado;
}
sprintf(strEstado, "%i", archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_verificarArchivoCreado",1,strEstado);
}

//off_t lseek(int fildes, off_t offset, int whence);
size_t  obtenerDatos(socket_connection * connection,char ** args){
char* path = args[0];
off_t  * offset = args[1];
size_t sizeText = args[2];
archivo->fd = verificarSiExisteArchivo(path);
if(archivo->fd == noExiste)
{
archivo->estado = noExiste;	
}
else
{
flock(archivo->fd,LOCK_EX);	
lseek(archivo->fd,offset,SEEK_CUR);
flock(archivo->fd,LOCK_UN);
}
return 0;
}

void guardarDatos(socket_connection * connection ,char * path,off_t  * offset,size_t *   size,char * buffer){
}

void borrarArchivo(socket_connection* connection,char * path){
archivo->path = path;
archivo->fd  = verificarSiExisteArchivo(path);
if (archivo->fd  == existe)
{
remove(path);
archivo->estado = recienBorrado;
}
else
{
archivo->estado = noBorrado;
}
sprintf(strEstado, "%i", archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_verificameSiArchivoFueBorrado",1,strEstado);
}

int verificarSiExisteArchivo(char * path){
archivo->path = path;
archivo->fd = open(path,O_RDONLY|O_CREAT);
if(archivo->fd  == -1){
return noExiste;
}
else{
return existe;
}
close(archivo->fd);
}



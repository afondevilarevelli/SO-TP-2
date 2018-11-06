#include  "interfaz.h"
#include "libMDJ.h"
//https://www.programacion.com.py/escritorio/c/archivos-en-c-linux
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
t_metadata_filemetadata * metadata;
char strEstado[2];



t_metadata_filesystem obtenerMetadata{
t_metadata_filesystem * fs;
t_config * metadata = config_create(obtenerPtoMontaje + "/metadata.bin");
fs->tamanio_bloques = config_get_int_value(metadata,"TAMANIO_BLOQUES");
fs->cantidad_bloques = config_get_double_value(metadata,"CANTIDAD_BLOQUES")
fs->magic_number = string_new();
magic_number = config_get_string_value(metadata,"magic_number");
string_append(&fs->magic_number ,config_get_string_value(metadata,"magic_number"));
config_destroy(metadata);
return fs;
}


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
//args[0]: idGDT, args[1]: path
void  validarArchivo(socket_connection * connection,char ** args){
t_archivo *  archivo;	
archivo->path = args[1];
archivo->fd =  verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
archivo->estado =noExiste;
}
else {
archivo->estado=  existe;
}
sprintf(strEstado, "%i", archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_existeArchivo",2, args[0], strEstado);
}

//void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

//args[0]: rutaDelArchivo, args[1]: cantidadDeBytes
void crearArchivo(socket_connection * connection ,char** args)
{
t_archivo *  archivo;	
archivo->path = args[0];
archivo->size = args[1];
archivo->fd = verificarSiExisteArchivo(archivo->path);
t_metadata_filesystem * fs = obtenerMetadata();
archivo->bloques = malloc(fs->cantidad_bloques);
if(archivo->fd == existe)
{
archivo->estado = yaCreado;
}
else if (archivo->fd == noExiste)
{
flock(archivo->fd,LOCK_EX);	
for(int i = 0; i < fs->cantidad_bloques;i++){
char * bloq[i] = crearBloques(i,archivo->path,archivo->size);
archivo->bloques[i] =  bloq[i];
archivo->bloques[i] = mmap((void *)NULL,fs->tamanio_bloques,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,archivo->fd,0);
char nVeces = string_repeat("\n",archivo->size);
strcpy(archivo->bloques[i],nVeces);
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
t_archivo *  archivo;
char * bufferDeBytes = malloc(args[2]);		
archivo->path = args[0];
off_t  * offset = args[1];
archivo->size= args[2];
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste)
{
archivo->estado = noExiste;	
}
else
{
flock(archivo->fd,LOCK_EX);	
long posCorrida = lseek(archivo->fd,offset,SEEK_CUR);
int leidos = read(archivo->fd,bufferDeBytes,posCorrida);
flock(archivo->fd,LOCK_UN);
}
return bufferDeBytes;
}

void guardarDatos(socket_connection * connection ,char * path,off_t  * offset,size_t *  size,char * buffer){
}

void borrarArchivo(socket_connection* connection,char * path){
t_archivo * archivo;
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

FILE * crearBloques(int i,char * path,size_t size){
FILE * archivos[i];
for(int j = 0;j< i; j++){ 
sprintf(path, "%j.bin", j);
archivos[j] = creat(obtenerPtoMontaje +"Bloques/" + path,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
fseek(archivos[j], size , SEEK_SET);
return archivos;
}
}

int verificarSiExisteArchivo(char * path){
t_archivo *  archivo;	
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



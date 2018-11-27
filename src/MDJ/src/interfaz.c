#include  "interfaz.h"
#include "libMDJ.h"
//https://www.programacion.com.py/escritorio/c/archivos-en-c-linux
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
t_metadata_filemetadata * metadata;
char strEstado[2];





char * obtenerPtoMontaje()
{
t_config* fileConfig  = config_create("MDJ.config");
char * ptoMontaje = config_get_string_value(fileConfig,"PTO_MONTAJE");
config_destroy(fileConfig);
return ptoMontaje;
}

//falta probar y hacer algunos free de los malloc.

t_metadata_filesystem *  obtenerMetadata () {
t_metadata_filesystem * fs = malloc(sizeof(t_metadata_filesystem));
char * motanjeMasBin = string_new();
string_append(motanjeMasBin,obtenerPtoMontaje);
string_append(motanjeMasBin,"/metadata.bin");
t_config * metadata = config_create(motanjeMasBin);
fs->tamanio_bloques = config_get_int_value(metadata,"TAMANIO_BLOQUES");
fs->cantidad_bloques = config_get_double_value(metadata,"CANTIDAD_BLOQUES");
fs->magic_number = string_new();
char * magic_number = config_get_string_value(metadata,"MAGIC_NUMBER");
string_append(&fs->magic_number ,magic_number);
config_destroy(metadata);
return fs;
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
t_archivo *  archivo = malloc(sizeof(t_archivo));	
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
t_archivo *  archivo= malloc(sizeof(t_archivo));	
archivo->path = args[0];
size_t tsize = atoi(args[1]);
archivo->size =  tsize;
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
//int * fdBloques = crearBloques(i,archivo->path,archivo->size);
//archivo->bloques[i] = mmap((void *)NULL,fs->tamanio_bloques,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,fdBloques[i],0);
char nVeces = string_repeat("\n",archivo->size);
strcpy(archivo->bloques[i],nVeces);
archivo->estado = recienCreado;
flock(archivo->fd,LOCK_UN);
}
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
t_archivo *  archivo= malloc(sizeof(t_archivo));
size_t leidos;
archivo->path = args[0];
off_t  offset = atoi(args[1]);
size_t tsize =  atoi(args[2]);
archivo->size =  tsize;	
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste)
{
archivo->estado = noExiste;	
}
else
{
flock(archivo->fd,LOCK_EX);	
off_t posCorrida = lseek(archivo->fd,offset,SEEK_CUR);
long  tamanioRestante = tsize - posCorrida;
char * bufferDeBytes = malloc(tamanioRestante);	
leidos = read(archivo->fd,bufferDeBytes,posCorrida);
flock(archivo->fd,LOCK_UN);
}
return leidos ;
}

void guardarDatos(socket_connection * connection ,char * path,off_t  * offset,size_t *  size,char * buffer){
}

void borrarArchivo(socket_connection* connection,char ** args){
t_archivo * archivo= malloc(sizeof(t_archivo));
archivo->path = args[0];
archivo->fd  = verificarSiExisteArchivo(archivo->path);
if (archivo->fd  == existe)
{
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

int * crearBloques(socket_connection* connection,char ** args){
 int i = atoi(args[0]);
 char * path = args[1];
 int size = atoi(args[2]);   
int *  fdBloques[i];
char * nVeces = string_new();
char * destino = string_new();
nVeces = string_repeat('/n',size);
for(int j = 0;j< i; j++){ 
sprintf(destino,"%s/Bloque/%d.bin",path,j);
fdBloques[j] = open(destino,O_RDWR | O_CREAT);
//lseek( fdBloques[j], size , SEEK_SET);
}
return fdBloques;
}

int verificarSiExisteArchivo(char * path){
t_archivo *  archivo= malloc(sizeof(t_archivo));	
archivo->path = path;
archivo->fd = open(path,O_RDONLY|O_WRONLY);
if(archivo->fd  == -1){
return noExiste;
}
else{
return existe;
}
close(archivo->fd);
}

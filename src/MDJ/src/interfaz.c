#include  "interfaz.h"
#include "libMDJ.h"
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
t_metadata_filemetadata * metadata;
char strEstado[2];


char * obtenerPtoMontaje()
{
t_config* fileConfig  = config_create("MDJ.config");
char * ptoMontaje = string_new();
string_append(&ptoMontaje,config_get_string_value(fileConfig,"PTO_MONTAJE"));
config_destroy(fileConfig);
if( strcmp(ptoMontaje,"") > 0){
log_trace(logger,"Se obtuvo correctamente el pto de montaje");
}
else
{
log_error(logger,"No se puedo obtener el pto de montaje");
}
return ptoMontaje;
}

//falta probar y hacer algunos free de los malloc.

t_metadata_filesystem *  obtenerMetadata () {
t_metadata_filesystem * fs = malloc(sizeof(t_metadata_filesystem));
char * motanjeMasBin = string_new();
//string_append(motanjeMasBin,obtenerPtoMontaje ());
//string_append(motanjeMasBin,"/home/utnso/Escritorio/fifa-checkpoint/Metadata");
t_config * metadata = config_create("/home/utnso/Escritorio/fifa-checkpoint/Metadata/Metadata.bin");
fs->tamanio_bloques = config_get_int_value(metadata,"TAMANIO_BLOQUES");
fs->cantidad_bloques = config_get_double_value(metadata,"CANTIDAD_BLOQUES");
char * magic_number = string_new();
string_append(&magic_number,config_get_string_value(metadata,"MAGIC_NUMBER"));
fs->magic_number = magic_number;
if( strcmp(magic_number,"") > 0 && fs->tamanio_bloques != 0 && fs->cantidad_bloques != 0){
log_trace(logger,"Se obtuvo correctamente la metadata");
}
else
{
log_error(logger,"No se puedo obtener la metadata");
}
//config_destroy(metadata);
//free(magic_number);
return fs;
}


void aplicarRetardo()
{
t_config* fileConfig  = config_create("MDJ.config");
int ret = config_get_int_value(fileConfig,"RETARDO");
usleep(ret);
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
char *  nVeces = string_repeat('\n',archivo->size);
if(archivo->fd == existe)
{
archivo->estado = yaCreado;
}
else if (archivo->fd == noExiste)
{
flock(archivo->fd,LOCK_EX);	
for(int i = 0; i < fs->cantidad_bloques;i++){
int * fdBloques = crearBloques(i,archivo->path,archivo->size);
archivo->bloques[i] = mmap(0,fs->tamanio_bloques,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,fdBloques[i],0);
//strcpy(archivo->bloques[i],nVeces);
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
runFunction(connection->socket,"MDJ_DAM_verificarArchivoCreado",2,strEstado,archivo->path);
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

int * crearBloques(int i,char * path,size_t size){ 
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

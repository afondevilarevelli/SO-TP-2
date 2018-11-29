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
if( strcmp(ptoMontaje,"") == 0){
log_error(logger,"No se puedo obtener el pto de montaje");
}
return ptoMontaje;
}

//falta probar y hacer algunos free de los malloc.

t_metadata_filesystem *  obtenerMetadata() {
t_metadata_filesystem * fs = malloc(sizeof(t_metadata_filesystem));
char * motanjeMasBin = string_new();
string_append(&motanjeMasBin,obtenerPtoMontaje());
string_append(&motanjeMasBin,"/Metadata/Metadata.bin");
t_config * metadata = config_create(motanjeMasBin);
fs->tamanio_bloques = config_get_int_value(metadata,"TAMANIO_BLOQUES");
fs->cantidad_bloques = config_get_int_value(metadata,"CANTIDAD_BLOQUES");
char * magic_number = string_new();
string_append(&magic_number,config_get_string_value(metadata,"MAGIC_NUMBER"));
fs->magic_number = magic_number;
if( strcmp(magic_number,"") > 0 && fs->tamanio_bloques != 0 && fs->cantidad_bloques != 0){
log_trace(logger,"Se obtuvo correctamente la metadata y el pto montaje");
}
else
{
log_error(logger,"No se puedo obtener la metadata ni el pto montaje");
}
config_destroy(metadata);
free(magic_number);
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
archivo->fd =  1;//verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
archivo->estado =noExiste;
}
else {
archivo->estado=  existe;
}
sprintf(strEstado, "%i", archivo->estado);
aplicarRetardo();
free(archivo);
runFunction(connection->socket,"MDJ_DAM_existeArchivo",3, args[0], strEstado, args[1]);
}

//void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

//args[0]: rutaDelArchivo, args[1]: cantidadDeBytes
void crearArchivo(socket_connection * connection ,char** args)
{
t_archivo *  archivo= malloc(sizeof(t_archivo));	
archivo->path = args[0];
size_t tsize = atoi(args[1]);
archivo->size =  (tsize - 1);
t_metadata_filesystem * fs = obtenerMetadata();
archivo->bloques = malloc(fs->cantidad_bloques);
int * fdBloques = malloc(fs->cantidad_bloques);
size_t tamanioBloques = (fs->tamanio_bloques - 1);
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == -1)
{
char * pathMasArchivos = string_new();
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos/scripts/");
string_append(&pathMasArchivos,archivo->path);
archivo-> fd = open(pathMasArchivos,O_RDWR|O_CREAT);
flock(archivo->fd,LOCK_SH);	
char *  nVeces = string_repeat('\n',archivo->size);
lseek(archivo->fd,archivo->size, SEEK_SET);
write(archivo->fd, "",1);
char * file = mmap(0, archivo->size, PROT_READ | PROT_WRITE, MAP_SHARED, archivo->fd, 0);
memcpy(file,nVeces,strlen(nVeces));
msync(file,archivo->size, MS_SYNC);
munmap(file,archivo->size);
close(archivo->fd);
log_trace(logger,"Archivo %s creado correctamente en %s/Archivos/scripts",archivo->path,obtenerPtoMontaje());
flock(archivo->fd,LOCK_UN);
char * destino = string_new();
for(int j = 0;j< fs->cantidad_bloques; j++){ 
sprintf(destino,"%s/Bloque/%d.bin",archivo->path,j);
fdBloques[j] = open(destino,O_RDWR | O_CREAT);
flock(fdBloques[j],LOCK_EX);
lseek(fdBloques[j],tamanioBloques, SEEK_SET);
write(fdBloques[j],"",1);
archivo->bloques[j] = mmap(0,fs->tamanio_bloques,PROT_EXEC|PROT_READ|PROT_WRITE,MAP_SHARED,fdBloques[j],0);
memcpy(file,nVeces,strlen(nVeces));
msync(file,archivo->size, MS_SYNC);
munmap(file,archivo->size);
flock(archivo->fd,LOCK_UN);
close(archivo->fd);
}
/*archivo->estado = recienCreado;
}
}
else
{
archivo->estado = noCreado;*/
//}
//sprintf(strEstado, "%i", archivo->estado);*/
aplicarRetardo();
//runFunction(connection->socket,"MDJ_DAM_verificarArchivoCreado",2,strEstado,archivo->path);
}

//off_t lseek(int fildes, off_t offset, int whence);
size_t  obtenerDatos(socket_connection * connection,char ** args){
t_archivo *  archivo= malloc(sizeof(t_archivo));
size_t leidos;
archivo->path = args[0];
off_t  offset = atoi(args[1]);
size_t tsize =  atoi(args[2]);
archivo->size =  tsize;	
archivo->fd = 1;//verificarSiExisteArchivo(archivo->path);
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
archivo->fd  = 1; //verificarSiExisteArchivo(archivo->path);
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
char * destino = string_new();
for(int j = 0;j< i; j++){ 
sprintf(destino,"%s/Bloque/%d.bin",path,j);
fdBloques[j] = open(destino,O_RDWR | O_CREAT);
//lseek( fdBloques[j], size , SEEK_SET);
}
return fdBloques;
}

int verificarSiExisteArchivo(char * path){	
char * pathMasArchivos = string_new();
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos/scripts/");
string_append(&pathMasArchivos,path);
int fd = open(pathMasArchivos,O_RDONLY);
if(fd < 0)
return -1;
else
{
return fd;
}
close(fd);
}

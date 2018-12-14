#include  "interfaz.h"
#include "libMDJ.h"
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
//t_metadata_filemetadata * metadata;


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
sleep(ret / 1000);
config_destroy(fileConfig);
}
//args[0]:idGDT ,args[1]: path, args[2]: socketCPU, args[3]: 1(Dummy) ó 0(no Dummy)
void  validarArchivo(socket_connection * connection,char ** args){
char strEstado[2];
t_archivo *  archivo = malloc(sizeof(t_archivo));	
archivo->path = args[0];
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
archivo->estado = noExiste;
}
else {
archivo->estado=  existe;
}
sprintf(strEstado,"%i", archivo->estado);
aplicarRetardo();
free(archivo);
runFunction(connection->socket,"MDJ_DAM_existeArchivo",5, strEstado, args[2], args[1], args[0], args[3]);
}



//args[0]: idGDT, args[1]: rutaDelArchivo, args[2]: cantidadDeBytes
void crearArchivo(socket_connection * connection ,char** args)
{
t_archivo *  archivo= malloc(sizeof(t_archivo));
t_metadata_bitmap * bitMap = malloc(sizeof(t_metadata_bitmap));	
t_metadata_filesystem * fs = obtenerMetadata();
archivo->path = args[1];
size_t tsize = atoi(args[2]);
t_list * bloquesLibres = list_create();
t_list * bloquesOcupados = list_create();
archivo->size = (size_t) (tsize - 1);
size_t tamanioBloques = (fs->tamanio_bloques - 1);
char * pathMasArchivos = string_new();
char * tam = string_new(); 
char * tamBloq = string_new();
char * archivoBloques = string_new();
char * temp = string_new();
bitMap->bitarray = crearBitmap(fs->cantidad_bloques);
for(int i = 0; i < fs->cantidad_bloques; i++)
{
if(bitarray_test_bit(bitMap->bitarray,i) == 0)
{
list_add(bloquesLibres,(void * ) i);    
}
else
{
 list_add(bloquesOcupados,(void * ) i);  
}
}
t_bloques * bitmapBloques = asignarBloques(bloquesLibres,bloquesOcupados,archivo->size);
int s;
for(s=0; s < bitmapBloques->bloques;s++){
	string_append_with_format(&temp,"%s,",string_itoa(bitmapBloques->bloqArchivo[s]));
}
char * archTemp = string_new();
archTemp = string_substring(temp,0,strlen(temp) - 1);
char * aux = string_new();
char * dir = string_new();
string_append(&aux,archivo->path);
string_append(&dir,obtenerPtoMontaje());
string_append(&dir,"/Archivos/");
string_append(&dir,strtok(aux,"/"));
DIR * directory = opendir(dir);
if (directory == NULL)
{
mkdir(dir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
closedir(directory);
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == -1)
{
sprintf(tam,"%i",tsize);
string_append(&tamBloq,"TAMANIO=");
string_append(&tamBloq,tam);
string_append(&tamBloq,"\n");
string_append(&archivoBloques,"BLOQUES=");
string_append(&archivoBloques,"[");
string_append(&archivoBloques,archTemp);
string_append(&archivoBloques,"]");
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos/");
string_append(&pathMasArchivos,archivo->path);
archivo-> fd = open(pathMasArchivos,O_RDWR|O_CREAT);
flock(archivo->fd,LOCK_SH);	
char *  nVeces = string_repeat('\n',archivo->size);
lseek(archivo->fd,archivo->size, SEEK_SET);
write(archivo->fd, "",1);
char * file = mmap(0, archivo->size, PROT_READ | PROT_WRITE, MAP_SHARED, archivo->fd, 0);
memcpy(file,nVeces,strlen(nVeces));
memcpy(file,tamBloq,strlen(tamBloq));
memcpy(file,strcat(tamBloq,archivoBloques),strlen(archivoBloques)+strlen(tamBloq));
msync(file,archivo->size, MS_SYNC);
munmap(file,archivo->size);
flock(archivo->fd,LOCK_UN);
close(archivo->fd);
log_trace(logger,"Archivo %s creado correctamente en %s/Archivos",archivo->path,obtenerPtoMontaje());
archivo->estado = recienCreado;
}
else
{
archivo->estado = yaCreado;
log_info(logger,"Archivo %s ya creado",archivo->path);
}
char * strEstado = string_itoa(archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_verificarArchivoCreado",3,args[0],strEstado,archivo->path);
list_destroy(bloquesLibres);
list_destroy(bloquesOcupados);
free(archivo);
free(bitMap);
free(fs);
free(bitmapBloques);
free(archTemp);
free(tam);
free(temp);
free(pathMasArchivos);
free(dir);
free(aux);
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
return leidos;
}

void guardarDatos(socket_connection* connection,char ** args){
t_archivo * archivo = malloc(sizeof(t_archivo));
t_metadata_filesystem * fs = obtenerMetadata();
archivo->path = args[0];
off_t offset = atoi(args[1]);
size_t size = atoi(args[2]);
char * buffer = args[3];
char ** bloques = obtenerBloques(archivo->path);
if(string_is_empty(bloques) == 1)
{
 log_info(logger,"El archivo %s no existe",archivo->path);   
}
else
{
char temp[200];
int * fd = malloc(sizeof(cantElementos2(bloques)));
for(int i=0; i < cantElementos2(bloques);i++){
snprintf(temp, sizeof(temp), "%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",atoi(bloques[i]));
fd[i] = open(temp,O_RDWR);
ftruncate(fd[i],fs->tamanio_bloques);
flcose(fd[i]);
}
}
free(archivo);
free(fs);
}


//args[0]: idGDT, args[1]: arch
void borrarArchivo(socket_connection* connection,char ** args){
t_archivo * archivo= malloc(sizeof(t_archivo));
archivo->path = args[1];
t_metadata_filesystem * fs = obtenerMetadata();
t_bitarray * bitarray = crearBitmap(fs->cantidad_bloques);
archivo->fd = verificarSiExisteArchivo(archivo->path);
char * temp = string_new();
string_append(&temp,obtenerPtoMontaje());
string_append(&temp,"/Archivos/");
string_append(&temp,archivo->path);
if (archivo->fd == noExiste)
{
log_info(logger,"El achivo %s no existe",archivo->path);
archivo->estado = noBorrado;
}
else
{
char ** bloques = obtenerBloques(archivo->path);
for(int i=0; i < cantElementos2(bloques);i++)
{
bitarray_clean_bit(bitarray,atoi(bloques[i]));
}
int r = unlink(temp);
if(r < 0)
{
log_error(logger,"Hubo un error al querer borrar %s",archivo->path);
archivo->estado = errorBorrado;
}
else
{
log_trace(logger,"Archivo %s borrado correctamente",archivo->path);    
}
archivo->estado = recienBorrado;
}
char * strEstado = string_itoa(archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_verificameSiArchivoFueBorrado",3,args[0], strEstado, archivo->path);
free(fs);
free(bitarray);
free(temp);
free(archivo);
}


int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

t_bitarray * crearBitmap(int  size){
char * montajeMasBitmap = string_new();
string_append(&montajeMasBitmap,obtenerPtoMontaje());
string_append(&montajeMasBitmap,"/Metadata/Bitmap.bin");
int bitmap = open(montajeMasBitmap,O_RDWR);
struct stat mystat;
if (fstat(bitmap, &mystat) < 0) {
	printf("Error al establecer fstat\n");
	close(bitmap);
	}
char * bmap = mmap(0,mystat.st_size,PROT_EXEC | PROT_WRITE | PROT_READ, MAP_SHARED, bitmap, 0);
if (bmap == MAP_FAILED) {
			printf("Error al mapear a memoria: %s\n", strerror(errno));

	}
t_bitarray * bitarray = bitarray_create_with_mode(bmap,size/8,MSB_FIRST);
free(montajeMasBitmap);
return bitarray;
}

int cantElementos2(char ** array)
{
int cont = 0;
while (array[cont] != NULL){
cont = cont + 1;	
}	
return cont;
}

int cantElementos1(char * array)
{
int cont = 0;
while (array[cont] != NULL){
cont = cont + 1;	
}	
return cont;
}

char ** obtenerBloques(char * path){
int estado = verificarSiExisteArchivo(path);
if(estado != -1)
{
char * temp = string_new();
string_append(&temp,obtenerPtoMontaje());
string_append(&temp,"/Archivos/");
string_append(&temp,path);
t_config * config = config_create(temp);
char **  bloques = config_get_array_value(config,"BLOQUES");
free(temp);
return bloques;
}
else{
 return  "";  
}
}

t_bloques *  asignarBloques(t_list * libres,t_list * ocupados ,size_t size)
{
t_metadata_filesystem * fs = obtenerMetadata();
t_bloques * bloques = malloc(sizeof(t_bloques));
t_list * temp = list_create();
t_bitarray * bitarray = crearBitmap(fs->cantidad_bloques);
int nBloques = 0;
if(size % fs->tamanio_bloques == 0)
{
nBloques = size / fs->tamanio_bloques;
}
else
{
nBloques = (size / fs->tamanio_bloques) + 1;    
}
if (list_size(libres) >= nBloques){
temp  = list_take_and_remove(libres,nBloques);
list_add_all(ocupados,temp);
}
else
{
log_error(logger,"No hay bloques disponibles , vuelva a intentarlo");
exit;
}
bloques->bloqLibres = list_duplicate(libres);
bloques->bloqOcupados = list_duplicate(ocupados);
bloques->bloqArchivo = calloc(nBloques,sizeof(int));
for (int i=0;i <list_size(temp);i++){
 bloques->bloqArchivo[i]=  list_get(temp,i);
}
for(int i = 0;i < nBloques;i++){
bitarray_set_bit(bitarray,bloques->bloqArchivo[i]);	
}
bloques->bloques = nBloques;
return bloques;
}

void liberarLista(t_list *lista) {
	list_destroy_and_destroy_elements(lista,(void *) free);
}


// retorna un -1 si el archivo no existe
// reortna un numero >0 si el archivo existe
int verificarSiExisteArchivo(char * path)
{	
char * pathMasArchivos = string_new();
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos/");
string_append(&pathMasArchivos,path);
int fd = open(pathMasArchivos,O_RDONLY);
if(fd < 0){
    close(fd);
    return -1;
}
else
{
    close(fd);
    return fd;
}
free(pathMasArchivos);
}

























































































































































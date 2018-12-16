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

//args[0]:idGDT ,args[1]: path, args[2]: socketCPU
void  validarArchivo(socket_connection * connection,char ** args){
char strEstado[2];
t_archivo *  archivo = malloc(sizeof(t_archivo));	
archivo->path = args[1];
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
runFunction(connection->socket,"MDJ_DAM_existeArchivo",4, strEstado, args[2], args[1], args[0]);
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
char * aux = string_new();
char * dir = string_new();
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
if(string_contains(archivo->path,"/"))
{
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
}    
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
msync(file,archivo->size,MS_SYNC);
munmap(file,archivo->size);
flock(archivo->fd,LOCK_UN);
close(archivo->fd);
char ** bloques = obtenerBloques(archivo->path);
char temp2[200];
int * fd = malloc(sizeof(cantElementos2(bloques)));
for( int i = 0;i < cantElementos2(bloques);i++){
snprintf(temp2,sizeof(temp2),"%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",atoi(bloques[i]));
fd[i] = open(temp2,O_RDWR);
flock(fd[i],LOCK_SH);	
ftruncate(fd[i],fs->tamanio_bloques);
flock(fd[i],LOCK_UN);
close(fd[i]);
}
log_trace(logger,"Archivo %s creado correctamente en %s/Archivos",archivo->path,obtenerPtoMontaje());
archivo->estado = recienCreado;
free(fd);
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
free(dir);
free(aux);
free(archTemp);
free(tam);
free(pathMasArchivos);
}

//args[0]: idGDT, args[1]: path, args[2]: offset, args[3]: size, args[4]: socketCPU
void obtenerDatos(socket_connection * connection,char ** args){
t_archivo *  archivo= malloc(sizeof(t_archivo));
t_metadata_filesystem * fs = obtenerMetadata();
archivo->path = args[1];
off_t  offset = atoi(args[2]);
size_t tsize =  atoi(args[3]);
archivo->size =  tsize;	
archivo->fd = verificarSiExisteArchivo(archivo->path);
int bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
int bloqueFinal = (offset + tsize) / fs->tamanio_bloques;
char ** bloques = obtenerBloques(archivo->path);
char * buffer = string_new();
if(archivo->fd == noExiste)
{
log_error(logger,"El archivo %s es inexistente",archivo->path);    
archivo->estado = noExiste;	
}
else
{
if(bloqueInicial < 0)
{
log_error("El offset no puede ser mayor al tamanio de %s",archivo->path);  
}
else{
char * temp [200];
while (bloqueInicial <= bloqueFinal && bloques[bloqueInicial] != NULL)
{
int posBloque = atoi(bloques[bloqueInicial]);
char * bufferBloques = obtenerDatosBloque(posBloque);
int tamanioBufferBloques = string_length(bufferBloques);
int longitud; 
if (tsize - tamanioBufferBloques < 0) 
{
longitud = tsize;
}
else
{
longitud = tamanioBufferBloques;
}
char * res  = string_substring_until(bufferBloques,longitud);
string_append(&buffer,res);
free(bufferBloques);
free(res);
free(bloques[bloqueInicial]);
bloqueInicial++;
tsize = tsize - longitud;
}
}
log_trace(logger,"Se obtuvieron %d bytes",string_length(buffer));
}
aplicarRetardo();
char * bytes = string_itoa(string_length(buffer));
char * strEstado = string_itoa(archivo->estado); // cuando se setea este valor si sale todo bien?
runFunction(connection->socket,"MDJ_DAM_respuestaDatos",5,args[0],bytes,strEstado,archivo->path, args[4]);
//El DAM necesita que la variable bytes sea todo el string que pasa, no un numero indicando cuando se leyeron
//( digo por que no sé qué estas pasando acá)
}

void guardarDatos(socket_connection* connection,char ** args){
t_archivo * archivo = malloc(sizeof(t_archivo));
t_metadata_filesystem * fs = obtenerMetadata();
archivo->path = args[1];
off_t offset = atoi(args[2]);
size_t size = atoi(args[3]);
char * buffer = args[4];
char ** bloques = obtenerBloques(archivo->path);
int bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
int bloqueFinal = (offset + size) / fs->tamanio_bloques;
int offRestante = offset - (offset /fs->tamanio_bloques) * fs->tamanio_bloques;
int longitud;  
if(string_is_empty(bloques) == 1)
{
 log_info(logger,"El archivo %s no existe",archivo->path);   
}
else if (obtenerBloqueInicial(archivo->path,offset) == -1)
{
log_error(logger,"El offset no puede ser mayor al tamanio del archivo"); 
}
else
{
if (size > fs->tamanio_bloques)
{
longitud = fs->tamanio_bloques;
 }
 else
 { 
 longitud = size;
 }
longitud =longitud - offRestante;
escribirBloque(atoi(bloques[bloqueInicial]), offRestante,longitud, string_substring_until(buffer,longitud));
free(bloques[bloqueInicial]);
bloqueInicial++;
size = size - longitud;
int escritos = longitud;
while (bloqueInicial <= bloqueFinal && bloques[bloqueInicial] != NULL) { 
	int tamBloq;
	if (size > fs->tamanio_bloques)
	{
		tamBloq =  fs->tamanio_bloques;
	} 
	else
	{
    tamBloq = size;
	}
    escribirBloque(atoi(bloques[bloqueInicial]), 0,tamBloq, string_substring(buffer,escritos,tamBloq));
    bloqueInicial++;
	size = size - tamBloq;
	escritos =  escritos + tamBloq;
	free(bloques[bloqueInicial]);
	}
free(bloques);
free(archivo);
free(fs);
}
}



void escribirBloque(int bloque, int offset, int length, char * buffer) {
	char temp[200];
	snprintf(temp, sizeof(temp), "%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",bloque);
	FILE* archivoBloque = fopen(temp,"r+");
	fseek(archivoBloque, offset, SEEK_SET);
	fwrite(buffer, 1, length, archivoBloque);
	fclose(archivoBloque);
}


char * obtenerDatosBloque (int bloque) {
    char * temp [200];
    snprintf(temp,sizeof(temp),"%s%s%i.bin",obtenerPtoMontaje(),"/Bloques/",bloque);
	FILE * bloqueFile = fopen(temp,"r");
	int size;
	char* buffer;
	fseek(bloqueFile, 0L, SEEK_END);
	size = ftell(bloqueFile);
	fseek(bloqueFile, 0L, SEEK_SET);
    buffer = malloc(size);
	fread(buffer, size, 1, bloqueFile);
	buffer = string_substring_until(buffer, size);
	fclose(bloqueFile);
	return buffer;
}



int obtenerBloqueInicial(char * path,off_t offset)
{
t_archivo * archivo = malloc(sizeof(t_archivo));
t_metadata_filesystem * fs = obtenerMetadata();
char * pathMasArchivos = string_new();
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos/");
string_append(&pathMasArchivos,path);
t_config * config = config_create(pathMasArchivos);
int tamanio = config_get_int_value(config,"TAMANIO");
if(offset > tamanio)
{
   return -1;
}
else if (offset == 0 || offset < fs->tamanio_bloques)
{
    return 0;
}
else
{
if(offset % fs->tamanio_bloques == 0)
{
  return offset / fs->tamanio_bloques;  
}
else
{
    return (offset / fs->tamanio_bloques) + 1;
}
}
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
char aux[200];
int * fd = malloc(sizeof(cantElementos2(bloques)));
for(int i=0; i < cantElementos2(bloques);i++)
{
bitarray_clean_bit(bitarray,atoi(bloques[i]));
snprintf(aux, sizeof(aux), "%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",atoi(bloques[i]));
fd[i] = open(aux, O_TRUNC| O_RDWR);
ftruncate(fd[i],0);
close(fd[i]);
}
free(fd);
int r = unlink(temp);
if(r < 0)
{
log_error(logger,"Hubo un error al querer borrar %s",archivo->path);
archivo->estado = errorBorrado;
}
else
{
log_trace(logger,"Archivo %s borrado correctamente",archivo->path);
archivo->estado = recienBorrado;    
}
if(string_contains(archivo->path,"/"))
{
char * auxx = string_new();
string_append(&auxx,archivo->path);
char * dir = string_new();
string_append(&dir,obtenerPtoMontaje());
string_append(&dir,"/Archivos/");
string_append(&dir,strtok(auxx,"/"));
DIR * directory = opendir(dir);
if (directory != NULL)
{
closedir(directory);    
rmdir(dir);
}
free(auxx);
free(dir);
}
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
msync(bmap,mystat.st_size,MS_SYNC);
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

























































































































































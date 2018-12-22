#include  "interfaz.h"
#include "libMDJ.h"
// Con esto se maneja cada mensaje que se le mando al dam, si es 0 es porque es false, si es 1 es porque es true
//t_metadata_filemetadata * metadata;

char * obtenerPtoMontaje()
{/*
t_config* fileConfig  = config_create("MDJ.config");
char * ptoMontaje = string_new();
string_append(&ptoMontaje,config_get_string_value(fileConfig,"PTO_MONTAJE"));
config_destroy(fileConfig);
if( strcmp(ptoMontaje,"") == 0){
log_error(logger,"No se puedo obtener el pto de montaje");
}*/
return datosConfMDJ->ptoMontaje;
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
log_info(logger,"Se va a verificar que el archivo %s exista", args[1]);
char strEstado[2];
t_archivo *  archivo = malloc(sizeof(t_archivo));	
archivo->path = args[1];
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == noExiste){
archivo->estado = noExiste;
log_trace(logger,"El archivo %s NO existe", args[1]);
}
else {
archivo->estado=  existe;
log_trace(logger,"El archivo %s SI existe", args[1]);
}
sprintf(strEstado,"%i", archivo->estado);
aplicarRetardo();
free(archivo);
runFunction(connection->socket,"MDJ_DAM_existeArchivo",4, strEstado, args[2], args[1], args[0]);
}



//args[0]: idGDT, args[1]: rutaDelArchivo, args[2]: cant de bytes, args[3]: socketCPU
void crearArchivo(socket_connection * connection ,char** args)
{
log_info(logger,"Se va a crear el archivo %s con %s lineas", args[1], args[2]);
t_archivo *  archivo= malloc(sizeof(t_archivo));
t_metadata_bitmap * bitMap = malloc(sizeof(t_metadata_bitmap));	
archivo->path = args[1];
size_t tsize = atoi(args[2]);
t_list * bloquesLibres = list_create();
t_list * bloquesOcupados = list_create();
archivo->size = (size_t) tsize;
size_t tamanioBloques = TAM_BLOQUE;
char * pathMasArchivos = string_new();
char * tam = string_new(); 
char * tamBloq = string_new();
char * archivoBloques = string_new();
char * temp = string_new();
char * aux = string_new();
char * dir = string_new();
bitMap->bitarray = crearBitmap(CANT_BLOQUES);
void cicloSeteoBloques(){ 
	for(int i = 0; i < CANT_BLOQUES; i++)
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
}
cicloSeteoBloques();
t_bloques * bitmapBloques = asignarBloques(bloquesLibres,bloquesOcupados,archivo->size);

if(bitmapBloques == NULL){
	list_destroy(bloquesLibres);
	list_destroy(bloquesOcupados);
	runFunction(connection->socket,"MDJ_DAM_resultadoCreacionArchivo",4,args[0],"-1",archivo->path, args[3]);
	free(archivo);
	free(bitMap);
	free(dir);
	free(aux);
	free(tam);
	free(pathMasArchivos);
	return;
}
int s;
for(s=0; s < bitmapBloques->bloques;s++){
	string_append_with_format(&temp,"%s,",bitmapBloques->bloqArchivo[s]);
	free(bitmapBloques->bloqArchivo[s]);
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
mkdir(dir,S_IRWXU | S_IRWXG | S_IRWXO );
}
closedir(directory);  
}    
archivo->fd = verificarSiExisteArchivo(archivo->path);
if(archivo->fd == -1)
{
pthread_mutex_lock(&mdjInterfaz);	
sprintf(tam,"%i",tsize);
string_append(&tamBloq,"TAMANIO=");
string_append(&tamBloq,tam);
string_append(&tamBloq,"\n");
string_append(&archivoBloques,"BLOQUES=");
string_append(&archivoBloques,"[");
string_append(&archivoBloques,archTemp);
string_append(&archivoBloques,"]");
string_append(&pathMasArchivos,obtenerPtoMontaje());
string_append(&pathMasArchivos,"/Archivos");
string_append(&pathMasArchivos,archivo->path);
archivo-> fd = open(pathMasArchivos,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);

char *  nVeces = string_repeat('\n',archivo->size);
string_append(&tamBloq, archivoBloques);
lseek(archivo->fd,0, SEEK_SET);
write(archivo->fd, tamBloq,strlen(tamBloq));
//char * file = mmap(0, archivo->size, PROT_READ | PROT_WRITE, MAP_SHARED, archivo->fd, 0);
//memcpy(file,nVeces,strlen(nVeces));
//memcpy(file,tamBloq,tamTodo);
//msync(file,archivo->size,MS_SYNC);
//munmap(file,tamTodo);//archivo->size);
close(archivo->fd);
char ** bloques = obtenerBloques(archivo->path);
char temp2[200];
int * fd = malloc(sizeof(cantElementos2(bloques)));
int restante = archivo->size;
for( int i = 0;i < cantElementos2(bloques);i++){
snprintf(temp2,sizeof(temp2),"%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",atoi(bloques[i]));
fd[i] = open(temp2,O_RDWR);
ftruncate(fd[i],TAM_BLOQUE);
write(archivo->fd, nVeces,restante);
restante -= TAM_BLOQUE;
close(fd[i]);
pthread_mutex_unlock(&mdjInterfaz);
}
log_trace(logger,"Archivo %s creado correctamente en %s/Archivos",archivo->path,obtenerPtoMontaje());
archivo->estado = recienCreado;
free(fd);
}
else
{
archivo->estado = yaCreado;
log_trace(logger,"El archivo %s previamente ya existia",archivo->path);
}
char * strEstado = string_itoa(archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_resultadoCreacionArchivo",4,args[0],strEstado,archivo->path, args[3]);
list_destroy(bloquesLibres);
list_destroy(bloquesOcupados);
free(archivo);
free(bitMap);
free(bitmapBloques->bloqArchivo);
free(bitmapBloques);
free(dir);
free(aux);
free(archTemp);
free(tam);
free(pathMasArchivos);
}

//args[0]: idGDT, args[1]: path, args[2]: offset, args[3]: size, args[4]: dummy, args[5]:primera o no,args[6]:socketCPU
void obtenerDatos(socket_connection * connection,char ** args){
log_info(logger,"Se quieren obtener datos del archivo %s",args[1]);
t_archivo *  archivo= malloc(sizeof(t_archivo));
char ultimaLectura[1] = "0";
archivo->path = args[1];
off_t  offset = atoi(args[2]);
int offsetDeBloque;
if(offset < TAM_BLOQUE)
	offsetDeBloque = offset;
else
	offsetDeBloque = offset % TAM_BLOQUE;
size_t tsize =  atoi(args[3]);
size_t sizePosta =  atoi(args[3]);
int tamanio = atoi(args[3]);
archivo->size =  tsize;	
archivo->fd = verificarSiExisteArchivo(archivo->path);
int bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
int bloqueFinal;
if( (offset + tsize - 1) > 0)
	bloqueFinal = (offset + tsize - 1) / TAM_BLOQUE;
else
	bloqueFinal = 0;
char ** bloques = obtenerBloques(archivo->path);
char * buffer = string_new();
char * bufferBloques;
if(archivo->fd == noExiste)
{
log_error(logger,"El archivo %s es inexistente",archivo->path);    
archivo->estado = noExiste;	
}
else
{
if(bloqueInicial < 0)
{
log_error(logger,"El offset no puede ser mayor al tamanio del archivo %s",archivo->path);  
}
else{
pthread_mutex_lock(&mdjInterfaz);	
char * temp [200];
while (bloqueInicial <= bloqueFinal && bloques[bloqueInicial] != NULL)
{
int posBloque = atoi(bloques[bloqueInicial]);
bufferBloques = obtenerDatosBloque(posBloque);
int tamanioBufferBloques = string_length(bufferBloques);
int longitud; 
if (offsetDeBloque + tsize <= tamanioBufferBloques ) 
{
longitud = tsize;
//char * res  = string_substring(bufferBloques,offsetDeBloque, longitud);
}
else
{
longitud = tamanioBufferBloques - offsetDeBloque;
if(longitud < 0)
	longitud *= -1;
//char * res  = string_substring(bufferBloques,offsetDeBloque, longitud);
//offsetDeBloque = 0;
}
char * res  = string_substring(bufferBloques,offsetDeBloque, longitud);
string_append(&buffer,res);
free(res);
free(bloques[bloqueInicial]);
free(bufferBloques);
bloqueInicial++;
tsize = tsize - longitud;
}
archivo->estado = existe;
pthread_mutex_unlock(&mdjInterfaz);
}
char * temp = string_new();
string_append(&temp,obtenerPtoMontaje());
string_append(&temp,"/Archivos");
string_append(&temp,archivo->path);
t_config * config = config_create(temp);
int tamBytesArchivo = config_get_int_value(config,"TAMANIO");
free(temp);
if(offset + sizePosta >= tamBytesArchivo)
	ultimaLectura[0] = '1';
log_trace(logger,"Se obtuvieron %d bytes: %s ",string_length(buffer),buffer);
config_destroy(config);
}
aplicarRetardo();
char* strEstado = string_itoa(archivo->estado);
runFunction(connection->socket,"MDJ_DAM_respuestaDatos",8,args[0],buffer,strEstado,archivo->path, args[4],args[5], args[6], ultimaLectura);
}


//args[0]: path, args[1]: offset, args[2]: size, args[3]: datos, , args[4]: 1(ultimo) ó 0 (sigue)
//args[5]:socketCPU, args[6]: idGDT
void guardarDatos(socket_connection* connection,char ** args){ 
log_info(logger,"Se intentara guardar datos en el archivo %s",args[0]);
int cambiar = 1;
t_metadata_bitmap * bitMap = malloc(sizeof(t_metadata_bitmap));	
t_bloques * bitmapBloques = NULL;
t_archivo *  archivo= malloc(sizeof(t_archivo));
t_list * bloquesLibres = list_create();
t_list * bloquesOcupados = list_create();
char estadoGuardado[2] = "0";
char* datos = args[3];
archivo->path = args[0];
off_t  offset = atoi(args[1]);
int offsetDeBloque;
if(offset < TAM_BLOQUE)
	offsetDeBloque = offset;
else
	offsetDeBloque = offset % TAM_BLOQUE;
size_t tsize =  atoi(args[2]);
int tamanioGuardado = 0;
archivo->size =  tsize;	
archivo->fd = verificarSiExisteArchivo(archivo->path);
int bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
int bloqueFinal;
if( (offset + tsize - 1) > 0)
	bloqueFinal = (offset + tsize - 1) / TAM_BLOQUE;
else
	bloqueFinal = 0;
char ** bloques = obtenerBloques(archivo->path);
char * buffer = string_new();
char * bufferBloques;
bitMap->bitarray = crearBitmap(CANT_BLOQUES);
void cicloSeteoBloques(){ 
	for(int i = 0; i < CANT_BLOQUES; i++)
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
}
pthread_mutex_lock(&mdjInterfaz);
if(archivo->fd == -1)
{
 log_info(logger,"El archivo %s no existe",archivo->path);
 estadoGuardado[0] = '-';
estadoGuardado[1] = '1';
runFunction(connection->socket,"MDJ_DAM_respuestaFlush",5,args[6],datos,estadoGuardado, args[5],args[4]);
list_destroy(bloquesLibres);
list_destroy(bloquesOcupados);
aplicarRetardo();
pthread_mutex_unlock(&mdjInterfaz);
return;
}
else if (bloqueInicial == -1 || bloqueFinal >= cantElementos2(bloques))
{	
	cicloSeteoBloques();
	if(bloqueInicial != -1){
bitmapBloques = asignarBloques(bloquesLibres,bloquesOcupados,(bloqueFinal-bloqueInicial)*TAM_BLOQUE);
	}
	else{
bitmapBloques = asignarBloques(bloquesLibres,bloquesOcupados,tsize);
	}

char* temp = string_new();
string_append(&temp, "[");
for(int l =0; l< cantElementos2(bloques); l++){
	string_append(&temp, bloques[l]);
	string_append(&temp, ",");
}
if(bitmapBloques != NULL){
	for(int s=0; s < bitmapBloques->bloques;s++){
		string_append_with_format(&temp,"%s,",bitmapBloques->bloqArchivo[s]);
		free(bitmapBloques->bloqArchivo[s]);
	}
}
char * archTemp = string_new();
archTemp = string_substring(temp,0,strlen(temp) - 1);
string_append(&archTemp, "]");

char * rutaArchConf = string_new();
char* tamanioConfig = string_new();

string_append(&rutaArchConf,obtenerPtoMontaje());
string_append(&rutaArchConf,"/Archivos");
string_append(&rutaArchConf,archivo->path);
t_config * config = config_create(rutaArchConf);
int tamanio = config_get_int_value(config, "TAMANIO");
config_destroy(config);

char string_tam[4];
sprintf(string_tam, "%i", tamanio + tsize);
string_append(&tamanioConfig,"TAMANIO=");
string_append(&tamanioConfig,string_tam);
string_append(&tamanioConfig,"\n");
string_append(&tamanioConfig,"BLOQUES=");
string_append(&tamanioConfig,archTemp);

char* stringNuletes = string_repeat('\n', strlen(tamanioConfig)+1);
archivo-> fd = open(rutaArchConf,O_RDWR);
lseek(archivo->fd,0, SEEK_SET);
write(archivo->fd, stringNuletes,strlen(tamanioConfig)+1);
lseek(archivo->fd,0, SEEK_SET);
write(archivo->fd, tamanioConfig,strlen(tamanioConfig));
close(archivo-> fd);

bloques = obtenerBloques(archivo->path);
bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
cambiar = 0;
}

while (tamanioGuardado < tsize)//bloqueInicial <= bloqueFinal && bloques[bloqueInicial] != NULL)
{
int posBloque = atoi(bloques[bloqueInicial]);
int longitud; 
if (offsetDeBloque + (tsize-tamanioGuardado) <= TAM_BLOQUE ) 
{
	longitud = tsize-tamanioGuardado;
}
else
{
	longitud = TAM_BLOQUE - offsetDeBloque;
	if(longitud < 0)
		longitud *= -1;
}
char* datosAEscribir = string_substring(datos,tamanioGuardado,longitud);
int bloquecito = atoi(bloques[bloqueInicial]);
escribirBloque(bloquecito, offsetDeBloque,longitud, datosAEscribir);
bloqueInicial++;
tamanioGuardado += longitud;
offsetDeBloque = 0;
}
if(cambiar){
char* temp = string_new();
string_append(&temp, "[");
for(int l =0; l< cantElementos2(bloques); l++){
	string_append(&temp, bloques[l]);
	string_append(&temp, ",");
}
if(bitmapBloques != NULL){
	for(int s=0; s < bitmapBloques->bloques;s++){
		string_append_with_format(&temp,"%s,",bitmapBloques->bloqArchivo[s]);
		free(bitmapBloques->bloqArchivo[s]);
	}
}
char * archTemp = string_new();
archTemp = string_substring(temp,0,strlen(temp) - 1);
string_append(&archTemp, "]");

char * rutaArchConf = string_new();
char* tamanioConfig = string_new();

string_append(&rutaArchConf,obtenerPtoMontaje());
string_append(&rutaArchConf,"/Archivos");
string_append(&rutaArchConf,archivo->path);
t_config * config = config_create(rutaArchConf);
int tamanio = config_get_int_value(config, "TAMANIO");
config_destroy(config);

char string_tam[4];
sprintf(string_tam, "%i", tamanio + tsize);
string_append(&tamanioConfig,"TAMANIO=");
string_append(&tamanioConfig,string_tam);
string_append(&tamanioConfig,"\n");
string_append(&tamanioConfig,"BLOQUES=");
string_append(&tamanioConfig,archTemp);
/*
char* stringNuletes = string_repeat('\n', fs->tamanio_bloques);
archivo-> fd = open(rutaArchConf,O_RDWR);
lseek(archivo->fd,0, SEEK_SET);
write(archivo->fd, stringNuletes,fs->tamanio_bloques);
lseek(archivo->fd,0, SEEK_SET);
write(archivo->fd, tamanioConfig,strlen(tamanioConfig));
close(archivo-> fd);*/

char* stringNuletes = string_repeat('\n', strlen(tamanioConfig)+1);
FILE* archivoBloque = fopen(rutaArchConf,"r+");
fseek(archivoBloque, 0, SEEK_SET);
fwrite(stringNuletes, strlen(tamanioConfig)+1, 1, archivoBloque);
fseek(archivoBloque, 0, SEEK_SET);
fwrite(tamanioConfig, strlen(tamanioConfig), 1, archivoBloque);
fclose(archivoBloque);
}

for(int h=0; h<cantElementos2(bloques); h++){
	free(bloques[h]);
}
estadoGuardado[0] = '0';
estadoGuardado[1] = '\0';
log_trace(logger,"Se guardaron correctamente %d bytes: '%s' ",string_length(datos),datos);
pthread_mutex_unlock(&mdjInterfaz);

aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_respuestaFlush",5,args[6],datos,estadoGuardado, args[5],args[4]);
list_destroy(bloquesLibres);
list_destroy(bloquesOcupados);
free(bitmapBloques);
}


//args[0]: path, args[1]: offset, args[2]: size, args[3]: datos, , args[4]: 1(ultimo) ó 0 (sigue)
/*void guardarDatos(socket_connection* connection,char ** args){
t_archivo * archivo = malloc(sizeof(t_archivo));
t_metadata_filesystem * fs = obtenerMetadata();
archivo->path = args[0];
off_t offset = atoi(args[1]);
size_t size = atoi(args[2]);
char * buffer = args[3];
char ** bloques = obtenerBloques(archivo->path);
int bloqueInicial = obtenerBloqueInicial(archivo->path,offset);
//int bloqueFinal = (offset + size) / fs->tamanio_bloques;
int bloqueFinal;
if( (offset + size - 1) > 0)
	bloqueFinal = (offset + size - 1) / fs->tamanio_bloques;
else
	bloqueFinal = 0;
int offRestante = offset - (offset /fs->tamanio_bloques) * fs->tamanio_bloques;
int longitud; 
int estadoGuardado; 
if(cantElementos2(bloques) == 0)
{
 log_info(logger,"El archivo %s no existe",archivo->path);
 estadoGuardado = -1;  
}
else if (bloqueInicial == -1)
{
log_error(logger,"El offset no puede ser mayor al tamanio del archivo");
estadoGuardado = -2;
}
else
{
pthread_mutex_lock(&mdjInterfaz);	
if (size > fs->tamanio_bloques)
{
longitud = fs->tamanio_bloques;
 }
 else
 { 
 longitud = size;
 }
 if (offsetDeBloque + tsize <= tamanioBufferBloques ) 
{
longitud = tsize;
}
else
{
longitud = tamanioBufferBloques - offsetDeBloque;
if(longitud < 0)
	longitud *= -1;
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
pthread_mutex_unlock(&mdjInterfaz);
estadoGuardado = 0;
}
aplicarRetardo();
char * estado = string_itoa(estadoGuardado);
runFunction(connection->socket,"MDJ_DAM_datosGuardados",3,args[0],estado,archivo->path);
} */



void escribirBloque(int bloque, int offset, int length, char * buffer) {
	char temp[200];
	snprintf(temp, sizeof(temp), "%s%s%i.bin", obtenerPtoMontaje(), "/Bloques/",bloque);
	FILE* archivoBloque = fopen(temp,"r+");
	fseek(archivoBloque, offset, SEEK_SET);
	fwrite(buffer, 1, length, archivoBloque);
	fclose(archivoBloque);
}


char * obtenerDatosBloque (int bloque) {
    char temp [200];
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
else if (offset == 0 || offset < TAM_BLOQUE)
{
    return 0;
}
else
{
int retorno;
retorno = offset / TAM_BLOQUE;
config_destroy(config);
return retorno;
}
}


//args[0]: idGDT, args[1]: arch, args[2]: socketCPU
void borrarArchivo(socket_connection* connection,char ** args){
t_archivo * archivo= malloc(sizeof(t_archivo));
archivo->path = args[1];
t_bitarray * bitarray = crearBitmap(CANT_BLOQUES);
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
pthread_mutex_lock(&mdjInterfaz);
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
pthread_mutex_unlock(&mdjInterfaz);
}
char * strEstado = string_itoa(archivo->estado);
aplicarRetardo();
runFunction(connection->socket,"MDJ_DAM_resultadoBorradoArchivo",4,args[0], strEstado, archivo->path, args[2]);
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
if(array == NULL)
	return 0;
while (array[cont] != NULL){
cont = cont + 1;	
}	
return cont;
}

int cantElementos1(char * array)
{
int cont = 0;
while (array[cont] != '\0'){ //caracter nulo
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
string_append(&temp,"/Archivos");
string_append(&temp,path);
t_config * config = config_create(temp);
char **  bloques = config_get_array_value(config,"BLOQUES");
free(temp);
return bloques;
}
else{
 return  NULL;  
}
}

char ** obtenerBloquesDesdeConsola(char * path){
t_config * config = config_create(path);
char **  bloques = config_get_array_value(config,"BLOQUES");
config_destroy(config);
return bloques;
}

t_bloques *  asignarBloques(t_list * libres,t_list * ocupados ,size_t size)
{
t_bloques * bloques = malloc(sizeof(t_bloques));
t_list * temp = list_create();
t_bitarray * bitarray = crearBitmap(CANT_BLOQUES);
int nBloques = 0;
if(size % TAM_BLOQUE == 0)
{
nBloques = size / TAM_BLOQUE;
}
else
{
nBloques = (size / TAM_BLOQUE) + 1;    
}
if (list_size(libres) >= nBloques){
temp  = list_take_and_remove(libres,nBloques);
list_add_all(ocupados,temp);
}
else
{
log_error(logger,"No hay bloques disponibles , vuelva a intentarlo");
return NULL;
}
bloques->bloqLibres = _list_duplicate(libres);
bloques->bloqOcupados = _list_duplicate(ocupados);
bloques->bloqArchivo = calloc(nBloques,sizeof(char*));
//t_list* listaBloq = list_create();
for(int i = 0; i <list_size(temp); i++){
	int blo = (int)list_get(temp,i);
	char* charBloq = string_itoa(blo);
	bloques->bloqArchivo[i] = malloc(strlen(charBloq) + 1);
	strcpy(bloques->bloqArchivo[i], charBloq);
}
for(int i = 0;i < nBloques;i++){
	int bitInd = atoi(bloques->bloqArchivo[i]);
	bitarray_set_bit(bitarray,(off_t)bitInd);	
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

t_list* _list_duplicate(t_list* self) {
	t_list* duplicated = list_create();
	list_add_all(duplicated, self);
	return duplicated;
}
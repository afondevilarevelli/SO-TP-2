#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "libDAM.h"

void disconnect(socket_connection* socketInfo);

int main(void){
	 signal(SIGINT, cerrarPrograma);
	
       	configure_logger();
	datosConfigDAM =read_and_log_config("DAM.config");
	//diccionarios

        callableRemoteFunctions = dictionary_create();

  pthread_mutex_init(&m_pedido, NULL);
	
	//--------------------------------------------------------------------
	//Dicionarios del mdj
    dictionary_put(callableRemoteFunctions, "MDJ_DAM_existeArchivo", &MDJ_DAM_existeArchivo);
    dictionary_put(callableRemoteFunctions, "MDJ_DAM_resultadoCreacionArchivo",&MDJ_DAM_resultadoCreacionArchivo);
    dictionary_put(callableRemoteFunctions,"MDJ_DAM_respuestaDatos",&MDJ_DAM_respuestaCargaGDT);
    dictionary_put(callableRemoteFunctions, "MDJ_DAM_resultadoBorradoArchivo",&MDJ_DAM_resultadoBorradoArchivo);
    dictionary_put(callableRemoteFunctions, "MDJ_DAM_respuestaFlush", &MDJ_DAM_respuestaFlush);
    dictionary_put(callableRemoteFunctions, "MDJ_DAM_avisoSAFAResultadoDTBDummy",&MDJ_DAM_avisarResultadoDTB);
	//--------------------------------------------------------------------
  //dicionarios de CPU
      dictionary_put(callableRemoteFunctions, "CPU_DAM_solicitudCargaGDT", &CPU_DAM_solicitudCargaGDT);
      dictionary_put(callableRemoteFunctions, "CPU_DAM_existeArchivo", &CPU_DAM_existeArchivo);
      dictionary_put(callableRemoteFunctions, "identificarProcesoEnDAM", &identificarProceso);
      dictionary_put(callableRemoteFunctions, "CPU_DAM_crearArchivo", &CPU_DAM_crearArchivo);
      dictionary_put(callableRemoteFunctions, "CPU_DAM_borrarArchivo", &CPU_DAM_borrarArchivo); 
      dictionary_put(callableRemoteFunctions, "CPU_DAM_solicitudDeFlush", &CPU_DAM_solicitudDeFlush);
  //diccionarios de FM9
      dictionary_put(callableRemoteFunctions, "FM9_DAM_archivoCargado", &FM9_DAM_archivoCargadoCorrectamente);
      dictionary_put(callableRemoteFunctions, "FM9_DAM_respuestaFlush", &FM9_DAM_respuestaFlush);
      
    
	socketSAFA = connectServer("172.17.0.1", 8001,callableRemoteFunctions, &disconnect, NULL);
  //CUANDO ME CONECTO AL SAFA LE DIGO QUE SOY EL PROCESO DAM (para manejar estadoCorrupto)
  runFunction(socketSAFA,"identificarNuevaConexion",1,"DAM"); 
	socketFM9 = connectServer("172.17.0.1",8003, callableRemoteFunctions, &disconnect, NULL);
	socketMDJ = connectServer("172.17.0.1", 8002, callableRemoteFunctions, &disconnect, NULL);
         
	
	if(socketSAFA== -1  ){
	log_error(logger,"no se pudo conectar con SAFA");
	 }
        else  {
        log_info(logger,"me conecto al SAFA");
         runFunction(socketSAFA,"identificarProcesoEnSAFA",1,"DAM");        
         }

     if (socketFM9 == -1) {
      log_error(logger,"no se pudo conectar con FM9");
       }
       else  {
        log_info(logger,"me conecto al FM9");
         runFunction(socketFM9,"identificarProcesoEnFM9",1,"DAM");
         }
      

        if(socketMDJ == -1){
        log_error(logger,"no se pudo conectar con MDJ");
        }
       else {      
        log_info(logger,"me conecto al MDJ");
      runFunction(socketMDJ,"identificarProcesoEnMDJ",1,"DAM");
      //runFunction(socketMDJ,"crearArchivo",3,"0","asuanito.txt","200");
      //runFunction(socketMDJ,"crearArchivo",3,"0","equipos/juanito.txt","250");
      /*char * buffer = "wait equipos ssssssssssssssssssssssssss";
      char size[3];
      sprintf(size,"%i",strlen(buffer)+1);
      runFunction(socketMDJ,"guardarDatos",5,"0","equipos/juanito.txt","100","4","asd");*/
      //runFunction(socketMDJ,"obtenerDatos",4,"0","equipos/juanito.txt","120","200");
    //runFunction(socketMDJ,"borrarArchivo",2,"0","equipos/juanito.txt");
      //runFunction(socketMDJ,"borrarArchivo",2,"0","asuanito.txt");
        //runFunction(socketMDJ,"guardarDatos",4,"equipos/juanito.txt","100","10","hola manola");
        
       }
      

        log_info(logger,"voy a escuchar el puerto %d ",datosConfigDAM->puertoEscucha);

	  createListen(datosConfigDAM->puertoEscucha, NULL,callableRemoteFunctions, &disconnect, NULL);
       

 	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	return EXIT_SUCCESS;
}

//FUNCIONES
void disconnect(socket_connection* socketInfo) {
  log_info(logger,"socket n° %d se ha desconectado.\n", socketInfo->socket);
}



void cerrarPrograma() {
    log_info(logger, "Voy a cerrar DAM");
    close_logger();
     dictionary_destroy(callableRemoteFunctions);
     if(datosConfigDAM->IPSAFA != NULL)
      free(datosConfigDAM->IPSAFA);
     if(datosConfigDAM->IPMDJ != NULL)
      free(datosConfigDAM->IPMDJ);
     if(datosConfigDAM->IPFM9 != NULL)
      free(datosConfigDAM->IPFM9);
     free(datosConfigDAM);
     pthread_mutex_destroy(&m_pedido);
    pthread_mutex_unlock(&mx_main);
    pthread_mutex_destroy(&mx_main);
}

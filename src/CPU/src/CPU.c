#include "libCPU.h"
#include <signal.h>

void cerrarPrograma(); 

int main() {
  signal(SIGINT, cerrarPrograma);

  configure_logger();
  datosCPU = read_and_log_config("CPU.config");

  callableRemoteFunctionsCPU = dictionary_create();
  

  int socketDAM = connectServer(datosCPU->ipD, datosCPU->puertoD, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConDAM(&socketDAM);

  int socketSAFA = connectServer(datosCPU->ipS, datosCPU->puertoS, callableRemoteFunctionsCPU, &disconnect, NULL);
  //CUANDO ME CONECTO AL SAFA LE DIGO QUE SOY UN PROCESO CPU
  runFunction(socketSAFA,"identificarNuevaConexion",1,"CPU");
  
  intentandoConexionConSAFA(&socketSAFA);

  int socketFM9 = connectServer(datosCPU->ipF, datosCPU->puertoF, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConFM9(&socketFM9);
  
  pthread_mutex_init(&m_main, NULL);
	pthread_mutex_lock(&m_main);
	pthread_mutex_lock(&m_main); 

  return 0;

}

void cerrarPrograma() {
  disconnect();
  close_logger();
  free(datosCPU);

  dictionary_destroy(callableRemoteFunctionsCPU);
  pthread_mutex_unlock(&m_main);
  pthread_mutex_destroy(&m_main);
}
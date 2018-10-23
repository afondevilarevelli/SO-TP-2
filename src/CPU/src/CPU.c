#include "libCPU.h"
#include <signal.h>

void cerrarPrograma(); 
void destruirScriptsGDT();

int main() {
  signal(SIGINT, cerrarPrograma);

  listaScriptsGDT = list_create();
  pthread_mutex_init(&m_busqueda, NULL);
  pthread_mutex_init(&m_listaScriptsGDT, NULL);
  configure_loggerCPU();
  datosCPU = read_and_log_configCPU("CPU.config");

  callableRemoteFunctionsCPU = dictionary_create();
  dictionary_put(callableRemoteFunctionsCPU,"ejecutarCPU",&permisoConcedidoParaEjecutar);
  dictionary_put(callableRemoteFunctionsCPU,"establecerQuantumYID",&establecerQuantumYID);
  

  socketDAM = connectServer(datosCPU->ipD, datosCPU->puertoD, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConDAM(&socketDAM);

  socketSAFA = connectServer(datosCPU->ipS, datosCPU->puertoS, callableRemoteFunctionsCPU, &disconnect, NULL);
  //CUANDO ME CONECTO AL SAFA LE DIGO QUE SOY UN PROCESO CPU
  runFunction(socketSAFA,"identificarNuevaConexion",1,"CPU");
  
  intentandoConexionConSAFA(&socketSAFA);

  socketFM9 = connectServer(datosCPU->ipF, datosCPU->puertoF, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConFM9(&socketFM9);
  
  pthread_mutex_init(&m_main, NULL);
	pthread_mutex_lock(&m_main);
	pthread_mutex_lock(&m_main); 

  return 0;

}

void cerrarPrograma() {
  disconnect();
  close_loggerCPU();
  free(datosCPU);
  list_destroy_and_destroy_elements(listaScriptsGDT, (void*) &destruirScriptsGDT);

  dictionary_destroy(callableRemoteFunctionsCPU);
  pthread_mutex_destroy(&m_busqueda);
  pthread_mutex_destroy(&m_listaScriptsGDT);
  pthread_mutex_unlock(&m_main);
  pthread_mutex_destroy(&m_main);
}

void destruirScriptsGDT(scriptGDT* sc){
  fclose(sc->scriptf);
  free(sc);
}

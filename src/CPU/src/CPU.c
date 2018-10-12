#include "libCPU.h"

int main() {
  
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
  
  exit_gracefully(1);

}


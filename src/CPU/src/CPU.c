#include "libCPU.h"

int main() {

  configure_logger();
  datosCPU = read_and_log_config("CPU.config");

  callableRemoteFunctionsCPU = dictionary_create();

  dictionary_put(callableRemoteFunctionsCPU, "SAFA_CPU_handshake", &SAFA_CPU_handshake);
  dictionary_put(callableRemoteFunctionsCPU, "DAM_CPU_handshake", &DAM_CPU_handshake);
  dictionary_put(callableRemoteFunctionsCPU, "FM9_CPU_handshake", &FM9_CPU_handshake);

  int socketDAM = connectServer(datosCPU->ipD, datosCPU->puertoD, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConDAM(&socketDAM);

  int socketSAFA = connectServer(datosCPU->ipS, datosCPU->puertoS, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConSAFA(&socketSAFA);

  int socketFM9 = connectServer(datosCPU->ipF, datosCPU->puertoF, callableRemoteFunctionsCPU, &disconnect, NULL);

  intentandoConexionConFM9(&socketFM9);

  exit_gracefully(1);

}


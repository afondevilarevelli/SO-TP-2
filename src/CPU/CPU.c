#include "libCPU/libCPU.h"
#include "../shared/mySocket.h"
#include "../shared/buffer.h"

char* saludo = "Hola S-AFA, soy la CPU";

int main() {

  configure_logger();
  read_and_log_config("CPU.config");
  int socket = connectTo(IP, (int) datosCPU->puertoS); //La idea es que no tome un entero el ip sino un char*
  enviando_mensaje(socket, saludo);
  esperando_respuesta(socket);
  exit_gracefully(0);


  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include "libFM9.h"

int main(void) {
	configure_logger();

	datosConfigFM9 = read_and_log_config("FM9.config");

	callableRemoteFunctions = dictionary_create();

	dictionary_put(callableRemoteFunctions, "startProcess", &startProcess);

	log_info(logger, "Voy a escuchar el puerto: %d", datosConfigFM9->puerto);

	createListen(datosConfigFM9->puerto, &connectionNew,
			callableRemoteFunctions, &disconnect, NULL);

	log_info(logger, "Estoy escuchando el puerto: %d", datosConfigFM9->puerto);

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	//pthread_mutex_lock(&mx_main);

	close_logger();

	free(datosConfigFM9);
	dictionary_destroy(callableRemoteFunctions);
	pthread_mutex_unlock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	return EXIT_SUCCESS;
}


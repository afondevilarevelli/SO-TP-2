#include <stdio.h>
#include <stdlib.h>
#include "libFM9.h"

int main(void) {
	configure_logger();

	read_and_log_config("FM9.config");

	close_logger();

	return EXIT_SUCCESS;
}


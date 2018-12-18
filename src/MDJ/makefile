sources = src/MDJ.c src/consolaMDJ.c src/libMDJ.c src/interfaz.c
sharedLibraries = ../sample-socket/socket.c
pathLibraries = -lcommons -lreadline -lpthread -lssl -lcrypto 

MDJ: $(sources)
	gcc -g -o MDJ $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm MDJ  *.log
	rm *.o
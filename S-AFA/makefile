sources = src/SAFA.c  src/consolaSAFA.c  src/libSAFA.c
socketLibraries = ../sample-socket/socket.c   ../gestionArchConfig/gestionArchConf.c
pathLibraries = -lreadline -lcommons -lpthread 

SAFA: $(sources)
	gcc -g -o SAFA $(sources) $(socketLibraries) $(pathLibraries)

clean:
	rm SAFA
	rm *.o
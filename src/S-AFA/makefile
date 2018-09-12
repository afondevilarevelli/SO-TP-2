sources = SAFA.c consolaSAFA.c  libSAFA/libSAFA.c
socketLibraries = ../sample-socket/socket.c
pathLibraries = -lreadline -lcommons -lpthread 

SAFA: $(sources)
	gcc -g -o SAFA $(sources) $(socketLibraries) $(pathLibraries)

clean:
	rm SAFA
	rm *.o
sources = src/FM9.c  src/libFM9.c  
sharedLibraries = ../sample-socket/socket.c   ../Utils/gestionArchConf.c   ../Utils/gestionProcesos.h
pathLibraries = -lreadline -lcommons -lpthread 

FM9: $(sources)
	gcc -g -o FM9 $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm FM9
	
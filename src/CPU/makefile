sources = src/CPU.c src/libCPU.c src/parser.c
sharedLibraries = ../sample-socket/socket.c   ../Utils/gestionArchConf.c   ../Utils/gestionProcesos.c
pathLibraries = -lreadline -lcommons -lpthread 

CPU: $(sources)
	gcc -g -o CPU $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm CPU
	rm *.o

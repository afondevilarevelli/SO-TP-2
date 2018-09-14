sources = CPU.c   libCPU/libCPU.c
sharedLibraries = ../sample-socket/socket.c
pathLibraries = -lreadline -lcommons -lpthread 

CPU: $(sources)
	gcc -g -o CPU $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm CPU
	rm *.o
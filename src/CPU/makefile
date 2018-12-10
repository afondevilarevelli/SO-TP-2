sources = src/CPU.c  src/libCPU.c  
sharedLibraries = ../sample-socket/socket.c 
pathLibraries = -lreadline -lcommons -lpthread 

CPU: $(sources)
	gcc -g -o CPU $(sources) $(sharedLibraries) $(pathLibraries)
	cp CPU ../CPU1/CPU
	cp CPU ../CPU2/CPU
	cp CPU ../CPU3/CPU
	cp CPU ../CPU4/CPU
	cp CPU.config ../CPU1/CPU.config
	cp CPU.config ../CPU2/CPU.config
	cp CPU.config ../CPU3/CPU.config
	cp CPU.config ../CPU4/CPU.config
	

clean:
	rm CPU
	rm *.o

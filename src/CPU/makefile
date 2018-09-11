sources = CPU.c   libCPU/libCPU.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lreadline -lcommons #-lpthread 

CPU: $(sources)
	gcc -g -o CPU $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm CPU
	rm *.o
sources = SAFA.c consolaSAFA.c  libSAFA/libSAFA.c
sharedLibraries = ../shared/mySocket.c ../shared/buffer.c
pathLibraries = -lreadline -lcommons #-lpthread 

SAFA: $(sources)
	gcc -g -o SAFA $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm SAFA
	rm *.o
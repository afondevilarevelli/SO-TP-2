sources = DAM.c	libDAM.c
sharedLibraries	= ../shared/mySocket.c ../shared/buffer.c
pathLibraries	=	-lreadline -lcommons #-lpthread				
DAM: $(sources)	
	gcc -g -o DAM $(sources) $(sharedLibraries) $(pathLibraries)		
clean:	
	rm DAM	
	rm *.o

sources = src/MDJ.c  src/libMDJ.c  
sharedLibraries = ../sample-socket/socket.c  ../Utils/gestionArchConf.c  ../Utils/gestionProcesos.c
pathLibraries = -lreadline -lcommons -lpthread 

MDJ: $(sources)
	gcc -g -o MDJ $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm MDJ
	
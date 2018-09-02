sources = SAFA.c consolaSAFA.c  
#sharedLibraries = 
pathLibraries = -lreadline #-lcommons -lpthread 

SAFA: $(sources)
	gcc -g -o SAFA $(sources) $(sharedLibraries) $(pathLibraries)

clean:
	rm SAFA
	rm *.o
/*
 * gestionArchConfig.c
 *
 *  Created on: 3 sep. 2018
 *      Author: utns
 */


#include "gestionArchConf.h"


// determinar si el archivo ya fue creado o no
int existeArchivoConf(char * path)
{
	   FILE * f = fopen(path,"r");
		if (f){
		return 1;
        }
		else {
		return 0;
		}
        free(path);
        fclose(f);

}

// crea el archivo config
void crearArchivoConfig(char * path)
{

	if(!existeArchivoConf(path)){
	     FILE * f =  txt_open_for_append(path);
               txt_close_file(f);
	}
free(path);
}

//escribe en el las key
void setKey(char * path ,char * key)
{

   FILE * arch = txt_open_for_append(path);
   char* string = string_from_format("\n%s\n",key);
   txt_write_in_file(arch,string);
   free(string);
   free(path);
   txt_close_file(arch);
}

void setValue(t_config * conf,char * path,char * key,char * value)
{
	           conf = config_create(path);

			if(configPoseeKey(conf,path,key))
			{
				config_set_value(conf,key,value);
                               
			}
           config_destroy(conf);
}

int configPoseeKey(t_config * conf,char * path,char * key){

	conf = config_create(path);
	return config_has_property(conf,key);
       free(path);
      free(key);
       config_destroy(conf);
}

int  obtenerInt(t_config * conf,char * path , char * key)
{
	       conf = config_create(path);
		if(configPoseeKey(conf,path,key))
		{
		return config_get_int_value(conf,key);
		}
		else
		{
		printf("No existe en %s la key %s",path,key);
		return 0;
		}
   free(path);
   free(key);
   config_destroy(conf);
}



char * obtenerString(t_config * conf,char * path,char * key)
{
	 conf = config_create(path);
	if(configPoseeKey(conf,path,key))
	{
	return config_get_string_value(conf,key);
	}
	else
	{
	printf("No existe en %s la key %s",path,key);
	return 0;
	}

free(path);
free(key);
 config_destroy(conf);
}
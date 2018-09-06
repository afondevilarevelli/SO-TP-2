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
		fclose(f);
		return 1;
        }
		else {
		return 0;
		}
}

// crea el archivo config
void crearArchivoConfig(char * path)
{
	if(!existeArchivoConf(path)){
		txt_open_for_append(path);
	}
}

//escribe en el las key
void setKey(char * path ,char * key)
{

   FILE * arch = txt_open_for_append(path);
   char* string = string_from_format("\n%s\n",key);
   txt_write_in_file(arch,string);
   txt_close_file(arch);
}

void setValue(char * path,char * key,char * value)
{
	t_config * conf = config_create(path);

			if(configPoseeKey(path,key))
			{
				config_set_value(conf,key,value);
			}
}

int configPoseeKey(char * path,char * key){

	t_config * conf = config_create(path);
	return config_has_property(conf,key);
}

int  obtenerInt(char * path , char * key)
{
	    t_config * conf = config_create(path);
		if(configPoseeKey(path,key))
		{
		return config_get_int_value(conf,key);
		}
		else
		{
		printf("No existe en %s la key %s",path,key);
		return 0;
		}
}



char * obtenerString(char * path,char * key)
{
	t_config * conf = config_create(path);
	if(configPoseeKey(path,key))
	{
	return config_get_string_value(conf,key);
	}
	else
	{
	printf("No existe en %s la key %s",path,key);
	return 0;
	}
}
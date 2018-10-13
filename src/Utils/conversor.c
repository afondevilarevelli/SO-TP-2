#include "conversor.h"

char intToChar(int num){
    char c;
    switch(num){
        case 0: c = '0'; break;
        case 1: c = '1'; break;
        case 2: c = '2'; break;
        case 3: c = '3'; break;
        case 4: c = '4'; break;
        case 5: c = '5'; break;
        case 6: c = '6'; break;
        case 7: c = '7'; break;
        case 8: c = '8'; break;
        case 9: c = '9'; break;
    }
    return c;
}

int charToInt(char ch){
    if( ch == '0' ){
        return 0;
    } else if( ch == '1' ){
        return 1;
    } else if( ch == '2' ){
        return 2;
    } else if( ch == '3' ){
        return 3;
    } else if( ch == '4' ){
        return 4;
    } else if( ch == '5' ){
        return 5;
    } else if( ch == '6' ){
        return 6;
    } else if( ch == '7' ){
        return 7;
    } else if( ch == '8' ){
        return 8;
    } else if( ch == '9' ){
        return 9;
    } else{
        return -1;
    }
}

int convertirAInt(char* pal){
    int i, resultado = 0;
    int potencia = pow(10, strlen(pal)-1 );
    for(i = 0; i < strlen(pal); i++){
        if(charToInt(pal[i]) == 0){
            resultado += potencia;
        }else{
            resultado += potencia * charToInt(pal[i]);
        }    
        potencia = potencia / 10;
    }
    return resultado;
}

char* convertirAString(int num){
    char* aux[ cantidadDeDigitos(num) ];
    int i;
    for(i = 0; i < cantidadDeDigitos; i++){
        aux[i] = 
    }
}

int cantidadDeDigitos(int num){
    int cant = 1;
    num = num/10;
    while(num > 1){
        cant++;
        num = num/10;
    }
    return cant;
}
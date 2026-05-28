#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <ctype.h> 

void leer_texto(char *buffer, int tamano, const char *mensaje);
int leer_entero(const char *mensaje);
int validar_cedula_ecuatoriana(const char *cedula);


int validar_telefono_movil(const char *telefono);

#endif

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <ctype.h> 

void leer_texto(char *buffer, int tamano, const char *mensaje);
int leer_entero(const char *mensaje);
int validar_cedula_ecuatoriana(const char *cedula);
int validar_telefono_movil(const char *telefono);


void leer_contrasena_oculta(char *buffer, int tamano, const char *mensaje);
int validar_fecha_futura(const char *fecha_str);
int validar_edad_realista(int edad);
int validar_motivo_cita(const char *motivo);
void limpiar_pantalla();
void pausa_sistema();

#endif

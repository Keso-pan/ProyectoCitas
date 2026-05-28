#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "../include/validaciones.h"

// ============================================================================
// FUNCIÓN 1: LEER TEXTO NORMAL
// ============================================================================
void leer_texto(char *buffer, int tamano, const char *mensaje) {
	printf("%s", mensaje);
	fflush(stdout);
	fgets(buffer, tamano, stdin);
	size_t len = strlen(buffer);
	if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
	fflush(stdin);
}


// ============================================================================
// FUNCIÓN 2: LEER CONTRASEÑA OCULTA (NUEVA - CRÍTICA)
// ============================================================================
void leer_contrasena_oculta(char *buffer, int tamano, const char *mensaje) {
	printf("%s", mensaje);
	fflush(stdout);
	
	int pos = 0;
	int ch;
	
#ifdef _WIN32
	// Windows: usar conio.h
#include <conio.h>
	
	while (pos < tamano - 1) {
		ch = _getch();  // Leer sin mostrar
		
		if (ch == '\r' || ch == '\n') {
			printf("\n");
			break;
		} else if (ch == '\b') {  // Backspace
			if (pos > 0) {
				pos--;
				printf("\b \b");  // Borrar último * de la pantalla
				fflush(stdout);
			}
		} else if (ch == 27) {  // ESC - cancelar
			printf("\n [!] Entrada cancelada.\n");
			buffer[0] = '\0';
			return;
		} else if (!iscntrl(ch)) {  // Solo caracteres válidos
			buffer[pos++] = ch;
			printf("*");  // Mostrar asterisco
			fflush(stdout);
		}
	}
#else
	// Linux/macOS: Usar termios
#include <termios.h>
#include <unistd.h>
	
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ECHO);  // Desactivar echo
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	while (pos < tamano - 1) {
		ch = getchar();
		
		if (ch == '\n' || ch == '\r') {
			printf("\n");
			break;
		} else if (ch == 127 || ch == '\b') {  // Backspace/Delete
			if (pos > 0) {
				pos--;
				printf("\b \b");
				fflush(stdout);
			}
		} else if (ch == 27) {  // ESC - cancelar
			printf("\n [!] Entrada cancelada.\n");
			tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
			buffer[0] = '\0';
			return;
		} else if (!iscntrl(ch)) {
			buffer[pos++] = ch;
			printf("*");
			fflush(stdout);
		}
	}
	
	// Restaurar configuración de terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
	
	buffer[pos] = '\0';
	fflush(stdin);
}


// ============================================================================
// FUNCIÓN 3: LEER ENTERO
// ============================================================================
int leer_entero(const char *mensaje) {
	char temp_buff[64];
	leer_texto(temp_buff, 64, mensaje);
	for(int i = 0; temp_buff[i] != '\0'; i++) {
		if(!isdigit(temp_buff[i])) return -1;
	}
	if(strlen(temp_buff) == 0) return 0;
	return atoi(temp_buff);
}


// ============================================================================
// FUNCIÓN 4: VALIDAR CÉDULA ECUATORIANA
// ============================================================================
int validar_cedula_ecuatoriana(const char *cedula) {
	if (strlen(cedula) != 10) return 0;
	for(int i=0; i<10; i++) if(!isdigit(cedula[i])) return 0;
	int suma_total = 0;
	int codigo_provincia = (cedula[0] - '0') * 10 + (cedula[1] - '0');
	if (codigo_provincia < 1 || codigo_provincia > 24) return 0;
	int coeficientes[] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
	for (int k = 0; k < 9; k++) {
		int digito_actual = cedula[k] - '0';
		int producto = digito_actual * coeficientes[k];
		if (producto >= 10) producto -= 9;
		suma_total += producto;
	}
	int digito_verificador = cedula[9] - '0';
	int decena_superior = ((suma_total / 10) + 1) * 10;
	if (suma_total % 10 == 0) decena_superior = suma_total;
	return ((decena_superior - suma_total) == digito_verificador);
}


// ============================================================================
// FUNCIÓN 5: VALIDAR TELÉFONO MÓVIL (MEJORADA)
// ============================================================================
int validar_telefono_movil(const char *telefono) {
	// Longitud exacta
	if(strlen(telefono) != 10) return 0;
	
	// Solo números
	for(int i=0; i<10; i++) {
		if(!isdigit(telefono[i])) return 0;
	}
	
	// Que empiece con 09
	if(telefono[0] != '0' || telefono[1] != '9') return 0;
	
	// Validar operador (tercer dígito debe ser válido 1-8)
	char operador = telefono[2];
	if(operador < '1' || operador > '8') return 0;
	
	return 1;
}


// ============================================================================
// FUNCIÓN 6: VALIDAR FECHA FUTURA (NUEVA - CRÍTICA)
// ============================================================================
int validar_fecha_futura(const char *fecha_str) {
	// Validar formato DD-MM-AAAA
	if (strlen(fecha_str) != 10) return 0;
	if (fecha_str[2] != '-' || fecha_str[5] != '-') return 0;
	
	// Extraer día, mes, año
	int dia, mes, anio;
	if (sscanf(fecha_str, "%d-%d-%d", &dia, &mes, &anio) != 3) return 0;
	
	// Validar rango de mes
	if (mes < 1 || mes > 12) return 0;
	
	// Tabla de días por mes
	int dias_por_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	// Validar año bisiesto
	if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
		dias_por_mes[1] = 29;
	}
	
	// Validar día
	if (dia < 1 || dia > dias_por_mes[mes - 1]) return 0;
	
	// Obtener fecha actual
	time_t t = time(NULL);
	struct tm *ahora = localtime(&t);
	
	int anio_actual = ahora->tm_year + 1900;
	int mes_actual = ahora->tm_mon + 1;
	int dia_actual = ahora->tm_mday;
	
	
	if (anio < anio_actual) return 0;  
	if (anio > anio_actual) {
		
		if (anio > anio_actual + 2) {
			printf(" [!] La cita no puede ser mas de 2 anos en el futuro.\n");
			return 0;
		}
		return 1;  
	}
	

	if (mes < mes_actual) return 0; 
	if (mes > mes_actual) return 1;  
	
	
	if (dia <= dia_actual) return 0;  
	
	return 1;  
}



int validar_edad_realista(int edad) {
	if (edad < 5) {
		printf(" [!] Edad minima recomendada: 5 anos.\n");
		return 0;
	}
	if (edad > 120) {
		printf(" [!] Edad maxima: 120 anos.\n");
		return 0;
	}
	return 1;
}



int validar_motivo_cita(const char *motivo) {
	const char *motivos_validos[] = {
		"Chequeo de rutina",
			"Fiebre aguda",
			"Dolor articular",
			"Control anual",
			"Sintomas virales",
			"Dolor de cabeza",
			"Revision general",
			"Dolor de muelas",
			"Problemas digestivos",
			"Seguimiento",
			"Otro"
	};
	
	int num_motivos = 11;
	
	for (int i = 0; i < num_motivos; i++) {
		if (strcmp(motivo, motivos_validos[i]) == 0) {
			return 1;
		}
	}
	
	
	if (strlen(motivo) > 0 && strlen(motivo) < 100) {
		return 1;
	}
	
	return 0;
}


void limpiar_pantalla() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}



void pausa_sistema() {
	printf(" [>] Presione ENTER para continuar...\n");
	fflush(stdout);
	char c = getchar();
	while (c != '\n' && getchar() != '\n');
}

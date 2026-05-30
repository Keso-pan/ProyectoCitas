#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "../include/validaciones.h"


void leer_texto(char *buffer, int tamano, const char *mensaje) {
	printf("%s", mensaje);
	fflush(stdout);
	fgets(buffer, tamano, stdin);
	size_t len = strlen(buffer);
	if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
	fflush(stdin);
}



void leer_contrasena_oculta(char *buffer, int tamano, const char *mensaje) {
	printf("%s", mensaje);
	fflush(stdout);
	
	int pos = 0;
	int ch;
	
#ifdef _WIN32

#include <conio.h>
	
	while (pos < tamano - 1) {
		ch = _getch();  
		
		if (ch == '\r' || ch == '\n') {
			printf("\n");
			break;
		} else if (ch == '\b') { 
			if (pos > 0) {
				pos--;
				printf("\b \b");  
				fflush(stdout);
			}
		} else if (ch == 27) {  
			printf("\n [!] Entrada cancelada.\n");
			buffer[0] = '\0';
			return;
		} else if (!iscntrl(ch)) {  
			buffer[pos++] = ch;
			printf("*"); 
			fflush(stdout);
		}
	}
#else
#include <termios.h>
#include <unistd.h>
	
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ECHO); 
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	while (pos < tamano - 1) {
		ch = getchar();
		
		if (ch == '\n' || ch == '\r') {
			printf("\n");
			break;
		} else if (ch == 127 || ch == '\b') {  
			if (pos > 0) {
				pos--;
				printf("\b \b");
				fflush(stdout);
			}
		} else if (ch == 27) { 
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
	
	
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
	
	buffer[pos] = '\0';
	fflush(stdin);
}


int leer_entero(const char *mensaje) {
	char temp_buff[64];
	leer_texto(temp_buff, 64, mensaje);
	for(int i = 0; temp_buff[i] != '\0'; i++) {
		if(!isdigit(temp_buff[i])) return -1;
	}
	if(strlen(temp_buff) == 0) return 0;
	return atoi(temp_buff);
}



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



int validar_telefono_movil(const char *telefono) {
	
	if(strlen(telefono) != 10) return 0;
	
	for(int i=0; i<10; i++) {
		if(!isdigit(telefono[i])) return 0;
	}
	
	if(telefono[0] != '0' || telefono[1] != '9') return 0;
	
	
	return 1;
}

int validar_fecha_futura(const char *fecha_str) {
	if (strlen(fecha_str) != 10) return 0;
	if (fecha_str[2] != '-' || fecha_str[5] != '-') return 0;
	
	int dia, mes, anio;
	if (sscanf(fecha_str, "%d-%d-%d", &dia, &mes, &anio) != 3) return 0;
	
	if (mes < 1 || mes > 12) return 0;
	
	int dias_por_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
		dias_por_mes[1] = 29;
	}
	if (dia < 1 || dia > dias_por_mes[mes - 1]) return 0;
	
	
	time_t t = time(NULL);
	struct tm *ahora = localtime(&t);
	
	
	ahora->tm_hour = 0;
	ahora->tm_min = 0;
	ahora->tm_sec = 0;
	
	
	struct tm tm_ingresada = {0};
	tm_ingresada.tm_mday = dia;
	tm_ingresada.tm_mon = mes - 1;
	tm_ingresada.tm_year = anio - 1900;
	tm_ingresada.tm_hour = 0;
	tm_ingresada.tm_min = 0;
	tm_ingresada.tm_sec = 0;
	
	time_t time_base = mktime(ahora);
	time_t time_ingresada = mktime(&tm_ingresada);
	
	if (time_ingresada == -1 || time_base == -1) return 0;
	
	
	double diferencia_dias = difftime(time_ingresada, time_base) / (60 * 60 * 24);
	
	if (diferencia_dias < 0) {
		printf(" [!] Error: No puede agendar o reprogramar citas en el pasado.\n");
		return 0;
	}
	if (diferencia_dias > 365) {
		printf(" [!] Error: La fecha excede el limite permitido (maximo 365 dias desde hoy).\n");
		return 0;
	}
	
	return 1;  
}

int validar_edad_realista(int edad) {
	if (edad < 13) {
		printf(" [!] Edad minima recomendada: 13 años.\n");
		return 0;
	}
	if (edad > 120) {
		printf(" [!] Edad maxima: 120 años.\n");
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

int validar_horario_medico(const char *horario_doc, const char *hora_cita) {
	
	if (strcmp(horario_doc, "08:00-14:00") == 0) {
		if (strcmp(hora_cita, "08:00") == 0 || strcmp(hora_cita, "10:00") == 0 || strcmp(hora_cita, "12:00") == 0) return 1;
	} 
	
	else if (strcmp(horario_doc, "14:00-20:00") == 0) {
		if (strcmp(hora_cita, "14:00") == 0 || strcmp(hora_cita, "16:00") == 0 || strcmp(hora_cita, "18:00") == 0) return 1;
	} 
	
	else if (strcmp(horario_doc, "08:00-20:00") == 0) {
		if (strcmp(hora_cita, "08:00") == 0 || strcmp(hora_cita, "10:00") == 0 || strcmp(hora_cita, "12:00") == 0 ||
			strcmp(hora_cita, "14:00") == 0 || strcmp(hora_cita, "16:00") == 0 || strcmp(hora_cita, "18:00") == 0) return 1;
	}
	return 0; 
}

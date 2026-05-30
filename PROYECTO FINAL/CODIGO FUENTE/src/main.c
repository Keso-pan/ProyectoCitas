#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>  
#include "../include/pacientes.h"
#include "../include/medicos.h"
#include "../include/citas.h"
#include "../include/validaciones.h"

extern void registrar_inasistencia(const char *cedula);
extern float obtener_multa_paciente(const char *cedula);
extern void pagar_multa_paciente(const char *cedula);
extern void mostrar_multas_admin();

int verificar_password_admin(const char *password_ingresado) {
	if (strcmp(password_ingresado, "admin123") == 0) {
		return 1;
	}
	return 0;
}



void menu_admin() {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n*************************************************\n");
		printf("          PANEL DE ADMINISTRADOR\n");
		printf("*************************************************\n");
		printf(" [a] Gestion de Personal Medico\n");
		printf(" [b] Gestion de Pacientes\n");
		printf(" [c] Control de Citas\n");
		printf(" --- REPORTES ---\n");
		printf(" [d] Exportar Lista de Medicos (TXT)\n");
		printf(" [e] Exportar Lista de Pacientes (TXT)\n");
		printf(" [f] Generar Reporte Estadistico General\n");
		printf(" [g] Ver Dashboard Gerencial (MIS)\n");
		printf(" [h] Generar Reporte Completo de Citas (TXT/HTML)\n");
		printf(" [i] Ver Multas por Inasistencia\n");
		printf(" [x] Cerrar Sesion\n");
		printf("=================================================\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Opción: ");
		op = tolower(buff[0]);
		
		if(op == 'a') gestion_medicos_admin();
		else if(op == 'b') gestion_pacientes_admin();
		else if(op == 'c') gestion_citas_admin();
		else if(op == 'd') exportar_reporte_medicos();
		else if(op == 'e') exportar_reporte_pacientes();
		else if(op == 'f') generar_reporte_general();
		else if(op == 'g') generar_reportes_mis();
		else if(op == 'h') generar_reporte_citas_completo();
		else if(op == 'i') mostrar_multas_admin();
		else if(op == 'j') exportar_reporte_multas();
	}
}


void menu_medico(const char *codigo_medico) {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n *** PANEL DEL MEDICO ***\n");
		printf("   [a] Ver mi agenda\n");
		printf("   [b] Historial de pacientes\n");
		printf("   [c] Gestionar estatus de cita\n");
		printf("   [d] Ver mi perfil profesional\n");
		printf("   [e] Modificar mis datos\n");
		printf("   ===============================\n");
		printf("   [x] Cerrar sesion\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Seleccione: ");
		op = tolower(buff[0]);
		
		if (op == 'a') ver_agenda_medico(codigo_medico);
		else if (op == 'b') ver_historial_medico(codigo_medico);
		else if (op == 'c') gestionar_estatus_cita(codigo_medico);
		else if (op == 'd') ver_perfil_medico(codigo_medico);
		else if (op == 'e') modificar_medico(codigo_medico);
	}
}



void menu_paciente(char *cedula) {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n*************************************************\n");
		printf("          PANEL PACIENTE (%s)\n", cedula);
		printf("*************************************************\n");
		printf(" [a] Solicitar Nueva Cita\n");
		printf(" [b] Ver Historial de Citas\n");
		printf(" [c] Cancelar una Cita Activa\n");
		printf(" [d] Ver Mis Datos Personales\n");
		printf(" [e] Modificar Perfil\n");
		printf(" [f] Verificar y Pagar Multas\n");
		printf(" [g] Eliminar Cuenta\n");
		printf(" [x] Cerrar Sesion\n");
		printf("===================================================\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Opcion: ");
		op = tolower(buff[0]);
		
		if(op == 'a') solicitar_cita(cedula);
		else if(op == 'b') ver_mis_citas(cedula);
		else if(op == 'c') cancelar_cita_paciente(cedula);
		else if(op == 'd') ver_perfil_paciente(cedula);
		else if(op == 'e') modificar_paciente(cedula);
		else if(op == 'f') pagar_multa_paciente(cedula);  
		else if(op == 'g') {
			if(eliminar_paciente(cedula) == 1) return;
		}
	}
}



void iniciar_sesion() {
	char credencial[30];
	char pass_input[30];
	int intentos = 0;
	const int MAX_INTENTOS = 3;
	
	printf("\n");
	printf("***************************************************************************\n");
	printf("                    SISTEMA DE SALUD\n");
	printf("                  INGRESO AL SISTEMA\n");
	printf("****************************************************************************\n");
	
	leer_texto(credencial, 30, " [!] Credencial (Cedula, Codigo Medico o Admin): ");
	
	
	if(strcmp(credencial, "admin") == 0 || strcmp(credencial, "1234") == 0) {
		printf("\n");
		leer_contrasena_oculta(pass_input, 30, " [!] Contraseña de Administrador: ");
		
		if(verificar_password_admin(pass_input)) {
			printf("\n\n [i] Bienvenido Administrador.\n");
			pausa_sistema();
			menu_admin();
		} else {
			printf("\n [!] — Contraseña de administrador incorrecta.\n");
			printf("  [!]  Acceso denegado.\n");
			pausa_sistema();
		}
		return;
	}
	
	
	if(strstr(credencial, "MED-") != NULL) {
		if(buscar_indice_medico(credencial) != -1) {
			printf("\n");
			leer_contrasena_oculta(pass_input, 30, " [>] Contraseña Medica: ");
			
			if(verificar_contrasena_medico(credencial, pass_input)) {
				printf("\n\n [i]  Bienvenido Doctor(a).\n");
				pausa_sistema();
				menu_medico(credencial);
			} else {
				printf("\n [!] — Contraseña incorrecta.\n");
				intentos++;
				if (intentos >= MAX_INTENTOS) {
					printf("   [!]  Maximo de intentos excedido. Acceso bloqueado.\n");
				}
				pausa_sistema();
			}
		} else {
			printf("\n [!] Codigo medico '%s' no encontrado en el sistema.\n", credencial);
			pausa_sistema();
		}
		return;
	}
	
	
	int idx = buscar_indice_paciente(credencial);
	if(idx != -1) {
		printf(" [i] Usuario encontrado: %s\n", db_pacientes[idx].nombre_completo);
		printf(" [i] Edad: %d años\n\n", db_pacientes[idx].edad);
		
		while (intentos < MAX_INTENTOS) {
			leer_contrasena_oculta(pass_input, 30, " [>] Ingrese su Contraseña: ");
			
			if(verificar_contrasena_paciente(idx, pass_input)) {
				printf("\n\n [!] Acceso concedido.\n");
				printf("  [>]  Bienvenido/a %s.\n", db_pacientes[idx].nombre_completo);
				pausa_sistema();
				menu_paciente(credencial);
				return;
			} else {
				intentos++;
				if (intentos < MAX_INTENTOS) {
					printf("\n [!] Contrasena incorrecta. Intento %d/%d\n\n", 
						   intentos, MAX_INTENTOS);
				} else {
					printf("\n [!] Maximo de intentos (%d) excedido.\n", MAX_INTENTOS);
					printf("  [!]  Acceso denegado por seguridad.\n");
				}
			}
		}
		pausa_sistema();
		return;
	}
	
	printf("\n [!] Credencial '%s' no reconocida en el sistema.\n", credencial);
	printf("     Opciones:\n");
	printf("     [i] Si eres paciente: usa tu numero de cedula\n");
	printf("     [i] Si eres medico: usa tu codigo (ej. MED-001)\n");
	printf("     [i] Si eres administrador: usa 'admin' como usuario\n");
	pausa_sistema();
}


void menu_registro() {
	limpiar_pantalla();
	printf("\n*************************************************\n");
	printf("      SISTEMA DE SALUD - ACCESO SEGURO\n");
	printf("          NUEVO REGISTRO DE USUARIO\n");
	printf("*************************************************\n");
	printf("   Seleccione el tipo de cuenta:\n");
	printf("   [a] Paciente\n");
	printf("   [b] Medico / Especialista\n");
	printf("==================================================\n");
	
	char buff[10];
	leer_texto(buff, 10, " [>] Seleccione: ");
	char op = tolower(buff[0]);
	
	if (op == 'a') {
		registrar_paciente();
	} else if (op == 'b') {
		registrar_medico();
	} else {
		printf("\n [!] Opcion no valida.\n");
		pausa_sistema();
	}
}



int main() {
	setlocale(LC_ALL, "");
	
	
	printf("\n [!] Cargando datos del sistema...\n");
	cargar_medicos();
	cargar_pacientes();
	cargar_citas();
	printf(" [i] Datos cargados exitosamente.\n");
	pausa_sistema();
	
	char opcion = ' ';
	while(opcion != 'q') {
		limpiar_pantalla();
		printf("*************************************************\n");
		printf("            SISTEMA DE CITAS MEDICAS \n");
		printf("                     BIENVENIDO\n");
		printf("*************************************************\n");
		printf(" [a] Iniciar Sesion\n");
		printf(" [b] Registrarse\n");
		printf(" [q] Salir\n");
		printf("===================================================\n");
		
		char buff[10];
		leer_texto(buff, 10, " [!] Seleccione: ");
		opcion = tolower(buff[0]);
		
		switch(opcion) {
		case 'a': 
			iniciar_sesion(); 
			break;
		case 'b': 
			menu_registro(); 
			break;
		case 'q': 
			printf("\n [!] Cerrando sistema...\n");
			printf(" [>] Gracias por usar el Sistema de Salud.\n");
			break;
		default: 
			printf("\n [!] Opcion invalida. Por favor intente de nuevo.\n");
			pausa_sistema();
		}
	}
	
	printf("\n [>] Sistema cerrado correctamente.\n");
	return 0;
}
void exportar_reporte_multas() {
	FILE *f = fopen("reporte_multas.txt", "w");
	if (!f) {
		printf(" [!] Error al crear el reporte.\n");
		pausa_sistema();
		return;
	}
	
	fprintf(f, "=====================================================================\n");
	fprintf(f, "         REPORTE DE MULTAS POR INASISTENCIA A CITAS MEDICAS\n");
	fprintf(f, "=====================================================================\n\n");
	
	time_t t = time(NULL);
	struct tm *fecha_actual = localtime(&t);
	fprintf(f, "Generado: %d-%d-%d %d:%02d:%02d\n\n",
			fecha_actual->tm_mday,
			fecha_actual->tm_mon + 1,
			fecha_actual->tm_year + 1900,
			fecha_actual->tm_hour,
			fecha_actual->tm_min,
			fecha_actual->tm_sec);
	
	float total_multas = 0.0;
	int pacientes_con_multa = 0;
	
	fprintf(f, "%-12s | %-30s | %-10s | %-15s\n", "CEDULA", "NOMBRE", "EDAD", "MULTA TOTAL");
	fprintf(f, "=====================================================================\n");
	
	for (int i = 0; i < total_pacientes; i++) {
		if (db_pacientes[i].multa_pendiente > 0) {
			fprintf(f, "%-12s | %-30s | %-10d | $%-14.2f\n",
					db_pacientes[i].cedula,
					db_pacientes[i].nombre_completo,
					db_pacientes[i].edad,
					db_pacientes[i].multa_pendiente);
			total_multas += db_pacientes[i].multa_pendiente;
			pacientes_con_multa++;
		}
	}
	
	fprintf(f, "=====================================================================\n");
	fprintf(f, "Total de pacientes con multa:  %d\n", pacientes_con_multa);
	fprintf(f, "Total a cobrar:                $%.2f\n", total_multas);
	fprintf(f, "=====================================================================\n\n");
	fprintf(f, "Nota: Esta multa es por inasistencia a citas medicas programadas.\n");
	fprintf(f, "      Monto: $25.00 por cita no asistida.\n");
	
	fclose(f);
	printf("\n [OK] Archivo 'reporte_multas.txt' generado exitosamente.\n");
	pausa_sistema();
}

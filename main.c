#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "../include/pacientes.h"
#include "../include/medicos.h"
#include "../include/citas.h"
#include "../include/validaciones.h"


// ============================================================================
// FUNCIÓN AUXILIAR: Verificar contraseña admin
// ============================================================================
int verificar_password_admin(const char *password_ingresado) {
	if (strcmp(password_ingresado, "admin123") == 0) {
		return 1;
	}
	return 0;
}


// ============================================================================
// MENÚ ADMINISTRADOR
// ============================================================================
void menu_admin() {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n*************************************************\n");
		printf("          PANEL DE ADMINISTRADOR\n");
		printf("*************************************************\n");
		printf(" [a] Gestion de Personal Medico\n");
		printf(" [b] Control de Citas\n");
		printf(" --- REPORTES ---\n");
		printf(" [c] Exportar Lista de Medicos (TXT)\n");
		printf(" [d] Exportar Lista de Pacientes (TXT)\n");
		printf(" [e] Generar Reporte Estadistico General\n");
		printf(" [f] Ver Dashboard Gerencial (MIS)\n");
		printf(" [g] Generar Reporte Completo de Citas (TXT/HTML)\n");
		printf(" [x] Cerrar Sesion\n");
		printf("*************************************************\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Opcion: ");
		op = tolower(buff[0]);
		
		if(op == 'a') gestion_medicos_admin();
		else if(op == 'b') gestion_citas_admin();
		else if(op == 'c') exportar_reporte_medicos();
		else if(op == 'd') exportar_reporte_pacientes();
		else if(op == 'e') generar_reporte_general();
		else if(op == 'f') generar_reportes_mis();
		else if(op == 'g') generar_reporte_citas_completo();
	}
}


// ============================================================================
// MENÚ MÉDICO
// ============================================================================
void menu_medico(char *codigo) {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n*************************************************\n");
		printf("          PANEL MEDICO (%s)\n", codigo);
		printf("*************************************************\n");
		printf(" [a] Ver Mi Agenda del Dia\n");
		printf(" [b] Ver Historial Completo\n");
		printf(" [x] Cerrar Sesion\n");
		printf("*************************************************\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Opcion: ");
		op = tolower(buff[0]);
		
		if(op == 'a') ver_agenda_medico(codigo);
		else if(op == 'b') ver_historial_medico(codigo);
	}
}


// ============================================================================
// MENÚ PACIENTE
// ============================================================================
void menu_paciente(char *cedula) {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n*************************************************\n");
		printf("          PANEL PACIENTE (%s)\n", cedula);
		printf("*************************************************\n");
		printf(" [a] Solicitar Nueva Cita\n");
		printf(" [b] Ver Historial de Citas\n");
		printf(" [c] Ver Mis Datos Personales\n");
		printf(" [d] Modificar Perfil\n");
		printf(" [e] Eliminar Cuenta\n");
		printf(" [x] Cerrar Sesion\n");
		printf("*************************************************\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Opcion: ");
		op = tolower(buff[0]);
		
		if(op == 'a') solicitar_cita(cedula);
		else if(op == 'b') ver_mis_citas(cedula);
		else if(op == 'c') ver_perfil_paciente(cedula);
		else if(op == 'd') modificar_paciente(cedula);
		else if(op == 'e') {
			if(eliminar_paciente(cedula) == 1) return;
		}
	}
}


// ============================================================================
// LOGIN CON SEGURIDAD MEJORADA
// ============================================================================
void iniciar_sesion() {
	char credencial[30];
	char pass_input[30];
	int intentos = 0;
	const int MAX_INTENTOS = 3;
	
	printf("\n");
	printf("═══════════════════════════════════════════════════════════════\n");
	printf("                    SISTEMA DE SALUD\n");
	printf("                  INGRESO AL SISTEMA\n");
	printf("═══════════════════════════════════════════════════════════════\n\n");
	
	leer_texto(credencial, 30, " [>] Credencial (Cedula, Codigo Medico o Admin): ");
	
	// ========== VALIDACIÓN 1: Administrador ==========
	if(strcmp(credencial, "admin") == 0 || strcmp(credencial, "1234") == 0) {
		printf("\n");
		leer_contrasena_oculta(pass_input, 30, " [>] Contrasena de Administrador: ");
		
		if(verificar_password_admin(pass_input)) {
			printf("\n\n [OK] ✓ Bienvenido Administrador.\n");
			pausa_sistema();
			menu_admin();
		} else {
			printf("\n [!] ✗ Contrasena de administrador incorrecta.\n");
			printf("     Acceso denegado.\n");
			pausa_sistema();
		}
		return;
	}
	
	// ========== VALIDACIÓN 2: Médico ==========
	if(strstr(credencial, "MED-") != NULL) {
		if(buscar_indice_medico(credencial) != -1) {
			printf("\n");
			leer_contrasena_oculta(pass_input, 30, " [>] Contrasena Medica: ");
			
			if(verificar_contrasena_medico(credencial, pass_input)) {
				printf("\n\n [OK] ✓ Bienvenido Doctor(a).\n");
				pausa_sistema();
				menu_medico(credencial);
			} else {
				printf("\n [!] ✗ Contrasena incorrecta.\n");
				intentos++;
				if (intentos >= MAX_INTENTOS) {
					printf("     Maximo de intentos excedido. Acceso bloqueado.\n");
				}
				pausa_sistema();
			}
		} else {
			printf("\n [!] Codigo medico '%s' no encontrado en el sistema.\n", credencial);
			pausa_sistema();
		}
		return;
	}
	
	// ========== VALIDACIÓN 3: Paciente ==========
	int idx = buscar_indice_paciente(credencial);
	if(idx != -1) {
		printf(" [i] Usuario encontrado: %s\n", db_pacientes[idx].nombre_completo);
		printf(" [i] Edad: %d anos\n\n", db_pacientes[idx].edad);
		
		while (intentos < MAX_INTENTOS) {
			leer_contrasena_oculta(pass_input, 30, " [>] Ingrese su Contrasena: ");
			
			if(verificar_contrasena_paciente(idx, pass_input)) {
				printf("\n\n [OK] ✓ Acceso concedido.\n");
				printf("     Bienvenido/a %s.\n", db_pacientes[idx].nombre_completo);
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
					printf("     Acceso denegado por seguridad.\n");
				}
			}
		}
		pausa_sistema();
		return;
	}
	
	printf("\n [!] Credencial '%s' no reconocida en el sistema.\n", credencial);
	printf("     Opciones:\n");
	printf("     • Si eres paciente: usa tu numero de cedula\n");
	printf("     • Si eres medico: usa tu codigo (ej. MED-001)\n");
	printf("     • Si eres administrador: usa 'admin' como usuario\n");
	pausa_sistema();
}


// ============================================================================
// MENÚ DE REGISTRO
// ============================================================================
void menu_registro() {
	limpiar_pantalla();
	printf("\n*************************************************\n");
	printf("      SISTEMA DE SALUD - ACCESO SEGURO\n");
	printf("          NUEVO REGISTRO DE USUARIO\n");
	printf("*************************************************\n");
	printf("   Seleccione el tipo de cuenta:\n");
	printf("   [a] Paciente\n");
	printf("   [b] Medico / Especialista\n");
	printf("   [c] Administrativo\n");
	printf("-------------------------------------------------\n");
	
	char buff[10];
	leer_texto(buff, 10, " [>] Seleccione: ");
	char op = tolower(buff[0]);
	
	if (op == 'a') {
		registrar_paciente();
	} else if (op == 'b') {
		registrar_medico();
	} else if (op == 'c') {
		printf("\n [!] El registro administrativo requiere autorizacion de TI.\n");
		printf("     Contacte al departamento de sistemas.\n");
		pausa_sistema();
	} else {
		printf("\n [!] Opcion no valida.\n");
		pausa_sistema();
	}
}


// ============================================================================
// FUNCIÓN PRINCIPAL
// ============================================================================
int main() {
	setlocale(LC_ALL, "");
	
	// Cargar datos al inicio
	printf("\n [i] Cargando datos del sistema...\n");
	cargar_medicos();
	cargar_pacientes();
	cargar_citas();
	printf(" [OK] Datos cargados exitosamente.\n");
	pausa_sistema();
	
	char opcion = ' ';
	while(opcion != 'q') {
		limpiar_pantalla();
		printf("*************************************************\n");
		printf(" SISTEMA DE SALUD - ACCESO SEGURO\n");
		printf(" BIENVENIDO\n");
		printf("*************************************************\n");
		printf(" [a] Iniciar Sesion\n");
		printf(" [b] Registrarse\n");
		printf(" [q] Salir\n");
		printf("*************************************************\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Seleccione: ");
		opcion = tolower(buff[0]);
		
		switch(opcion) {
		case 'a': 
			iniciar_sesion(); 
			break;
		case 'b': 
			menu_registro(); 
			break;
		case 'q': 
			printf("\n [i] Cerrando sistema...\n");
			printf(" [i] Gracias por usar el Sistema de Salud.\n");
			break;
		default: 
			printf("\n [!] Opcion invalida. Por favor intente de nuevo.\n");
			pausa_sistema();
		}
	}
	
	printf("\n [OK] Sistema cerrado correctamente.\n");
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pacientes.h"
#include "../include/validaciones.h"

extern void cancelar_citas_usuario(const char *cedula);

Paciente db_pacientes[MAX_PACIENTES];
int total_pacientes = 0;



void cargar_pacientes() {
	FILE *f = fopen("data_pacientes.txt", "r");
	if (!f) {
		printf(" [!] Archivo de pacientes no encontrado. Iniciando con BD vacía.\n");
		total_pacientes = 0;
		return;
	}
	
	total_pacientes = 0;
	char linea[250];
	int linea_num = 0;
	
	while (fgets(linea, sizeof(linea), f) && total_pacientes < MAX_PACIENTES) {
		linea_num++;
		linea[strcspn(linea, "\n")] = 0;  
		
	
		if (strlen(linea) == 0) continue;
		
		char *token = strtok(linea, "|");
		if(token) {
			
			strncpy(db_pacientes[total_pacientes].cedula, token, 
					sizeof(db_pacientes[total_pacientes].cedula)-1);
			db_pacientes[total_pacientes].cedula[sizeof(db_pacientes[total_pacientes].cedula)-1] = '\0';
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_pacientes[total_pacientes].nombre_completo, token,
						sizeof(db_pacientes[total_pacientes].nombre_completo)-1);
				db_pacientes[total_pacientes].nombre_completo[sizeof(db_pacientes[total_pacientes].nombre_completo)-1] = '\0';
			}
			
			token = strtok(NULL, "|");
			if(token) {
				db_pacientes[total_pacientes].edad = atoi(token);
			}
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_pacientes[total_pacientes].telefono, token,
						sizeof(db_pacientes[total_pacientes].telefono)-1);
				db_pacientes[total_pacientes].telefono[sizeof(db_pacientes[total_pacientes].telefono)-1] = '\0';
			}
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_pacientes[total_pacientes].contrasena, token,
						sizeof(db_pacientes[total_pacientes].contrasena)-1);
				db_pacientes[total_pacientes].contrasena[sizeof(db_pacientes[total_pacientes].contrasena)-1] = '\0';
			} else {
				strcpy(db_pacientes[total_pacientes].contrasena, "1234");
			}
			
			total_pacientes++;
		}
	}
	fclose(f);
	printf(" [OK] Se cargaron %d pacientes del archivo.\n", total_pacientes);
}



void registrar_paciente() {
	if(total_pacientes >= MAX_PACIENTES) {
		printf(" [!] Base de datos llena. No se pueden registrar más pacientes.\n");
		pausa_sistema();
		return;
	}
	
	Paciente nuevo;
	printf("\n --- REGISTRO DE NUEVO PACIENTE ---\n");
	

	int cedula_valida = 0;
	do {
		leer_texto(nuevo.cedula, 20, " Ingrese su Numero de Cedula: ");
		
		if(buscar_indice_paciente(nuevo.cedula) != -1) {
			printf(" [!] Error: Esta cedula ya esta registrada.\n");
		} else if(validar_cedula_ecuatoriana(nuevo.cedula) == 0) {
			printf(" [!] Error: Cedula invalida (debe ser 10 dígitos).\n");
		} else {
			cedula_valida = 1;
		}
	} while(!cedula_valida);
	
	
	leer_texto(nuevo.nombre_completo, 60, " Nombre Completo: ");
	
	
	do {
		nuevo.edad = leer_entero(" Edad: ");
		if(nuevo.edad < 5) {
			printf(" [!] Error: Edad minima recomendada es 5 anos.\n");
		} else if(nuevo.edad > 120) {
			printf(" [!] Error: Edad maxima es 120 anos.\n");
		}
	} while(nuevo.edad < 5 || nuevo.edad > 120);
	

	int tel_valido = 0;
	do {
		leer_texto(nuevo.telefono, 15, " Telefono de contacto (09XXXXXXXXX): ");
		if(validar_telefono_movil(nuevo.telefono)) {
			tel_valido = 1;
		} else {
			printf(" [!] Error: Telefono invalido.\n");
			printf("     Formato: 09 seguido de 8 dígitos (ej. 0987654321)\n");
		}
	} while(!tel_valido);
	
	
	printf("\n [ SEGURIDAD ]\n");
	int pass_valida = 0;
	do {
		leer_contrasena_oculta(nuevo.contrasena, 20, " Cree su Contrasena (minimo 6 caracteres): ");
		
		if (strlen(nuevo.contrasena) < 6) {
			printf(" [!] Contrasena muy corta (minimo 6 caracteres).\n");
		} else if (strlen(nuevo.contrasena) > 20) {
			printf(" [!] Contrasena muy larga (maximo 20 caracteres).\n");
		} else {
			pass_valida = 1;
		}
	} while (!pass_valida);
	
	
	char confirma_pass[20];
	leer_contrasena_oculta(confirma_pass, 20, " Confirme su Contrasena: ");
	
	if (strcmp(nuevo.contrasena, confirma_pass) != 0) {
		printf("\n [!] Las contrasenas no coinciden. Registro cancelado.\n");
		pausa_sistema();
		return;
	}
	

	db_pacientes[total_pacientes++] = nuevo;
	guardar_pacientes();
	
	printf("\n [OK] Paciente registrado exitosamente.\n");
	printf("     Cedula: %s\n", nuevo.cedula);
	printf("     Nombre: %s\n", nuevo.nombre_completo);
	printf("     Ya puede iniciar sesion.\n");
	pausa_sistema();
}



int eliminar_paciente(const char *cedula) {
	int idx = buscar_indice_paciente(cedula);
	if (idx == -1) return 0;
	
	limpiar_pantalla();
	printf("\n +----------------------------------------------------------------+\n");
	printf(" ¦                 ADVERTENCIA: ELIMINAR CUENTA                  ¦\n");
	printf(" ¦                                                                ¦\n");
	printf(" ¦ Esta operacion es IRREVERSIBLE.                               ¦\n");
	printf(" ¦                                                                ¦\n");
	printf(" ¦ Se eliminará:                                                 ¦\n");
	printf(" ¦ • Su perfil de usuario                                        ¦\n");
	printf(" ¦ • Todas sus citas programadas (se cancelarán)                 ¦\n");
	printf(" ¦ • Su historial médico                                         ¦\n");
	printf(" ¦ • Todos sus datos personales                                  ¦\n");
	printf(" ¦                                                                ¦\n");
	printf(" +----------------------------------------------------------------+\n\n");
	
	
	printf(" Paciente a eliminar:\n");
	printf(" • Cédula: %s\n", db_pacientes[idx].cedula);
	printf(" • Nombre: %s\n", db_pacientes[idx].nombre_completo);
	printf(" • Edad: %d años\n\n", db_pacientes[idx].edad);
	

	char cedula_confirm[20];
	leer_texto(cedula_confirm, 20, " Escriba su numero de cedula para confirmar: ");
	
	if (strcmp(cedula_confirm, cedula) != 0) {
		printf("\n [!] Cedula no coincide. Operacion cancelada.\n");
		pausa_sistema();
		return 0;
	}
	

	char pass_confirm[30];
	printf("\n");
	leer_contrasena_oculta(pass_confirm, 30, " Ingrese su contrasena para verificar: ");
	
	if (!verificar_contrasena_paciente(idx, pass_confirm)) {
		printf("\n [!] Contrasena incorrecta. Operacion cancelada.\n");
		pausa_sistema();
		return 0;
	}
	
	
	printf("\n");
	char respuesta[10];
	leer_texto(respuesta, 10, " Escriba 'ELIMINAR' (en mayusculas) para confirmar definitivamente: ");
	
	if (strcmp(respuesta, "ELIMINAR") == 0) {
	
		cancelar_citas_usuario(cedula);
		
		
		for (int i = idx; i < total_pacientes - 1; i++) {
			db_pacientes[i] = db_pacientes[i + 1];
		}
		total_pacientes--;
		

		guardar_pacientes();
		
		printf("\n");
		printf(" +----------------------------------------------------------------+\n");
		printf(" ¦              CUENTA ELIMINADA CORRECTAMENTE                    ¦\n");
		printf(" ¦                                                                ¦\n");
		printf(" ¦ Su cuenta ha sido eliminada permanentemente.                   ¦\n");
		printf(" ¦ No podrá volver a acceder al sistema con esta cédula.          ¦\n");
		printf(" ¦                                                                ¦\n");
		printf(" ¦ Si desea volver a usar el sistema, deberá registrarse de nuevo.¦\n");
		printf(" ¦                                                                ¦\n");
		printf(" ¦ Hasta luego.                                                   ¦\n");
		printf(" +----------------------------------------------------------------+\n\n");
		
		pausa_sistema();
		return 1;
	} else {
		printf("\n [i] Confirmacion rechazada. Cuenta NO fue eliminada.\n");
		pausa_sistema();
		return 0;
	}
}



void modificar_paciente(const char *cedula) {
	int idx = buscar_indice_paciente(cedula);
	if (idx == -1) return;
	

	char pass_verify[30];
	printf("\n");
	leer_contrasena_oculta(pass_verify, 30, " Ingrese su contrasena para modificar datos: ");
	
	if (!verificar_contrasena_paciente(idx, pass_verify)) {
		printf("\n [!] Contrasena incorrecta. Operacion cancelada.\n");
		pausa_sistema();
		return;
	}
	
	limpiar_pantalla();
	printf("\n =========================================\n");
	printf("        MODIFICAR DATOS PERSONALES\n");
	printf(" =========================================\n");
	printf("  [1] Nombre Completo\n");
	printf("  [2] Edad\n");
	printf("  [3] Telefono\n");
	printf("  [4] Cambiar Contrasena\n");
	printf("  [x] Cancelar\n");
	printf(" -----------------------------------------\n");
	
	char buff[10];
	leer_texto(buff, 10, " [>] Seleccione opcion: ");
	char op = buff[0];
	
	if (op == '1') {
		leer_texto(db_pacientes[idx].nombre_completo, 60, " Nuevo Nombre: ");
	} else if (op == '2') {
		int nueva_edad;
		do {
			nueva_edad = leer_entero(" Nueva Edad: ");
			if (nueva_edad < 5 || nueva_edad > 120) {
				printf(" [!] Edad debe estar entre 5 y 120 anos.\n");
			}
		} while (nueva_edad < 5 || nueva_edad > 120);
		db_pacientes[idx].edad = nueva_edad;
	} else if (op == '3') {
		char nuevo_tel[15];
		int valido = 0;
		do {
			leer_texto(nuevo_tel, 15, " Nuevo Telefono: ");
			if (validar_telefono_movil(nuevo_tel)) {
				valido = 1;
			} else {
				printf(" [!] Telefono invalido.\n");
			}
		} while (!valido);
		strncpy(db_pacientes[idx].telefono, nuevo_tel, sizeof(db_pacientes[idx].telefono)-1);
		db_pacientes[idx].telefono[sizeof(db_pacientes[idx].telefono)-1] = '\0';
	} else if (op == '4') {
		printf("\n");
		leer_contrasena_oculta(db_pacientes[idx].contrasena, 20, " Nueva Contrasena: ");
	} else {
		printf("\n [i] Operacion cancelada.\n");
		pausa_sistema();
		return;
	}
	
	guardar_pacientes();
	printf("\n [OK] Datos actualizados correctamente.\n");
	pausa_sistema();
}


void ver_perfil_paciente(const char *mi_cedula) {
	int idx = buscar_indice_paciente(mi_cedula);
	if(idx == -1) return;
	
	limpiar_pantalla();
	printf("\n+-------------------------------------------------------------+\n");
	printf("¦              MIS DATOS PERSONALES                          ¦\n");
	printf("¦-------------------------------------------------------------¦\n");
	printf("¦                                                             ¦\n");
	printf("¦  Nombre:    %-50s¦\n", db_pacientes[idx].nombre_completo);
	printf("¦  Cedula:    %-50s¦\n", db_pacientes[idx].cedula);
	printf("¦  Edad:      %-50d anos¦\n", db_pacientes[idx].edad);
	printf("¦  Telefono:  %-50s¦\n", db_pacientes[idx].telefono);
	printf("¦                                                             ¦\n");
	printf("+-------------------------------------------------------------+\n");
	pausa_sistema();
}

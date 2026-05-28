
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/medicos.h"
#include "../include/validaciones.h"

extern void cancelar_citas_medico(const char *codigo_medico);

Medico db_medicos[MAX_MEDICOS];
int total_medicos = 0;



void cargar_medicos() {
	FILE *f = fopen("data_medicos.txt", "r");
	
	if (!f) {
		printf(" [!] Archivo de medicos no encontrado.\n");
		total_medicos = 0;
		return;
	}
	
	total_medicos = 0;
	char linea[300]; 
	int linea_num = 0;
	
	while (fgets(linea, sizeof(linea), f) && total_medicos < MAX_MEDICOS) {
		linea_num++;
		linea[strcspn(linea, "\n")] = 0;
		
		
		if (strlen(linea) == 0) continue;
		
		char *token = strtok(linea, "|");
		if(token) {
			
			strncpy(db_medicos[total_medicos].codigo, token, 
					sizeof(db_medicos[total_medicos].codigo)-1);
			db_medicos[total_medicos].codigo[sizeof(db_medicos[total_medicos].codigo)-1] = '\0';
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_medicos[total_medicos].nombre, token,
						sizeof(db_medicos[total_medicos].nombre)-1);
				db_medicos[total_medicos].nombre[sizeof(db_medicos[total_medicos].nombre)-1] = '\0';
			}
			
			token = strtok(NULL, "|");
			if(token) {
				db_medicos[total_medicos].edad = atoi(token);
			} else {
				db_medicos[total_medicos].edad = 35;  
			}
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_medicos[total_medicos].especialidad, token,
						sizeof(db_medicos[total_medicos].especialidad)-1);
				db_medicos[total_medicos].especialidad[sizeof(db_medicos[total_medicos].especialidad)-1] = '\0';
			}
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_medicos[total_medicos].horario, token,
						sizeof(db_medicos[total_medicos].horario)-1);
				db_medicos[total_medicos].horario[sizeof(db_medicos[total_medicos].horario)-1] = '\0';
			}
			
			token = strtok(NULL, "|");
			if(token) {
				strncpy(db_medicos[total_medicos].contrasena, token,
						sizeof(db_medicos[total_medicos].contrasena)-1);
				db_medicos[total_medicos].contrasena[sizeof(db_medicos[total_medicos].contrasena)-1] = '\0';
			} else {
				strcpy(db_medicos[total_medicos].contrasena, "1234");
			}
			
			total_medicos++;
		}
	}
	fclose(f);
	printf(" [OK] Se cargaron %d medicos del archivo.\n", total_medicos);
}



void registrar_medico() {
	if(total_medicos >= MAX_MEDICOS) {
		printf(" [!] Base de datos de medicos llena.\n");
		pausa_sistema();
		return;
	}
	
	Medico nuevo;
	printf("\n --- ALTA DE PERSONAL MEDICO ---\n");
	
	
	int siguiente_num = total_medicos + 1;
	sprintf(nuevo.codigo, "MED-%03d", siguiente_num);
	printf(" > ID Asignado Automaticamente: %s\n", nuevo.codigo);
	
	
	fflush(stdin);
	leer_texto(nuevo.nombre, 60, " Nombre del Doctor(a): ");
	
	
	do {
		nuevo.edad = leer_entero(" Edad (entre 25 y 85 años): ");
		if(nuevo.edad < 25) {
			printf(" [!] Edad minima para ejercer: 25 años (profesional graduado).\n");
		} else if(nuevo.edad > 85) {
			printf(" [!] Edad maxima recomendada: 85 años.\n");
		}
	} while(nuevo.edad < 25 || nuevo.edad > 85);
	

	printf("\n Especialidades disponibles:\n");
	printf(" [>] Medicina General       [>] Cardiologia\n");
	printf(" [>] Pediatria              [>] Ginecologia\n");
	printf(" [>] Dermatologia           [>] Traumatologia\n");
	printf(" [>] Neurologia             [>] (Otra)\n\n");
	leer_texto(nuevo.especialidad, 50, " Especialidad: ");
	
	
	printf("\n Horarios disponibles:\n");
	printf(" [>] 08:00-14:00 (Mañana)\n");
	printf(" [>] 14:00-20:00 (Tarde)\n");
	printf(" [>] 08:00-20:00 (Jornada completa)\n\n");
	leer_texto(nuevo.horario, 20, " Horario asignado (ej. 08:00-14:00): ");
	

	printf("\n [ SEGURIDAD - CONTRASEÑA MÉDICA ]\n");
	int pass_valida = 0;
	do {
		leer_contrasena_oculta(nuevo.contrasena, 20, "[i] Asignar Contraseña (minimo 6 caracteres): ");
		
		if (strlen(nuevo.contrasena) < 6) {
			printf(" [!] Contraseña muy corta (minimo 6 caracteres).\n");
		} else if (strlen(nuevo.contrasena) > 20) {
			printf(" [!] Contraseña muy larga (maximo 20 caracteres).\n");
		} else {
			pass_valida = 1;
		}
	} while (!pass_valida);
	
	
	char confirma_pass[20];
	leer_contrasena_oculta(confirma_pass, 20, "[i] Confirme la Contraseña: ");
	
	if (strcmp(nuevo.contrasena, confirma_pass) != 0) {
		printf("\n [!] Las contraseñas no coinciden. Registro cancelado.\n");
		pausa_sistema();
		return;
	}
	
	
	db_medicos[total_medicos++] = nuevo;
	guardar_medicos();
	
	printf("\n +----------------------------------------------------------------+\n");
	printf(" ¦            MEDICO REGISTRADO EXITOSAMENTE                    ¦\n");
	printf(" ¦----------------------------------------------------------------¦\n");
	printf(" ¦ Código:         %-48s¦\n", nuevo.codigo);
	printf(" ¦ Nombre:         %-48s¦\n", nuevo.nombre);
	printf(" ¦ Especialidad:   %-48s¦\n", nuevo.especialidad);
	printf(" ¦ Horario:        %-48s¦\n", nuevo.horario);
	printf(" ¦                                                                ¦\n");
	printf(" ¦ Ya puede usar el sistema con su código de médico.             ¦\n");
	printf(" +----------------------------------------------------------------+\n\n");
	pausa_sistema();
}



void listar_medicos() {
	if (total_medicos == 0) {
		printf("\n [i] No hay medicos registrados en el sistema.\n");
		pausa_sistema(); 
		return;
	}
	
	printf("\n+--------------------------------------------------------------------+\n");
	printf("¦                    PLANTILLA DE MEDICOS                           ¦\n");
	printf("¦--------------------------------------------------------------------¦\n");
	printf("¦ CODIGO  ¦ NOMBRE                          ¦ ESPECIALIDAD        ¦\n");
	printf("¦--------------------------------------------------------------------¦\n");
	
	for(int i=0; i<total_medicos; i++) {
		printf("¦ %-7s ¦ %-31s ¦ %-19s¦\n", 
			   db_medicos[i].codigo, 
			   db_medicos[i].nombre,
			   db_medicos[i].especialidad);
	}
	printf("+--------------------------------------------------------------------+\n");
	pausa_sistema(); 
}



int eliminar_medico(const char *codigo) {
	int idx = buscar_indice_medico(codigo);
	if (idx == -1) {
		printf("\n [!] Codigo de medico no encontrado.\n");
		pausa_sistema();
		return 0;
	}
	
	printf("\n [!] ATENCION: Esta a punto de eliminar al %s\n", db_medicos[idx].nombre);
	char confirma[10];
	leer_texto(confirma, 10, " [>] Escriba 'ELIMINAR' para confirmar: ");
	
	if (strcmp(confirma, "ELIMINAR") == 0) {
		cancelar_citas_medico(codigo); // Cancela sus citas pendientes
		
		for (int i = idx; i < total_medicos - 1; i++) {
			db_medicos[i] = db_medicos[i + 1];
		}
		total_medicos--;
		guardar_medicos();
		
		printf("\n [i] Medico eliminado y sus citas han sido canceladas.\n");
	} else {
		printf("\n [i] Operacion cancelada.\n");
	}
	pausa_sistema();
	return 1;
}


void gestion_medicos_admin() {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n [ GESTION DE PERSONAL MEDICO ]\n");
		printf("   [a] Registrar nuevo medico\n");
		printf("   [b] Ver plantilla completa\n");
		printf("   [c] Eliminar medico\n");
		printf("   [x] Volver\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Seleccione: ");
		op = tolower(buff[0]);
		
		if (op == 'a') registrar_medico();
		else if (op == 'b') listar_medicos();
		else if (op == 'c') {
			char codigo[10];
			leer_texto(codigo, 10, "\n [>] Ingrese el codigo del medico a eliminar: ");
			eliminar_medico(codigo);
		}
	}
}



void exportar_reporte_medicos() {
	FILE *f = fopen("reporte_staff.txt", "w");
	if (!f) {
		printf(" [!] Error al crear el reporte.\n");
		return;
	}
	
	fprintf(f, "--------------------------------------------------------------------\n");
	fprintf(f, "                 NOMINA DE PERSONAL MEDICO\n");
	fprintf(f, "--------------------------------------------------------------------\n\n");
	
	
	int edad_promedio = 0;
	for (int i = 0; i < total_medicos; i++) {
		edad_promedio += db_medicos[i].edad;
	}
	if (total_medicos > 0) edad_promedio /= total_medicos;
	
	fprintf(f, "ESTADÍSTICAS:\n");
	fprintf(f, "  [i] Total de Medicos:        %d\n", total_medicos);
	fprintf(f, "  [i] Edad Promedio:           %d años\n", edad_promedio);
	fprintf(f, "\n");
	
	
	fprintf(f, "--------------------------------------------------------------------\n");
	fprintf(f, "%-10s ¦ %-25s ¦ %-15s ¦ %-10s\n", "CODIGO", "NOMBRE", "ESPECIALIDAD", "HORARIO");
	fprintf(f, "--------------------------------------------------------------------\n");
	
	for(int i=0; i<total_medicos; i++) {
		fprintf(f, "%-10s ¦ %-25s ¦ %-15s ¦ %-10s\n", 
				db_medicos[i].codigo, 
				db_medicos[i].nombre, 
				db_medicos[i].especialidad,
				db_medicos[i].horario);
	}
	
	fprintf(f, "--------------------------------------------------------------------\n\n");
	
	
	fprintf(f, "DISTRIBUCION POR ESPECIALIDAD:\n");
	fprintf(f, "--------------------------------------------------------------------\n");
	
	
	typedef struct {
		char especialidad[50];
		int cantidad;
	} ConteoEspecialidad;
	
	ConteoEspecialidad agrupado[20]; 
	int total_especialidades_unicas = 0;
	
	for (int i = 0; i < total_medicos; i++) {
		int encontrada = 0;
		
		for (int j = 0; j < total_especialidades_unicas; j++) {
			if (strcmp(db_medicos[i].especialidad, agrupado[j].especialidad) == 0) {
				agrupado[j].cantidad++; 
				encontrada = 1;
				break;
			}
		}
		
	
		if (!encontrada && total_especialidades_unicas < 20) {
			strcpy(agrupado[total_especialidades_unicas].especialidad, db_medicos[i].especialidad);
			agrupado[total_especialidades_unicas].cantidad = 1;
			total_especialidades_unicas++;
		}
	}
	
	
	for (int i = 0; i < total_especialidades_unicas; i++) {
		fprintf(f, " %-20s : %d medicos\n", agrupado[i].especialidad, agrupado[i].cantidad);
	}
	
	
	fprintf(f, "--------------------------------------------------------------------\n");
	fprintf(f, "Reporte generado automaticamente.\n");
	
	fclose(f);
	printf("\n [OK] Archivo 'reporte_staff.txt' generado exitosamente.\n");
	pausa_sistema();
}


void ver_perfil_medico(const char *codigo) {
	int idx = buscar_indice_medico(codigo); 
	if(idx == -1) {
		printf(" [!] Medico no encontrado.\n");
		return;
	}
	
	printf("\n +----------------------------------------------------------------+\n");
	printf(" ¦                    FICHA DE PERSONAL MEDICO                   ¦\n");
	printf(" ¦----------------------------------------------------------------¦\n");
	printf(" ¦                                                                ¦\n");
	printf(" ¦  Codigo:          %-50s¦\n", db_medicos[idx].codigo);
	printf(" ¦  Nombre:          %-50s¦\n", db_medicos[idx].nombre);
	printf(" ¦  Edad:            %-50d anos¦\n", db_medicos[idx].edad);
	printf(" ¦  Especialidad:    %-50s¦\n", db_medicos[idx].especialidad);
	printf(" ¦  Horario:         %-50s¦\n", db_medicos[idx].horario);
	printf(" ¦                                                                ¦\n");
	printf(" +----------------------------------------------------------------+\n");
	pausa_sistema();
}
int buscar_indice_medico(const char *codigo) {
	for(int i = 0; i < total_medicos; i++) {
		if(strcmp(db_medicos[i].codigo, codigo) == 0) return i;
	}
	return -1; // No encontrado
}

void guardar_medicos() {
	FILE *f = fopen("data_medicos.txt", "w");
	if (!f) return;
	
	for(int i = 0; i < total_medicos; i++) {
		fprintf(f, "%s|%s|%d|%s|%s|%s\n", 
				db_medicos[i].codigo, 
				db_medicos[i].nombre, 
				db_medicos[i].edad, 
				db_medicos[i].especialidad, 
				db_medicos[i].horario, 
				db_medicos[i].contrasena);
	}
	fclose(f);
}

int verificar_contrasena_medico(const char *codigo, const char *pass_intento) {
	int idx = buscar_indice_medico(codigo);
	if (idx == -1) return 0;
	
	if (strcmp(db_medicos[idx].contrasena, pass_intento) == 0) return 1;
	return 0;
}

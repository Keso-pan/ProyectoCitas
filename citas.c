#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "../include/citas.h"
#include "../include/validaciones.h"
#include "../include/medicos.h"
#include "../include/pacientes.h"

Cita db_citas[MAX_CITAS];
int total_citas = 0;


void cargar_citas() {
	FILE *f = fopen("data_citas.txt", "r");
	if (!f) { 
		total_citas = 0; 
		return; 
	}
	
	total_citas = 0;
	char linea[256];
	
	while (fgets(linea, sizeof(linea), f) && total_citas < MAX_CITAS) {
		linea[strcspn(linea, "\n")] = 0; 
		
		char *token = strtok(linea, "|");
		if(token) {
			db_citas[total_citas].id = atoi(token);
			token = strtok(NULL, "|"); 
			if(token) strncpy(db_citas[total_citas].id_paciente, token, sizeof(db_citas[total_citas].id_paciente)-1);
			token = strtok(NULL, "|"); 
			if(token) strncpy(db_citas[total_citas].codigo_medico, token, sizeof(db_citas[total_citas].codigo_medico)-1);
			token = strtok(NULL, "|"); 
			if(token) strncpy(db_citas[total_citas].fecha, token, sizeof(db_citas[total_citas].fecha)-1);
			token = strtok(NULL, "|"); 
			if(token) strncpy(db_citas[total_citas].motivo, token, sizeof(db_citas[total_citas].motivo)-1);
			token = strtok(NULL, "|"); 
			if(token) db_citas[total_citas].estado = atoi(token);
			total_citas++;
		}
	}
	fclose(f);
	printf(" [i] Se cargaron %d citas del archivo.\n", total_citas);
}


void guardar_citas() {
	FILE *f = fopen("data_citas.txt", "w");
	if (!f) return;
	
	for(int i=0; i<total_citas; i++) {
		fprintf(f, "%d|%s|%s|%s|%s|%d\n", 
				db_citas[i].id, 
				db_citas[i].id_paciente, 
				db_citas[i].codigo_medico,
				db_citas[i].fecha, 
				db_citas[i].motivo, 
				db_citas[i].estado);
	}
	fclose(f);
}


void solicitar_cita(const char *mi_cedula) {
	if(total_citas >= MAX_CITAS) {
		printf(" [!] Sistema de citas lleno.\n"); 
		pausa_sistema();
		return;
	}
	
	Cita nueva;
	nueva.id = total_citas + 1; 
	strncpy(nueva.id_paciente, mi_cedula, sizeof(nueva.id_paciente)-1);
	nueva.id_paciente[sizeof(nueva.id_paciente)-1] = '\0';
	
	printf("\n --- NUEVA SOLICITUD DE CITA ---\n");
	listar_medicos(); 
	
	int medico_valido = 0;
	do {
		leer_texto(nueva.codigo_medico, 10, " Ingrese Codigo del Medico (ej. MED-001): ");
		if(buscar_indice_medico(nueva.codigo_medico) != -1) {
			medico_valido = 1;
		} else {
			printf(" [!] Medico no encontrado. Intente de nuevo.\n");
		}
	} while(!medico_valido);
	
	
	int fecha_valida = 0;
	do {
		leer_texto(nueva.fecha, 20, " Fecha deseada (DD-MM-AAAA): ");
		
		
		if (!validar_fecha_futura(nueva.fecha)) {
			printf(" [!] Error: La fecha debe ser futura y en formato DD-MM-AAAA.\n");
			printf(" [>]    Ejemplos validos: 15-06-2026, 20-12-2027\n");
			printf(" [!]   NO se puede: Fechas pasadas, hoy, o mas de 2 años adelante.\n");
		} else {
			fecha_valida = 1;
		}
	} while (!fecha_valida);
	
	
	printf("\n Motivos comunes:\n");
	printf(" [>] Chequeo de rutina  [>] Fiebre aguda  [>] Dolor articular\n");
	printf(" [>] Control anual      [>] Sintomas virales  [>] Dolor de cabeza\n");
	printf(" [>] Revision general   [>] Otro\n\n");
	leer_texto(nueva.motivo, 100, " Motivo de consulta: ");
	
	nueva.estado = 0;
	
	db_citas[total_citas++] = nueva;
	guardar_citas();
	
	printf("\n [!] Cita #%d agendada correctamente para el %s.\n", nueva.id, nueva.fecha);
	pausa_sistema();
}


void ver_mis_citas(const char *mi_cedula) {
	printf("\n --- MIS CITAS ---\n");
	printf(" %-4s | %-12s | %-10s | %-15s\n", "ID", "FECHA", "MEDICO", "ESTADO");
	printf(" ------------------------------------------------\n");
	
	int encontradas = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].id_paciente, mi_cedula) == 0) {
			
			
			printf(" #%-3d | %-12s | %-10s | %s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].codigo_medico, 
				   obtener_estado_str(db_citas[i].estado));
			encontradas++;
		}
	}
	
	if(!encontradas) printf(" [i] No tiene citas registradas.\n");
	printf(" ------------------------------------------------\n");
	pausa_sistema();
}


void ver_agenda_medico(const char *mi_codigo) {
	printf("\n --- MI AGENDA (%s) ---\n", mi_codigo);
	printf(" %-5s | %-12s | %-12s | %-20s\n", "ID", "FECHA", "PACIENTE", "MOTIVO");
	printf(" ---------------------------------------------------------\n");
	
	int hay_citas = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, mi_codigo) == 0 && db_citas[i].estado == 0) {
			printf(" %-5d | %-12s | %-12s | %-20s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].id_paciente, 
				   db_citas[i].motivo);
			hay_citas = 1;
		}
	}
	
	if(!hay_citas) printf(" [i] No tiene citas activas programadas.\n");
	printf(" ---------------------------------------------------------\n");
	pausa_sistema();
}


void ver_historial_medico(const char *mi_codigo) {
	printf("\n =========================================================\n");
	printf("        HISTORIAL DE PACIENTES (%s)\n", mi_codigo);
	printf(" =========================================================\n");
	printf(" %-5s | %-12s | %-12s | %-15s\n", "ID", "FECHA", "PACIENTE", "ESTADO");
	printf(" ---------------------------------------------------------\n");
	
	int encontrados = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, mi_codigo) == 0) {
			
			
			printf(" %-5d | %-12s | %-12s | %-15s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].id_paciente, 
				   obtener_estado_str(db_citas[i].estado));
			encontrados++;
		}
	}
	if(!encontrados) printf(" [i] No tiene historial registrado.\n");
	else printf("\n [i] Total registros encontrados: %d\n", encontrados);
	printf(" =========================================================\n");
	pausa_sistema();
}

void generar_reporte_general() {
	FILE *f = fopen("reporte_gerencial.txt", "w");
	if (!f) {
		printf(" [!] Error al crear el reporte.\n");
		return;
	}
	
	int activas = 0;
	int canceladas = 0;
	
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].estado == 0) activas++;
		else canceladas++;
	}
	
	fprintf(f, "============================================\n");
	fprintf(f, "       REPORTE ESTADISTICO DEL HOSPITAL\n");
	fprintf(f, "============================================\n");
	fprintf(f, " Total de Citas Registradas:  %d\n", total_citas);
	fprintf(f, " --------------------------------------------\n");
	fprintf(f, " [v] Citas Activas (Pendientes): %d\n", activas);
	fprintf(f, " [x] Citas Canceladas:           %d\n", canceladas);
	fprintf(f, "============================================\n");
	
	fclose(f);
	printf("\n [i] Reporte 'reporte_gerencial.txt' generado exitosamente.\n");
	pausa_sistema();
}


void cancelar_citas_usuario(const char *cedula) {
	int cambios = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].id_paciente, cedula) == 0 && db_citas[i].estado == 0) {
			db_citas[i].estado = 1; 
			cambios = 1;
		}
	}
	if(cambios) {
		guardar_citas();
	}
}


void generar_reporte_citas_completo() {
	if (total_citas == 0) {
		printf("\n [!] No hay citas registradas para reportar.\n");
		pausa_sistema();
		return;
	}
	
	
	FILE *f_txt = fopen("reporte_citas_completo.txt", "w");
	if (!f_txt) {
		printf("\n [!] Error al crear reporte.\n");
		pausa_sistema();
		return;
	}
	
	
	fprintf(f_txt, "===================================================================\n");
	fprintf(f_txt, "              REPORTE COMPLETO DE CITAS MEDICAS\n");
	fprintf(f_txt, "===================================================================\n\n");
	
	
	time_t t = time(NULL);
	struct tm *fecha_actual = localtime(&t);
	fprintf(f_txt, "Generado: %d-%d-%d %d:%02d:%02d\n\n",
			fecha_actual->tm_mday,
			fecha_actual->tm_mon + 1,
			fecha_actual->tm_year + 1900,
			fecha_actual->tm_hour,
			fecha_actual->tm_min,
			fecha_actual->tm_sec);
	
	
	int activas = 0, canceladas = 0;
	for (int i = 0; i < total_citas; i++) {
		if (db_citas[i].estado == 0) activas++;
		else canceladas++;
	}
	
	fprintf(f_txt, "ESTADISTICAS GENERALES:\n");
	fprintf(f_txt, "****************************************************************************\n");
	fprintf(f_txt, "Total de citas:      %d\n", total_citas);
	fprintf(f_txt, "Citas activas:       %d (%.1f%%)\n", activas, (float)activas/total_citas*100);
	fprintf(f_txt, "Citas canceladas:    %d (%.1f%%)\n\n", canceladas, (float)canceladas/total_citas*100);
	
	
	fprintf(f_txt, "DETALLE DE CITAS:\n");
	fprintf(f_txt, "****************************************************************************\n");
	fprintf(f_txt, "%-4s | %-12s | %-10s | %-12s | %-30s | %s\n",
			"ID", "FECHA", "PACIENTE", "MEDICO", "MOTIVO", "ESTADO");
	fprintf(f_txt, "****************************************************************************\n");
	
	// Listar todas las citas
	for (int i = 0; i < total_citas; i++) {
		const char *estado_str = obtener_estado_str(db_citas[i].estado);
		fprintf(f_txt, "%-4d | %-12s | %-10s | %-10s | %-30s | %s\n",
				db_citas[i].id,
				db_citas[i].fecha,
				db_citas[i].id_paciente,
				db_citas[i].codigo_medico,
				db_citas[i].motivo,
				estado_str);
	}
	
	fprintf(f_txt, "****************************************************************************\n\n");
	fprintf(f_txt, "Fin del reporte.\n");
	
	fclose(f_txt);
	printf("\n [i] Reporte generado en 'reporte_citas_completo.txt'\n");
	
	
	FILE *f_html = fopen("reporte_citas_completo.html", "w");
	if (f_html) {
		fprintf(f_html, "<!DOCTYPE html>\n");
		fprintf(f_html, "<html>\n");
		fprintf(f_html, "<head>\n");
		fprintf(f_html, "  <meta charset='UTF-8'>\n");
		fprintf(f_html, "  <title>Reporte de Citas</title>\n");
		fprintf(f_html, "  <style>\n");
		fprintf(f_html, "    body { font-family: Arial, sans-serif; margin: 20px; }\n");
		fprintf(f_html, "    h1 { color: #1E5AAE; text-align: center; }\n");
		fprintf(f_html, "    table { border-collapse: collapse; width: 100%%; }\n");
		fprintf(f_html, "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n");
		fprintf(f_html, "    th { background-color: #2E75B6; color: white; }\n");
		fprintf(f_html, "    .estadistica { background-color: #f9f9f9; padding: 10px; margin: 10px 0; }\n");
		fprintf(f_html, "  </style>\n");
		fprintf(f_html, "</head>\n");
		fprintf(f_html, "<body>\n");
		fprintf(f_html, "  <h1>REPORTE COMPLETO DE CITAS MEDICAS</h1>\n");
		
		fprintf(f_html, "  <div class='estadistica'>\n");
		fprintf(f_html, "    <strong>Total de citas:</strong> %d<br>\n", total_citas);
		fprintf(f_html, "    <strong>Citas activas:</strong> %d (%.1f%%)<br>\n", activas, (float)activas/total_citas*100);
		fprintf(f_html, "    <strong>Citas canceladas:</strong> %d (%.1f%%)\n", canceladas, (float)canceladas/total_citas*100);
		fprintf(f_html, "  </div>\n");
		
		fprintf(f_html, "  <table>\n");
		fprintf(f_html, "    <tr><th>ID</th><th>Fecha</th><th>Paciente</th><th>Medico</th><th>Motivo</th><th>Estado</th></tr>\n");
		
		for (int i = 0; i < total_citas; i++) {
			const char *estado_str = obtener_estado_str(db_citas[i].estado);
			fprintf(f_html, "    <tr>\n");
			fprintf(f_html, "      <td>%d</td>\n", db_citas[i].id);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].fecha);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].id_paciente);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].codigo_medico);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].motivo);
			fprintf(f_html, "      <td>%s</td>\n", estado_str);
			fprintf(f_html, "    </tr>\n");
		}
		
		fprintf(f_html, "  </table>\n");
		fprintf(f_html, "</body>\n");
		fprintf(f_html, "</html>\n");
		
		fclose(f_html);
		printf(" [>] Archivo HTML generado: reporte_citas_completo.html\n");
		printf(" [>]    (Puedes abrirlo e imprimirlo a PDF)\n");
	}
	
	pausa_sistema();
}


void generar_reportes_mis() {
	limpiar_pantalla();
	printf("\n=========================================================\n");
	printf("         MIS - REPORTES GERENCIALES (DASHBOARD)\n");
	printf("=========================================================\n\n");
	
	
	int efectivas = 0, canceladas = 0;
	
	for(int i = 0; i < total_citas; i++) {
		if(db_citas[i].estado == 0) efectivas++;
		else canceladas++;
	}
	
	int total_evaluadas = efectivas + canceladas;
	
	printf(" [A] TASA DE EFECTIVIDAD DE CITAS ANUALES:\n");
	printf(" %-20s | %-10s | %-10s\n", "Estado de la Cita", "Cantidad", "Porcentaje");
	printf(" --------------------------------------------------\n");
	
	float pct_efectivas = (total_evaluadas > 0) ? ((float)efectivas / total_evaluadas) * 100 : 0.0;
	float pct_canceladas = (total_evaluadas > 0) ? ((float)canceladas / total_evaluadas) * 100 : 0.0;
	
	printf(" %-20s | %-10d | %.1f%%\n", "Efectiva", efectivas, pct_efectivas);
	printf(" %-20s | %-10d | %.1f%%\n", "Cancelada", canceladas, pct_canceladas);
	printf(" --------------------------------------------------\n\n");
	
	
	printf(" [B] DEMANDA POR ESPECIALIDAD:\n");
	printf(" %-25s | %-15s\n", "Especialidad Medica", "Total de Citas");
	printf(" --------------------------------------------\n");
	
	typedef struct {
		char nombre_esp[50];
		int cuenta;
	} TallyEspecialidad;
	
	TallyEspecialidad tallies[20]; 
	int num_especialidades_unicas = 0;
	
	for(int i = 0; i < total_citas; i++) {
		int idx_med = buscar_indice_medico(db_citas[i].codigo_medico);
		
		if(idx_med != -1) {
			char *esp_actual = db_medicos[idx_med].especialidad;
			int encontrada = 0;
			
			for(int j = 0; j < num_especialidades_unicas; j++) {
				if(strcmp(tallies[j].nombre_esp, esp_actual) == 0) {
					tallies[j].cuenta++;
					encontrada = 1; 
					break;
				}
			}
			if(!encontrada && num_especialidades_unicas < 20) {
				strncpy(tallies[num_especialidades_unicas].nombre_esp, esp_actual, sizeof(tallies[num_especialidades_unicas].nombre_esp)-1);
				tallies[num_especialidades_unicas].nombre_esp[sizeof(tallies[num_especialidades_unicas].nombre_esp)-1] = '\0';
				tallies[num_especialidades_unicas].cuenta = 1;
				num_especialidades_unicas++;
			}
		}
	}
	
	for(int i = 0; i < num_especialidades_unicas; i++) {
		printf(" %-25s | %-15d\n", tallies[i].nombre_esp, tallies[i].cuenta);
	}
	printf(" --------------------------------------------\n\n");
	
	
	int pediatricos = 0, adultos = 0, geriatricos = 0;
	
	for(int i = 0; i < total_citas; i++) {
		int idx_pac = buscar_indice_paciente(db_citas[i].id_paciente);
		
		if(idx_pac != -1) {
			int edad = db_pacientes[idx_pac].edad;
			
			if(edad <= 17) pediatricos++;
			else if(edad <= 64) adultos++;
			else geriatricos++;
		}
	}
	
	printf(" [C] DISTRIBUCION ETARIA DE PACIENTES ATENDIDOS:\n");
	printf(" %-25s | %-20s\n", "Grupo Etario", "Pacientes Agendados");
	printf(" -------------------------------------------------\n");
	printf(" %-25s | %-20d\n", "Pediatricos (0-17)", pediatricos);
	printf(" %-25s | %-20d\n", "Adultos (18-64)", adultos);
	printf(" %-25s | %-20d\n", "Geriatricos (65+)", geriatricos);
	printf(" -------------------------------------------------\n\n");
	
	printf("=========================================================\n");
	pausa_sistema();
}

void cancelar_citas_medico(const char *codigo_medico) {
	int cambios = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, codigo_medico) == 0 && db_citas[i].estado == 0) {
			db_citas[i].estado = 1; 
			cambios = 1;
		}
	}
	if(cambios) {
		guardar_citas();
	}
}

const char* obtener_estado_str(int estado) {
	if (estado == 0) return "ACTIVA";
	if (estado == 1) return "CANCELADA";
	if (estado == 2) return "ATENDIDA";
	if (estado == 3) return "NO ASISTIO";
	return "DESCONOCIDO";
}


void gestionar_estatus_cita(const char *codigo_medico) {
	printf("\n --- GESTIONAR ESTATUS DE CITA ---\n");
	int id = leer_entero(" [>] Ingrese el ID de la cita: ");
	
	int encontrado = -1;
	for(int i = 0; i < total_citas; i++) {
		if(db_citas[i].id == id) {
			encontrado = i;
			break;
		}
	}
	
	if(encontrado != -1) {
		// Validar que la cita le pertenezca a este doctor
		if(strcmp(db_citas[encontrado].codigo_medico, codigo_medico) != 0) {
			printf("\n [!] Permiso denegado. Esta cita esta asignada a otro medico.\n");
			pausa_sistema();
			return;
		}
		
		printf("\n --- DETALLES DE LA CITA ---\n");
		printf(" Paciente: %s\n", db_citas[encontrado].id_paciente);
		printf(" Fecha:    %s\n", db_citas[encontrado].fecha);
		printf(" Motivo:   %s\n", db_citas[encontrado].motivo);
		printf(" Estado:   %s\n", obtener_estado_str(db_citas[encontrado].estado));
		
		if (db_citas[encontrado].estado != 0) {
			printf("\n [i] Esta cita ya fue procesada anteriormente.\n");
		}
		
		printf("\n ¿Cual es el nuevo estatus de esta cita?\n");
		printf(" [1] Marcar como ATENDIDA\n");
		printf(" [2] Marcar como NO ASISTIO\n");
		printf(" [x] Cancelar operacion\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Seleccione: ");
		
		if (buff[0] == '1') {
			db_citas[encontrado].estado = 2; 
			guardar_citas();
			printf("\n [OK] Cita marcada como ATENDIDA correctamente.\n");
		} else if (buff[0] == '2') {
			db_citas[encontrado].estado = 3; 
			guardar_citas();
			printf("\n [OK] Cita marcada como NO ASISTIO correctamente.\n");
		} else {
			printf("\n [i] Operacion cancelada. No hubo cambios.\n");
		}
	} else {
		printf("\n [!] No se encontro ninguna cita con el ID #%d.\n", id);
	}
	pausa_sistema();
}

void cancelar_cita_paciente(const char *mi_cedula) {
	printf("\n --- CANCELAR UNA CITA ACTIVA ---\n");
	
	
	int activas = 0;
	printf(" %-4s | %-12s | %-10s | %-15s\n", "ID", "FECHA", "MEDICO", "ESTADO");
	printf(" ------------------------------------------------\n");
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].id_paciente, mi_cedula) == 0 && db_citas[i].estado == 0) {
			printf(" #%-3d | %-12s | %-10s | %s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].codigo_medico, 
				   obtener_estado_str(db_citas[i].estado));
			activas++;
		}
	}
	
	if (activas == 0) {
		printf("\n [i] No tiene citas activas programadas para cancelar.\n");
		pausa_sistema();
		return;
	}
	
	
	int id = leer_entero("\n [>] Ingrese el ID de la cita que desea cancelar (0 para salir): ");
	if (id <= 0) return;
	
	int encontrado = -1;
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].id == id && strcmp(db_citas[i].id_paciente, mi_cedula) == 0) {
			encontrado = i;
			break;
		}
	}
	
	
	if (encontrado != -1) {
		if (db_citas[encontrado].estado == 1) {
			printf("\n [!] Esta cita ya fue cancelada previamente.\n");
		} else if (db_citas[encontrado].estado == 2 || db_citas[encontrado].estado == 3) {
			printf("\n [!] ERROR: No puede cancelar una cita que ya fue 'Atendida' o 'No Asistio'.\n");
		} else {
			char confirma[10];
			leer_texto(confirma, 10, " [>] ¿Esta seguro? Escriba 'si' para confirmar: ");
			if (strcmp(confirma, "si") == 0) {
				db_citas[encontrado].estado = 1; // Cambiar a estado Cancelada
				guardar_citas();
				printf("\n [OK] Su cita #%d ha sido cancelada exitosamente.\n", id);
			} else {
				printf("\n [i] Operacion abortada.\n");
			}
		}
	} else {
		printf("\n [!] ID invalido o la cita no le pertenece.\n");
	}
	pausa_sistema();
}

void gestion_citas_admin() {
	printf("\n --- PANEL DE CONTROL DE CITAS (ADMINISTRADOR) ---\n");
	
	// 1. Mostrar todas las citas activas de todo el sistema
	int activas = 0;
	printf(" %-5s | %-12s | %-12s | %-10s | %-20s\n", "ID", "FECHA", "PACIENTE", "MEDICO", "MOTIVO");
	printf(" -----------------------------------------------------------------------\n");
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].estado == 0) { // Solo muestra las Activas (0)
			printf(" %-5d | %-12s | %-12s | %-10s | %-20s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].id_paciente, 
				   db_citas[i].codigo_medico, 
				   db_citas[i].motivo);
			activas++;
		}
	}
	
	if (activas == 0) {
		printf("\n [i] No hay citas activas pendientes en el sistema en este momento.\n");
		pausa_sistema();
		return;
	}
	
	printf("\n [!] Funcion administrativa: Cancelar Citas Activas\n");
	int id = leer_entero(" [>] Ingrese ID de la cita a cancelar (0 para volver): ");
	if (id <= 0) return;
	
	int encontrado = -1;
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].id == id) {
			encontrado = i;
			break;
		}
	}
	
	if(encontrado != -1) {
		printf("\n --- INFORMACION DE LA CITA ---\n");
		printf(" ID:       %d\n", db_citas[encontrado].id);
		printf(" Paciente: %s\n", db_citas[encontrado].id_paciente);
		printf(" Medico:   %s\n", db_citas[encontrado].codigo_medico);
		printf(" Estado:   %s\n", obtener_estado_str(db_citas[encontrado].estado));
		
		// 2. Regla de negocio de seguridad fuerte
		if (db_citas[encontrado].estado == 1) {
			printf("\n [!] Esta cita ya esta cancelada.\n");
		} else if (db_citas[encontrado].estado == 2 || db_citas[encontrado].estado == 3) {
			printf("\n [!] ACCESO DENEGADO: No se permite cancelar registros historicos (Atendida / No Asistio).\n");
		} else {
			char confirma[10];
			leer_texto(confirma, 10, "\n [>] ¿Desea CANCELAR esta cita? (escriba 'si' para confirmar): ");
			
			if (strcmp(confirma, "si") == 0) {
				db_citas[encontrado].estado = 1;
				guardar_citas();
				printf("\n [OK] La Cita #%d ha sido cancelada correctamente por el administrador.\n", id);
			} else {
				printf("\n [i] Operacion cancelada.\n");
			}
		}
	} else {
		printf("\n [!] Cita con ID %d no encontrada.\n", id);
	}
	
	pausa_sistema();
}


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

int cita_disponible(int id_excluir, const char *cod_med, const char *fecha, const char *hora) {
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].id != id_excluir && db_citas[i].estado == 0 &&
		   strcmp(db_citas[i].codigo_medico, cod_med) == 0 &&
		   strcmp(db_citas[i].fecha, fecha) == 0 && 
		   strcmp(db_citas[i].hora, hora) == 0) {
			return 0; 
		}
	}
	return 1; 
}
void cargar_citas() {
	FILE *f = fopen("data_citas.txt", "r");
	if (!f) { total_citas = 0; return; }
	total_citas = 0;
	char linea[256];
	while (fgets(linea, sizeof(linea), f) && total_citas < MAX_CITAS) {
		linea[strcspn(linea, "\n")] = 0; 
		char *token = strtok(linea, "|");
		if(token) {
			db_citas[total_citas].id = atoi(token);
			token = strtok(NULL, "|"); if(token) strcpy(db_citas[total_citas].id_paciente, token);
			token = strtok(NULL, "|"); if(token) strcpy(db_citas[total_citas].codigo_medico, token);
			token = strtok(NULL, "|"); if(token) strcpy(db_citas[total_citas].fecha, token);
			token = strtok(NULL, "|"); if(token) strcpy(db_citas[total_citas].hora, token); // <--- LECTURA DE HORA
			token = strtok(NULL, "|"); if(token) strcpy(db_citas[total_citas].motivo, token);
			token = strtok(NULL, "|"); if(token) db_citas[total_citas].estado = atoi(token);
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
		fprintf(f, "%d|%s|%s|%s|%s|%s|%d\n",  
				db_citas[i].id, db_citas[i].id_paciente, db_citas[i].codigo_medico,
				db_citas[i].fecha, db_citas[i].hora, db_citas[i].motivo, db_citas[i].estado);
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
	int max_id = 0;
	for (int i = 0; i < total_citas; i++) {
		if (db_citas[i].id > max_id) {
			max_id = db_citas[i].id;
		}
	}
	nueva.id = max_id + 1;
	strncpy(nueva.id_paciente, mi_cedula, sizeof(nueva.id_paciente)-1);
	nueva.id_paciente[sizeof(nueva.id_paciente)-1] = '\0';
	
	printf("\n *** NUEVA SOLICITUD DE CITA ***\n");
	
	char especialidades_unicas[20][50];
	int num_esp = 0;
	
	for (int i = 0; i < total_medicos; i++) {
		int existe = 0;
		for (int j = 0; j < num_esp; j++) {
			if (strcmp(db_medicos[i].especialidad, especialidades_unicas[j]) == 0) {
				existe = 1; break;
			}
		}
		if (!existe && num_esp < 20) {
			strcpy(especialidades_unicas[num_esp], db_medicos[i].especialidad);
			num_esp++;
		}
	}
	
	if (num_esp == 0) {
		printf(" [!] No hay medicos registrados en el sistema en este momento.\n");
		pausa_sistema();
		return;
	}
	
	int esp_seleccionada = 0;
	do {
		printf("\n *** 1. SELECCIONE UNA ESPECIALIDAD ***\n");
		for (int i = 0; i < num_esp; i++) {
			printf("  [%d] %s\n", i + 1, especialidades_unicas[i]);
		}
		esp_seleccionada = leer_entero("\n [>] Digite el numero de su opcion: ");
		
		if (esp_seleccionada < 1 || esp_seleccionada > num_esp) {
			printf(" [!] Opcion invalida. Intente de nuevo.\n");
		}
	} while (esp_seleccionada < 1 || esp_seleccionada > num_esp);
	
	char *esp_elegida = especialidades_unicas[esp_seleccionada - 1];
	
	printf("\n *** 2. MEDICOS DISPONIBLES EN %s ***\n", esp_elegida);
	printf(" %-10s | %-32s | %-15s\n", "CODIGO", "NOMBRE", "HORARIO");
	printf(" -----------------------------------------------------------------\n");
	for (int i = 0; i < total_medicos; i++) {
		if (strcmp(db_medicos[i].especialidad, esp_elegida) == 0) {
			printf(" %-10s | %-32s | %-15s\n", 
				   db_medicos[i].codigo, db_medicos[i].nombre, db_medicos[i].horario);
		}
	}
	printf(" ===================================================================\n");
	
	int medico_valido = 0;
	int idx_med = -1;
	do {
		leer_texto(nueva.codigo_medico, 10, "\n [>] Ingrese Codigo del Medico elegido (ej. MED-001): ");
		idx_med = buscar_indice_medico(nueva.codigo_medico);
		
		if(idx_med != -1) {
			
			if (strcmp(db_medicos[idx_med].especialidad, esp_elegida) == 0) {
				medico_valido = 1;
			} else {
				printf(" [!] El medico pertenece a otra especialidad. Elija uno de la lista mostrada.\n");
			}
		} else {
			printf(" [!] Codigo de medico no encontrado. Intente de nuevo.\n");
		}
	} while(!medico_valido);
	

	int fecha_valida = 0;
	do {
		leer_texto(nueva.fecha, 20, "\n [>] Fecha deseada (DD-MM-AAAA): ");
		if (!validar_fecha_futura(nueva.fecha)) {
			
		} else {
			fecha_valida = 1;
		}
	} while (!fecha_valida);
	
	char horario_doc[20];
	strcpy(horario_doc, db_medicos[idx_med].horario);
	int hora_valida = 0;
	do {
		printf("\n [i] Horario del medico: %s\n", horario_doc);
		printf(" [i] Intervalos de 2h admitidos: 08:00, 10:00, 12:00, 14:00, 16:00, 18:00\n");
		leer_texto(nueva.hora, 10, " [>] Hora deseada (HH:MM): ");
		
		if (!validar_horario_medico(horario_doc, nueva.hora)) {
			printf(" [!] Error: Hora invalida o el medico no atiende en ese turno.\n");
		} else if (!cita_disponible(-1, nueva.codigo_medico, nueva.fecha, nueva.hora)) {
			printf(" [!] Error: El medico ya tiene una cita ocupada el %s a las %s.\n", nueva.fecha, nueva.hora);
		} else {
			hora_valida = 1;
		}
	} while (!hora_valida);
	
	printf("\n *** 3. DETALLES FINALES ***\n");
	printf(" Motivos de consulta sugeridos para %s:\n", esp_elegida);
	
	const char *op1 = "Consulta general";
	const char *op2 = "Revision de examenes";
	const char *op3 = "Dolor inespecifico";
	
	if (strcmp(esp_elegida, "Medicina General") == 0) {
		op1 = "Chequeo de rutina"; op2 = "Fiebre y malestar"; op3 = "Dolor general";
	} else if (strcmp(esp_elegida, "Cardiologia") == 0) {
		op1 = "Dolor en el pecho"; op2 = "Control de presion"; op3 = "Arritmia cardiaca";
	} else if (strcmp(esp_elegida, "Pediatria") == 0) {
		op1 = "Control nino sano"; op2 = "Fiebre y tos"; op3 = "Vacunacion";
	} else if (strcmp(esp_elegida, "Oftalmologia") == 0) {
		op1 = "Perdida de vision"; op2 = "Dolor ocular"; op3 = "Revision de lentes";
	} else if (strcmp(esp_elegida, "Dermatologia") == 0) {
		op1 = "Alergia en piel"; op2 = "Acne severo"; op3 = "Revision de lunares";
	} else if (strcmp(esp_elegida, "Traumatologia") == 0) {
		op1 = "Dolor articular"; op2 = "Posible fractura"; op3 = "Dolor de espalda";
	} else if (strcmp(esp_elegida, "Neurologia") == 0) {
		op1 = "Migrana cronica"; op2 = "Mareos y vertigo"; op3 = "Convulsiones";
	}
	
	
	printf(" [1] %s\n", op1);
	printf(" [2] %s\n", op2);
	printf(" [3] %s\n", op3);
	printf(" [4] Otro (Especifique manualmente)\n");
	
	int op_motivo = 0;
	do {
		op_motivo = leer_entero("\n [>] Seleccione el numero de su motivo (1-4): ");
		if (op_motivo < 1 || op_motivo > 4) {
			printf(" [!] Opcion invalida. Intente de nuevo.\n");
		}
	} while(op_motivo < 1 || op_motivo > 4);
	
	
	if (op_motivo == 1) strcpy(nueva.motivo, op1);
	else if (op_motivo == 2) strcpy(nueva.motivo, op2);
	else if (op_motivo == 3) strcpy(nueva.motivo, op3);
	else if (op_motivo == 4) {
		leer_texto(nueva.motivo, 100, " [>] Escriba su motivo de consulta: ");
	}
	
	nueva.estado = 0;
	
	db_citas[total_citas++] = nueva;
	guardar_citas();
	
	printf("\n [OK] Cita #%d agendada correctamente para el %s a las %s.\n", nueva.id, nueva.fecha, nueva.hora);
	pausa_sistema();
}


void ver_mis_citas(const char *mi_cedula) {
	printf("\n *** MIS CITAS ***\n");
	
	printf(" %-4s | %-12s | %-7s | %-10s | %-18s | %-20s | %-12s\n", 
		   "ID", "FECHA", "HORA", "MEDICO", "ESPECIALIDAD", "MOTIVO", "ESTADO");
	printf(" ---------------------------------------------------------------------------------------------------\n");
	
	int encontradas = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].id_paciente, mi_cedula) == 0) {
			
			
			int idx_med = buscar_indice_medico(db_citas[i].codigo_medico);
			char especialidad_doc[50] = "No definida";
			if (idx_med != -1) {
				strcpy(especialidad_doc, db_medicos[idx_med].especialidad);
			}
			
			printf(" #%-3d | %-12s | %-7s | %-10s | %-18s | %-20s | %s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].hora, 
				   db_citas[i].codigo_medico, 
				   especialidad_doc,
				   db_citas[i].motivo,
				   obtener_estado_str(db_citas[i].estado));
			encontradas++;
		}
	}
	
	if(!encontradas) printf(" [i] No tiene citas registradas.\n");
	printf(" =====================================================================================================\n");
	pausa_sistema();
}


void ver_agenda_medico(const char *mi_codigo) {
	printf("\n *** MI AGENDA (%s) ***\n", mi_codigo);
	printf(" %-5s | %-12s | %-7s | %-12s | %-20s\n", "ID", "FECHA", "HORA", "PACIENTE", "MOTIVO");
	printf(" ==================================================================\n");
	
	int hay_citas = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, mi_codigo) == 0 && db_citas[i].estado == 0) {
			printf(" %-5d | %-12s | %-7s | %-12s | %-20s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].hora, 
				   db_citas[i].id_paciente, 
				   db_citas[i].motivo);
			hay_citas = 1;
		}
	}
	
	if(!hay_citas) printf(" [i] No tiene citas activas programadas.\n");
	printf(" =======================================================\n");
	pausa_sistema();
}


void ver_historial_medico(const char *mi_codigo) {
	printf("\n ******************************************************************\n");
	printf("        HISTORIAL DE PACIENTES (%s)\n", mi_codigo);
	printf(" ********************************************************************\n");
	printf(" %-5s | %-12s | %-7s | %-12s | %-15s\n", "ID", "FECHA", "HORA", "PACIENTE", "ESTADO");
	printf(" ================================================================================\n");
	
	int encontrados = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, mi_codigo) == 0) {
			printf(" %-5d | %-12s | %-7s | %-12s | %-15s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].hora, 
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
	
	fprintf(f, "********************************************\n");
	fprintf(f, "       REPORTE ESTADISTICO DEL HOSPITAL\n");
	fprintf(f, "********************************************\n");
	fprintf(f, " Total de Citas Registradas:  %d\n", total_citas);
	fprintf(f, " --------------------------------------------\n");
	fprintf(f, " [v] Citas Activas: %d\n", activas);
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
	
	
	fprintf(f_txt, "******************************************************************\n");
	fprintf(f_txt, "              REPORTE COMPLETO DE CITAS MEDICAS\n");
	fprintf(f_txt, "*******************************************************************\n\n");
	
	
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
	fprintf(f_txt, "*******************************************************************************************\n");
	fprintf(f_txt, "%-4s | %-12s | %-8s | %-10s | %-12s | %-25s | %-10s | %-8s\n",
			"ID", "FECHA", "HORA", "PACIENTE", "MEDICO", "MOTIVO", "ESTADO", "MULTA");
	
	fprintf(f_txt, "---------------------------------------------------------------------------------------------\n");
	
	for (int i = 0; i < total_citas; i++) {
		const char *estado_str = obtener_estado_str(db_citas[i].estado);
		int idx_pac = buscar_indice_paciente(db_citas[i].id_paciente);
		float multa = (idx_pac != -1) ? db_pacientes[idx_pac].multa_pendiente : 0.0;
		
		fprintf(f_txt, "%-4d | %-12s | %-8s | %-10s | %-12s | %-25s | %-10s | $%-7.2f\n",
				db_citas[i].id,
				db_citas[i].fecha,
				db_citas[i].hora,
				db_citas[i].id_paciente,
				db_citas[i].codigo_medico,
				db_citas[i].motivo,
				estado_str,
				multa);
	}
	
	
	fprintf(f_txt, "============================================================================\n\n");
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
		fprintf(f_html, "    <tr><th>ID</th><th>Fecha</th><th>Hora</th><th>Paciente</th><th>Medico</th><th>Motivo</th><th>Estado</th><th>Multa Pac</th></tr>\n");
		
		for (int i = 0; i < total_citas; i++) {
			const char *estado_str = obtener_estado_str(db_citas[i].estado);
			int idx_pac = buscar_indice_paciente(db_citas[i].id_paciente);
			float multa = (idx_pac != -1) ? db_pacientes[idx_pac].multa_pendiente : 0.0;
			
			fprintf(f_html, "    <tr>\n");
			fprintf(f_html, "      <td>%d</td>\n", db_citas[i].id);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].fecha);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].hora);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].id_paciente);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].codigo_medico);
			fprintf(f_html, "      <td>%s</td>\n", db_citas[i].motivo);
			fprintf(f_html, "      <td>%s</td>\n", estado_str);
			fprintf(f_html, "      <td>$%.2f</td>\n", multa);  // NUEVO: Mostrar multa
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
	printf("\n*********************************************************\n");
	printf("         MIS - REPORTES GERENCIALES (DASHBOARD)\n");
	printf("*********************************************************\n\n");
	
	
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
	printf(" ======================================================\n\n");
	
	
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
	printf(" ***************************************************\n");
	printf(" %-25s | %-20d\n", "Pediatricos (0-17)", pediatricos);
	printf(" %-25s | %-20d\n", "Adultos (18-64)", adultos);
	printf(" %-25s | %-20d\n", "Geriatricos (65+)", geriatricos);
	printf(" ***********************************************\n\n");
	
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
	printf("\n *** GESTIONAR ESTATUS DE CITA ***\n");
	
	printf(" %-5s | %-12s | %-7s | %-12s | %-20s\n", "ID", "FECHA", "HORA", "PACIENTE", "MOTIVO");
	printf(" ------------------------------------------------------------------\n");
	int hay_citas = 0;
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].codigo_medico, codigo_medico) == 0 && db_citas[i].estado == 0) {
			printf(" %-5d | %-12s | %-7s | %-12s | %-20s\n", 
				   db_citas[i].id, db_citas[i].fecha, db_citas[i].hora, 
				   db_citas[i].id_paciente, db_citas[i].motivo);
			hay_citas = 1;
		}
	}
	printf(" ==================================================================\n");
	
	if (!hay_citas) {
		printf(" [i] No tiene citas activas para gestionar en este momento.\n");
		pausa_sistema();
		return;
	}
	
	int id = leer_entero("\n [>] Ingrese el ID de la cita (0 para salir): ");
	if (id <= 0) return;
	
	int encontrado = -1; 
	for(int i = 0; i < total_citas; i++) {
		if(db_citas[i].id == id) {
			encontrado = i;
			break;
		}
	}
	
	if(encontrado != -1) {
		if(strcmp(db_citas[encontrado].codigo_medico, codigo_medico) != 0) {
			printf("\n [!] Permiso denegado. Esta cita esta asignada a otro medico.\n");
			pausa_sistema();
			return;
		}
		
		printf("\n *** DETALLES DE LA CITA ***\n");
		printf(" Paciente: %s\n", db_citas[encontrado].id_paciente);
		printf(" Fecha:    %s\n", db_citas[encontrado].fecha);
		printf(" Hora:     %s\n", db_citas[encontrado].hora);
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
			registrar_inasistencia(db_citas[encontrado].id_paciente);  // NUEVO: Multa
			guardar_citas();
			printf("\n [OK] Cita marcada como NO ASISTIO correctamente.\n");
			printf(" [!] Se ha registrado una multa de $25 por inasistencia.\n");
		}		
	pausa_sistema();
}
}
void cancelar_cita_paciente(const char *mi_cedula) {
	printf("\n *** CANCELAR UNA CITA ACTIVA ***\n");
	
	
	int activas = 0;
	printf(" %-4s | %-12s | %-7s | %-10s | %-18s | %-20s | %-12s\n", "ID", "FECHA", "HORA", "MEDICO", "ESPECIALIDAD", "MOTIVO", "ESTADO");
	printf(" ================================================================================================\n");
	for(int i=0; i<total_citas; i++) {
		if(strcmp(db_citas[i].id_paciente, mi_cedula) == 0 && db_citas[i].estado == 0) {
			
			int idx_med = buscar_indice_medico(db_citas[i].codigo_medico);
			char especialidad_doc[50] = "No definida";
			if (idx_med != -1) strcpy(especialidad_doc, db_medicos[idx_med].especialidad);
			
			printf(" #%-3d | %-12s | %-7s | %-10s | %-18s | %-20s | %s\n", 
				   db_citas[i].id, 
				   db_citas[i].fecha, 
				   db_citas[i].hora, 
				   db_citas[i].codigo_medico, 
				   especialidad_doc,
				   db_citas[i].motivo, 
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

void reprogramar_cita_admin() {
	printf("\n *** REPROGRAMAR CITA ACTIVA ***\n");
	int id = leer_entero(" [>] Ingrese ID de la cita a reprogramar (0 para volver): ");
	if (id <= 0) return;
	
	int encontrado = -1;
	for(int i=0; i<total_citas; i++) {
		if(db_citas[i].id == id) { encontrado = i; break; }
	}
	
	if(encontrado != -1) {
		if (db_citas[encontrado].estado != 0) {
			printf("\n [!] ACCESO DENEGADO: Solo puede reprogramar citas ACTIVAS.\n");
		} else {
			printf("\n *** DATOS ACTUALES DE LA CITA ***\n");
			printf(" ID:       %d\n Fecha:    %s\n Hora:     %s\n Medico:   %s\n", 
				   db_citas[encontrado].id, db_citas[encontrado].fecha, db_citas[encontrado].hora, db_citas[encontrado].codigo_medico);
			
			
			int idx_med = buscar_indice_medico(db_citas[encontrado].codigo_medico);
			char horario_doc[20];
			strcpy(horario_doc, db_medicos[idx_med].horario);
			
			char nueva_fecha[20], nueva_hora[10];
			int fecha_hora_valida = 0;
			
			do {
				leer_texto(nueva_fecha, 20, "\n [>] Ingrese NUEVA fecha (DD-MM-AAAA): ");
				if (validar_fecha_futura(nueva_fecha)) {
					
					printf("\n [i] El medico trabaja en horario: %s\n", horario_doc);
					printf(" [i] Intervalos de 2h admitidos: 08:00, 10:00, 12:00, 14:00, 16:00, 18:00\n");
					leer_texto(nueva_hora, 10, " [>] Ingrese NUEVA hora (HH:MM): ");
					
					if (!validar_horario_medico(horario_doc, nueva_hora)) {
						printf(" [!] Error: Hora invalida o el medico no trabaja en ese turno.\n");
					} else if (!cita_disponible(id, db_citas[encontrado].codigo_medico, nueva_fecha, nueva_hora)) {
						printf(" [!] Error: El medico ya tiene una cita ocupada el %s a las %s.\n", nueva_fecha, nueva_hora);
					} else {
						fecha_hora_valida = 1;
					}
				}
			} while(!fecha_hora_valida);
			
			
			strcpy(db_citas[encontrado].fecha, nueva_fecha);
			strcpy(db_citas[encontrado].hora, nueva_hora);
			guardar_citas();
			printf("\n [OK] Cita #%d reprogramada para el %s a las %s.\n", id, nueva_fecha, nueva_hora);
		}
	} else {
		printf("\n [!] Cita con ID %d no encontrada.\n", id);
	}
	pausa_sistema();
}

void gestion_citas_admin() {
	char op = ' ';
	while(op != 'x') {
		limpiar_pantalla();
		printf("\n *** PANEL DE CONTROL DE CITAS (ADMINISTRADOR) ***\n");
		
		int activas = 0;
		printf(" %-5s | %-12s | %-7s | %-12s | %-10s | %-20s\n", "ID", "FECHA", "HORA", "PACIENTE", "MEDICO", "MOTIVO");
		printf(" =======================================================================\n");
		for(int i=0; i<total_citas; i++) {
			if(db_citas[i].estado == 0) {
				printf(" %-5d | %-12s | %-7s | %-12s | %-10s | %-20s\n", 
					   db_citas[i].id, db_citas[i].fecha, db_citas[i].hora, 
					   db_citas[i].id_paciente, db_citas[i].codigo_medico, db_citas[i].motivo);
				activas++;
			}
		}
		
		if (activas == 0) {
			printf("\n [i] No hay citas activas pendientes en el sistema.\n");
			pausa_sistema();
			return;
		}
		
		printf("\n Opciones Administrativas:\n");
		printf(" [a] Cancelar una cita\n");
		printf(" [b] Reprogramar una cita\n");
		printf(" [x] Volver\n");
		
		char buff[10];
		leer_texto(buff, 10, " [>] Seleccione: ");
		op = tolower(buff[0]);
		
		if (op == 'a') {
			int id = leer_entero("\n [>] Ingrese ID de la cita a cancelar (0 para volver): ");
			if (id <= 0) continue;
			
			int encontrado = -1;
			for(int i=0; i<total_citas; i++) {
				if(db_citas[i].id == id) {
					encontrado = i; break;
				}
			}
			
			if(encontrado != -1 && db_citas[encontrado].estado == 0) {
				char confirma[10];
				leer_texto(confirma, 10, " [>] ¿Desea CANCELAR esta cita? (escriba 'si'): ");
				if (strcmp(confirma, "si") == 0) {
					db_citas[encontrado].estado = 1;
					guardar_citas();
					printf("\n [OK] Cita #%d cancelada.\n", id);
				}
			} else {
				printf("\n [!] Cita no encontrada o ya procesada.\n");
			}
			pausa_sistema();
			
		} else if (op == 'b') {
			reprogramar_cita_admin();
		}
	}
}

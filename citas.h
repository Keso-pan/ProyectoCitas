#ifndef CITAS_H
#define CITAS_H

#define MAX_CITAS 200

typedef struct {
	int id;
	char id_paciente[11];
	char codigo_medico[10];
	char fecha[20];
	char motivo[100];
	int estado; 
} Cita;

extern Cita db_citas[MAX_CITAS];
extern int total_citas;

void cargar_citas();
void guardar_citas();
void solicitar_cita(const char *mi_cedula);
void gestion_citas_admin();

void ver_mis_citas(const char *mi_cedula);
void ver_agenda_medico(const char *mi_codigo);
void generar_reporte_general();
void ver_historial_medico(const char *mi_codigo);


void cancelar_citas_usuario(const char *cedula);
void generar_reportes_mis();
#endif

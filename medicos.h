#ifndef MEDICOS_H
#define MEDICOS_H

#define MAX_MEDICOS 200

typedef struct {
	char codigo[10];
	char nombre[60];
	int edad;
	char especialidad[50];
	char horario[20];
	char contrasena[20]; 
} Medico;

extern Medico db_medicos[MAX_MEDICOS];
extern int total_medicos;

void cargar_medicos();
void guardar_medicos();
void registrar_medico();
void listar_medicos();
int buscar_indice_medico(const char *codigo);
void gestion_medicos_admin();
void exportar_reporte_medicos();
void ver_perfil_medico(const char *codigo);

int eliminar_medico(const char *codigo);
int verificar_contrasena_medico(const char *codigo, const char *pass_intento);

#endif

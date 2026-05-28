#ifndef PACIENTES_H
#define PACIENTES_H

#define MAX_PACIENTES 100

typedef struct {
	char cedula[20];
	char nombre_completo[60];
	int edad;
	char telefono[15];
	char contrasena[20]; 
} Paciente;

extern Paciente db_pacientes[MAX_PACIENTES];
extern int total_pacientes;

void cargar_pacientes();
void guardar_pacientes();
int buscar_indice_paciente(const char *cedula);
void registrar_paciente();
void listar_pacientes();
void ver_perfil_paciente(const char *mi_cedula);
void exportar_reporte_pacientes();

void modificar_paciente(const char *cedula);
int eliminar_paciente(const char *cedula);

int verificar_contrasena_paciente(int index, const char *pass_intento);

#endif

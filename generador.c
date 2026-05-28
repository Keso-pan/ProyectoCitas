#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *nombres[] = {"Andres", "Maria", "Carlos", "Sofia", "Jorge", "Ana", "Luis", "Elena", "Roberto", "Lucia", "Diego", "Carmen", "Fernando", "Diana"};
const char *apellidos[] = {"Lopez", "Perez", "Garcia", "Martinez", "Zambrano", "Castro", "Velez", "Sanchez", "Ramirez", "Torres", "Morales", "Ortiz"};
const char *especialidades[] = {"Medicina General", "Cardiologia", "Pediatria", "Ginecologia", "Dermatologia", "Traumatologia", "Neurologia"};
const char *motivos[] = {"Chequeo de rutina", "Fiebre aguda", "Dolor articular", "Control anual", "Sintomas virales", "Dolor de cabeza", "Revision general"};
const char *horarios[] = {"08:00-14:00", "14:00-20:00", "08:00-20:00"};

// Matriz de ocupacion: [medico_id][dia_del_ano_0_a_364]
// Garantiza que un doctor no tenga dos citas el mismo dia de forma exacta.
int doctor_ocupado[200][365] = {0}; 

// Generador de cedula con algoritmo de modulo 10
void generar_cedula_valida(char *cedula) {
	int provincia = (rand() % 24) + 1;
	int tercer_digito = rand() % 6; 
	int num[9];
	
	num[0] = provincia / 10;
	num[1] = provincia % 10;
	num[2] = tercer_digito;
	
	for(int i = 3; i < 9; i++) num[i] = rand() % 10;
	
	int coeficientes[] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
	int suma = 0;
	for(int i = 0; i < 9; i++) {
		int producto = num[i] * coeficientes[i];
		if (producto >= 10) producto -= 9;
		suma += producto;
	}
	
	int decena_superior = ((suma / 10) + 1) * 10;
	if (suma % 10 == 0) decena_superior = suma;
	int digito_verificador = decena_superior - suma;
	
	sprintf(cedula, "%d%d%d%d%d%d%d%d%d%d", num[0], num[1], num[2], num[3], num[4], num[5], num[6], num[7], num[8], digito_verificador);
}

int main() {
	srand(time(NULL)); 
	
	char cedulas_creadas[500][15];
	char codigos_medicos[200][10];
	
	// Obtener año actual automáticamente
	time_t t = time(NULL);
	struct tm *ahora = localtime(&t);
	int anio_actual = ahora->tm_year + 1900;
	
	printf("\n [i] Iniciando la fabrica de datos organizados...\n");
	
	// =======================================================
	// 1. GENERAR 500 PACIENTES
	// =======================================================
	FILE *f_pac = fopen("data_pacientes.txt", "w");
	if (!f_pac) return 1;
	for(int i = 0; i < 500; i++) {
		char cedula[15], nombre[60], telefono[15], contrasena[20];
		generar_cedula_valida(cedula);
		strcpy(cedulas_creadas[i], cedula); 
		
		sprintf(nombre, "%s %s", nombres[rand() % 14], apellidos[rand() % 12]);
		int edad = (rand() % 116) + 5; 
		sprintf(telefono, "09%04d%04d", rand() % 10000, rand() % 10000); 
		
		sprintf(contrasena, "pass%s", &cedula[6]);
		
		fprintf(f_pac, "%s|%s|%d|%s|%s\n", cedula, nombre, edad, telefono, contrasena);
	}
	fclose(f_pac);
	printf(" [OK] 500 Pacientes generados.\n");
	
	// =======================================================
	// 2. GENERAR 200 MEDICOS
	// =======================================================
	FILE *f_med = fopen("data_medicos.txt", "w");
	if (!f_med) return 1;
	for(int i = 0; i < 200; i++) {
		char codigo[10], nombre[60], contrasena[20];
		sprintf(codigo, "MED-%03d", i + 1);
		strcpy(codigos_medicos[i], codigo);
		
		sprintf(nombre, "Dr. %s %s", nombres[rand() % 14], apellidos[rand() % 12]);
		int edad = (rand() % 61) + 25; 
		const char *especialidad = especialidades[rand() % 7];
		const char *horario = horarios[rand() % 3];
		
		sprintf(contrasena, "doc%03d", i + 1);
		
		fprintf(f_med, "%s|%s|%d|%s|%s|%s\n", codigo, nombre, edad, especialidad, horario, contrasena);
	}
	fclose(f_med);
	printf(" [OK] 200 Medicos generados.\n");
	
	// =======================================================
	// 3. GENERAR 1000 CITAS (2 por paciente sin cruces)
	// =======================================================
	FILE *f_cit = fopen("data_citas.txt", "w");
	if (!f_cit) return 1;
	
	int cita_id = 1;
	
	for(int p = 0; p < 500; p++) {
		for(int c = 0; c < 2; c++) {
			const char *cedula_pac = cedulas_creadas[p];
			
			// Seleccionamos un medico al azar
			int doc_idx = rand() % 200;
			const char *cod_med = codigos_medicos[doc_idx];
			
			int offset_dias;
			
			// Busca un día (de 0 a 364) donde el doctor esté libre
			do {
				offset_dias = rand() % 365;
			} while (doctor_ocupado[doc_idx][offset_dias] == 1);
			
			doctor_ocupado[doc_idx][offset_dias] = 1;
			
			// Configuramos la fecha base: 1 de Junio del año actual
			struct tm fecha_cita = {0};
			fecha_cita.tm_year = anio_actual - 1900;
			fecha_cita.tm_mon  = 5; // Junio es el mes 5 (va del 0 al 11)
			fecha_cita.tm_mday = 1 + offset_dias; // Le sumamos los días aleatorios
			
			// Normalizar fecha (C recalcula meses y años automáticamente)
			mktime(&fecha_cita);
			
			char fecha[20];
			sprintf(fecha, "%02d-%02d-%04d", fecha_cita.tm_mday, fecha_cita.tm_mon + 1, fecha_cita.tm_year + 1900);
			
			const char *motivo = motivos[rand() % 7];
			
			// Probabilidades de estado (70% Activa, 10% Cancelada, 10% Atendida, 10% No Asistió)
			int probabilidad = rand() % 100;
			int estado = 0; 
			if (probabilidad >= 70 && probabilidad < 80) estado = 1;      // Cancelada
			else if (probabilidad >= 80 && probabilidad < 90) estado = 2; // Atendida
			else if (probabilidad >= 90) estado = 3;                      // No Asistio
			
			fprintf(f_cit, "%d|%s|%s|%s|%s|%d\n", cita_id, cedula_pac, cod_med, fecha, motivo, estado);
			cita_id++;
		}
	}
	fclose(f_cit);
	printf(" [OK] 1000 Citas generadas (a partir del 1-06-%d).\n", anio_actual);
	printf(" [OK] Estados incluidos: Activas, Canceladas, Atendidas, No Asistieron.\n");
	printf(" [OK] Cruces de horarios de medicos: 0 comprobados.\n");
	
	printf("\n [EXITO] Datos listos. Ejecuta tu proyecto principal para probar los reportes.\n");
	system("pause");
	return 0;
}

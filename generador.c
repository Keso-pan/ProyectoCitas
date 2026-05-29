#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *nombres[] = {"Andres", "Maria", "Carlos", "Sofia", "Jorge", "Ana", "Luis", "Elena", "Roberto", "Lucia", "Diego", "Carmen", "Fernando", "Diana"};
const char *apellidos[] = {"Lopez", "Perez", "Garcia", "Martinez", "Zambrano", "Castro", "Velez", "Sanchez", "Ramirez", "Torres", "Morales", "Ortiz"};

const char *especialidades[] = {"Medicina General", "Cardiologia", "Pediatria", "Oftalmologia", "Dermatologia", "Traumatologia", "Neurologia"};
const char *horarios[] = {"08:00-14:00", "14:00-20:00"};

int doctor_ocupado[200][365][6] = {0}; 
const char *horas_txt[] = {"08:00", "10:00", "12:00", "14:00", "16:00", "18:00"};

void generar_cedula_valida(char *cedula) {
	int provincia = (rand() % 24) + 1;
	int num[9];
	num[0] = provincia / 10; num[1] = provincia % 10; num[2] = rand() % 6;
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
	sprintf(cedula, "%d%d%d%d%d%d%d%d%d%d", num[0], num[1], num[2], num[3], num[4], num[5], num[6], num[7], num[8], decena_superior - suma);
}

int main() {
	srand(time(NULL)); 
	char cedulas_creadas[500][15];
	char codigos_medicos[200][10];
	char horarios_med_gen[200][20];
	char esp_medicos_gen[200][50]; 
	
	time_t t = time(NULL);
	struct tm *ahora = localtime(&t);
	
	printf("\n [i] Iniciando generador avanzado (Motivos Realistas)...\n");
	
	FILE *f_pac = fopen("data_pacientes.txt", "w");
	for(int i = 0; i < 500; i++) {
		char cedula[15]; generar_cedula_valida(cedula); strcpy(cedulas_creadas[i], cedula); 
		fprintf(f_pac, "%s|%s %s|%d|09%04d%04d|pass%s\n", cedula, nombres[rand() % 14], apellidos[rand() % 12], (rand() % 108) + 13, rand() % 10000, rand() % 10000, &cedula[6]);
	}
	fclose(f_pac);
	
	FILE *f_med = fopen("data_medicos.txt", "w");
	for(int i = 0; i < 200; i++) {
		sprintf(codigos_medicos[i], "MED-%03d", i + 1);
		const char *horario = horarios[rand() % 2];
		strcpy(horarios_med_gen[i], horario);
		
		const char *esp = especialidades[rand() % 7];
		strcpy(esp_medicos_gen[i], esp); 
		
		fprintf(f_med, "%s|Dr. %s %s|%d|%s|%s|doc%03d\n", codigos_medicos[i], nombres[rand() % 14], apellidos[rand() % 12], (rand() % 61) + 25, esp, horario, i + 1);
	}
	fclose(f_med);
	
	FILE *f_cit = fopen("data_citas.txt", "w");
	int cita_id = 1;
	for(int p = 0; p < 500; p++) {
		for(int c = 0; c < 2; c++) { 
			int doc_idx = rand() % 200;
			
			int valid_slots[6], num_slots = 0;
			if (strcmp(horarios_med_gen[doc_idx], "08:00-14:00") == 0) {
				valid_slots[0] = 0; valid_slots[1] = 1; valid_slots[2] = 2; num_slots = 3;
			} else if (strcmp(horarios_med_gen[doc_idx], "14:00-20:00") == 0) {
				valid_slots[0] = 3; valid_slots[1] = 4; valid_slots[2] = 5; num_slots = 3;
			} else {
				for(int s=0; s<6; s++) valid_slots[s] = s; num_slots = 6;
			}
			
			int offset_dias, real_slot;
			do {
				offset_dias = rand() % 365;
				real_slot = valid_slots[rand() % num_slots];
			} while (doctor_ocupado[doc_idx][offset_dias][real_slot] == 1);
			
			doctor_ocupado[doc_idx][offset_dias][real_slot] = 1;
			
			struct tm fecha_cita = *ahora;
			fecha_cita.tm_mday += offset_dias; 
			mktime(&fecha_cita);
			
			char fecha[20];
			sprintf(fecha, "%02d-%02d-%04d", fecha_cita.tm_mday, fecha_cita.tm_mon + 1, fecha_cita.tm_year + 1900);
			const char *hora = horas_txt[real_slot];
			
			// LÓGICA DE MOTIVOS REALISTAS POR ESPECIALIDAD
			const char *esp_actual = esp_medicos_gen[doc_idx];
			const char *motivo_elegido = "Consulta general";
			int rand_mot = rand() % 3;
			
			if(strcmp(esp_actual, "Medicina General") == 0) {
				const char *m[] = {"Chequeo de rutina", "Fiebre y malestar", "Dolor general"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Cardiologia") == 0) {
				const char *m[] = {"Dolor en el pecho", "Control de presion", "Arritmia cardiaca"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Pediatria") == 0) {
				const char *m[] = {"Control nino sano", "Fiebre y tos", "Vacunacion"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Oftalmologia") == 0) {
				const char *m[] = {"Perdida de vision", "Dolor ocular", "Revision de lentes"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Dermatologia") == 0) {
				const char *m[] = {"Alergia en piel", "Acne severo", "Revision de lunares"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Traumatologia") == 0) {
				const char *m[] = {"Dolor articular", "Posible fractura", "Dolor de espalda"}; motivo_elegido = m[rand_mot];
			} else if(strcmp(esp_actual, "Neurologia") == 0) {
				const char *m[] = {"Migrana cronica", "Mareos y vertigo", "Convulsiones"}; motivo_elegido = m[rand_mot];
			}
			
			int prob = rand() % 100, estado = 0; 
			if (prob >= 70 && prob < 80) estado = 1; else if (prob >= 80 && prob < 90) estado = 2; else if (prob >= 90) estado = 3;
			
			fprintf(f_cit, "%d|%s|%s|%s|%s|%s|%d\n", cita_id++, cedulas_creadas[p], codigos_medicos[doc_idx], fecha, hora, motivo_elegido, estado);
		}
	}
	fclose(f_cit);
	
	printf("\n ==================================================================\n");
	printf("                   RESUMEN DE GENERACION DE DATOS                  \n");
	printf(" ==================================================================\n");
	printf("  [OK] %-30s: 500 registros\n", "Pacientes creados");
	printf("  [OK] %-30s: 200 registros\n", "Medicos creados (con horario)");
	printf("  [OK] %-30s: 1000 registros\n", "Citas agendadas");
	printf(" ------------------------------------------------------------------\n");
	printf("\n [EXITO] Base de datos lista. Ya puedes ejecutar MiProyecto.exe\n\n");
	printf(" ==================================================================\n");
	system("pause");
	return 0;
}

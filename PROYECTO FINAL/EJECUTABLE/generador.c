#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_PACIENTES 15000
#define NUM_MEDICOS 500
#define NUM_CITAS 30000

const char *nombres[] = {"Andres", "Maria", "Carlos", "Sofia", "Jorge", "Ana", "Luis", "Elena", "Roberto", "Lucia", "Diego", "Carmen", "Fernando", "Diana", "Javier", "Valeria", "Ricardo", "Camila", "Hugo", "Isabel", "Mateo", "Valentina"};
const char *apellidos[] = {"Lopez", "Perez", "Garcia", "Martinez", "Zambrano", "Castro", "Velez", "Sanchez", "Ramirez", "Torres", "Morales", "Ortiz", "Mendoza", "Rojas", "Herrera", "Jimenez"};

const char *especialidades[] = {
	"Medicina General", "Cardiologia", "Pediatria", "Oftalmologia", 
		"Dermatologia", "Traumatologia", "Neurologia", "Gastroenterologia", 
		"Neumologia", "Endocrinologia", "Otorrinolaringologia", "Nefrologia"
};

const char *horarios[] = {"08:00-14:00", "14:00-20:00"};
const char *horas_txt[] = {"08:00", "10:00", "12:00", "14:00", "16:00", "18:00"};

int doctor_ocupado[NUM_MEDICOS][365][6] = {0}; 
char cedulas_creadas[NUM_PACIENTES][15]; 
char horarios_med_gen[NUM_MEDICOS][20]; 
char esp_medicos_gen[NUM_MEDICOS][50]; 

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
	srand((unsigned int)time(NULL)); 
	
	printf("\n [i] Generando BD Masiva (Liderazgo Organico Medicina General al 12%%)...\n");
	
	FILE *f_pac = fopen("data_pacientes.txt", "w");
	for(int i = 0; i < NUM_PACIENTES; i++) {
		char cedula[15]; generar_cedula_valida(cedula); strcpy(cedulas_creadas[i], cedula); 
		fprintf(f_pac, "%s|%s %s|%d|09%04d%04d|pass%s\n", cedula, nombres[rand() % 22], apellidos[rand() % 16], (rand() % 75) + 13, rand() % 10000, rand() % 10000, &cedula[6]);
	}
	fclose(f_pac);
	
	FILE *f_med = fopen("data_medicos.txt", "w");
	for(int i = 0; i < NUM_MEDICOS; i++) {
		// AJUSTE CLAVE: 12% para Medicina General. Ahora tendrá un liderazgo suave y creíble.
		int prob_esp = rand() % 100;
		const char *esp;
		if (prob_esp < 12) {
			esp = "Medicina General";
		} else {
			esp = especialidades[(rand() % 11) + 1]; 
		}
		
		const char *horario = horarios[rand() % 2];
		strcpy(horarios_med_gen[i], horario); strcpy(esp_medicos_gen[i], esp); 
		fprintf(f_med, "MED-%03d|Dr. %s %s|%d|%s|%s|doc%03d\n", i + 1, nombres[rand() % 22], apellidos[rand() % 16], (rand() % 40) + 25, esp, horario, i + 1);
	}
	fclose(f_med);
	
	time_t t_now = time(NULL);
	struct tm *tm_base = localtime(&t_now);
	tm_base->tm_hour = 12; 
	
	FILE *f_cit = fopen("data_citas.txt", "w");
	int cita_id = 1;
	int citas_generadas = 0;
	
	int contador_estados[4] = {0};
	int contador_curva_meses[12] = {0};
	
	int pesos_curva[12] = {4, 5, 6, 7, 9, 11, 14, 15, 10, 7, 6, 6}; 
	
	while(citas_generadas < NUM_CITAS) {
		
		int prob_mes = rand() % 100;
		int acumulado = 0;
		int mes_elegido = 0;
		
		for (int m = 0; m < 12; m++) {
			acumulado += pesos_curva[m];
			if (prob_mes < acumulado) {
				mes_elegido = m;
				break;
			}
		}
		
		int dias_desplazamiento = (mes_elegido * 30) + (rand() % 30);
		
		int d_idx = rand() % NUM_MEDICOS;
		int slot = (strcmp(horarios_med_gen[d_idx], "08:00-14:00") == 0) ? (rand() % 3) : ((rand() % 3) + 3);
		
		if (doctor_ocupado[d_idx][dias_desplazamiento][slot] == 0) {
			doctor_ocupado[d_idx][dias_desplazamiento][slot] = 1;
			
			int estado;
			int prob = rand() % 10000; 
			
			if (prob < 7800) estado = 2;       
			else if (prob < 9000) estado = 0;  
			else if (prob < 9600) estado = 1;  
			else estado = 3;                   
			
			int p_idx;
			if (estado == 3) {
				p_idx = rand() % 300; 
			} else {
				p_idx = rand() % NUM_PACIENTES; 
			}
			
			struct tm time_copy = *tm_base;
			time_copy.tm_mday += dias_desplazamiento;
			mktime(&time_copy); 
			
			char fecha_cita[15];
			sprintf(fecha_cita, "%02d-%02d-%04d", time_copy.tm_mday, time_copy.tm_mon + 1, time_copy.tm_year + 1900);
			
			const char *esp_actual = esp_medicos_gen[d_idx];
			const char *motivo_elegido = "Consulta general";
			int rand_mot = rand() % 3;
			
			if (strcmp(esp_actual, "Medicina General") == 0) { const char *m[] = {"Chequeo de rutina", "Fiebre y malestar", "Dolor general"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Cardiologia") == 0) { const char *m[] = {"Dolor en el pecho", "Control de presion", "Arritmia cardiaca"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Pediatria") == 0) { const char *m[] = {"Control nino sano", "Fiebre y tos", "Vacunacion"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Oftalmologia") == 0) { const char *m[] = {"Perdida de vision", "Dolor ocular", "Revision de lentes"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Dermatologia") == 0) { const char *m[] = {"Alergia en piel", "Acne severo", "Revision de lunares"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Traumatologia") == 0) { const char *m[] = {"Dolor articular", "Posible fractura", "Dolor de espalda"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Neurologia") == 0) { const char *m[] = {"Migrana cronica", "Mareos y vertigo", "Convulsiones"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Gastroenterologia") == 0) { const char *m[] = {"Dolor abdominal", "Reflujo gastrico", "Control de gastritis"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Neumologia") == 0) { const char *m[] = {"Dificultad al respirar", "Tos cronica", "Crisis de Asma"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Endocrinologia") == 0) { const char *m[] = {"Control de diabetes", "Problemas de tiroides", "Desorden hormonal"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Otorrinolaringologia") == 0) { const char *m[] = {"Dolor de oido", "Sinusitis severa", "Problemas de garganta"}; motivo_elegido = m[rand_mot]; }
			else if (strcmp(esp_actual, "Nefrologia") == 0) { const char *m[] = {"Control renal", "Infeccion urinaria", "Calculos renales"}; motivo_elegido = m[rand_mot]; }
			
			fprintf(f_cit, "%d|%s|MED-%03d|%s|%s|%s|%d\n", 
					cita_id++, cedulas_creadas[p_idx], d_idx + 1, fecha_cita, horas_txt[slot], motivo_elegido, estado);
			
			contador_estados[estado]++;
			contador_curva_meses[mes_elegido]++;
			citas_generadas++;
		}
	}
	fclose(f_cit);
	
	printf("\n === RESULTADOS EXACTOS DE ESTADOS ===\n");
	printf(" - Activa (0)    : %.2f%%\n", (contador_estados[0] / (float)NUM_CITAS) * 100);
	printf(" - Cancelada (1) : %.2f%%\n", (contador_estados[1] / (float)NUM_CITAS) * 100);
	printf(" - Atendida (2)  : %.2f%%\n", (contador_estados[2] / (float)NUM_CITAS) * 100);
	printf(" - No Asistio (3): %.2f%%\n", (contador_estados[3] / (float)NUM_CITAS) * 100);
	printf(" ========================================\n");
	
	return 0;
}

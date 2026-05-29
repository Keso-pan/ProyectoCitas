#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


const char *nombres[] = {"Andres", "Maria", "Carlos", "Sofia", "Jorge", "Ana", "Luis", "Elena", "Roberto", "Lucia", "Diego", "Carmen", "Fernando", "Diana", "Javier", "Valeria", "Ricardo", "Camila", "Hugo", "Isabel", "Mateo", "Valentina"};
const char *apellidos[] = {"Lopez", "Perez", "Garcia", "Martinez", "Zambrano", "Castro", "Velez", "Sanchez", "Ramirez", "Torres", "Morales", "Ortiz", "Mendoza", "Rojas", "Herrera", "Jimenez"};

// 12 Especialidades Neutrales
const char *especialidades[] = {
	"Medicina General", "Cardiologia", "Pediatria", "Oftalmologia", 
		"Dermatologia", "Traumatologia", "Neurologia", "Gastroenterologia", 
		"Neumologia", "Endocrinologia", "Otorrinolaringologia", "Nefrologia"
};

const char *horarios[] = {"08:00-14:00", "14:00-20:00"};
const char *horas_txt[] = {"08:00", "10:00", "12:00", "14:00", "16:00", "18:00"};


int doctor_ocupado[250][365][6] = {0}; 

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
	char cedulas_creadas[700][15]; 
	
	printf("\n [i] Generando base de datos corporativa (700 Pacientes, 250 Médicos, 1000 Citas)...\n");
	

	FILE *f_pac = fopen("data_pacientes.txt", "w");
	for(int i = 0; i < 700; i++) {
		char cedula[15]; generar_cedula_valida(cedula); strcpy(cedulas_creadas[i], cedula); 
		fprintf(f_pac, "%s|%s %s|%d|09%04d%04d|pass%s\n", cedula, nombres[rand() % 22], apellidos[rand() % 16], (rand() % 108) + 13, rand() % 10000, rand() % 10000, &cedula[6]);
	}
	fclose(f_pac);
	

	FILE *f_med = fopen("data_medicos.txt", "w");
	for(int i = 0; i < 250; i++) {
		const char *esp = especialidades[rand() % 12];
		const char *horario = horarios[rand() % 2];
		fprintf(f_med, "MED-%03d|Dr. %s %s|%d|%s|%s|doc%03d\n", i + 1, nombres[rand() % 22], apellidos[rand() % 16], (rand() % 40) + 25, esp, horario, i + 1);
	}
	fclose(f_med);
	

	FILE *f_cit = fopen("data_citas.txt", "w");
	int cita_id = 1;
	while(cita_id <= 1000) {
		int p_idx = rand() % 700;
		int d_idx = rand() % 250;
		int dia = rand() % 365;
		int slot = rand() % 6;
		
		
		if (doctor_ocupado[d_idx][dia][slot] == 0) {
			doctor_ocupado[d_idx][dia][slot] = 1;
			
			
			int estado = rand() % 4; 
			
			fprintf(f_cit, "%d|%s|MED-%03d|%02d-%02d-2026|%s|Consulta medica|%d\n", 
					cita_id++, cedulas_creadas[p_idx], d_idx + 1, (rand() % 28) + 1, (rand() % 12) + 1, horas_txt[slot], estado);
		}
	}
	fclose(f_cit);
	
	printf("\n [OK] Generación masiva completada exitosamente.\n");
	return 0;
}

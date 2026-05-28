#ifndef AUTH_H
#define AUTH_H

// Definicion de Roles del Sistema
#define ROLE_NONE 0
#define ROLE_ADMIN 1
#define ROLE_DOCTOR 2
#define ROLE_PATIENT 3

// Estructura para credenciales 
typedef struct {
	char username[20];
	char password[20];
	int role_id;
} UserCredential;

// Funcion principal de login. Retorna el Rol ID y guarda el usuario que entro.
int core_perform_login(char *logged_user_id_out);

#endif

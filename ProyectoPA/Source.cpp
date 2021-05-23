#include <Windows.h>
#include <string>
#include <fstream>
#include "resource.h"

using namespace std;

struct User {
	int userId;
	string username;
	string password;
	string name;
	User* prev;
	User* next;
}*oUser, * aUser, * logged;

int GLOBAL_USER_ID = 1;
fstream archive;

//Protipos de los callbacks
BOOL CALLBACK fLoginDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fRegistroDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fInfoVendedorDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fEnviosMisEnvios(HWND, UINT, WPARAM, LPARAM);

void handleLoginClickButton(HWND);
void cerrarVentana(HWND);
void setEnable(HWND, UINT, bool);
void handleRegisterClickButton(HWND);
void createRegisterDialog();

//Creacion de las ventanas
void createLoginDialog();
void createModalDialog(HWND);
void createVendedorInfoDialog();
void createEnviosMisEnvios();

// Alertas
void mostrarMensaje(HWND, LPCSTR);


//Banderas

bool isExitLogin = false;
bool closeRegister = false;
bool closeVendedorInfo = false;

//Helpers
string getText(int, HWND);
void freeMemory();


//Usuarios
void loadUsers();
void saveUserId();
void loadUserId();
void saveUser(User*);

// Handlers Register Dialog
void handleRegistrarUsuario(HWND);


HINSTANCE hGlobalInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	
	hGlobalInstance = hInstance;

	//Primera ventana
	createLoginDialog();
	//createRegisterDialog();
	//createEnviosMisEnvios();
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;
}



void handleLoginClickButton(HWND hwnd) {

	//Logeo del usuario
	
	string password = getText(TXT_PASSWORD, hwnd);
	string username = getText(TXT_NOMBRE, hwnd);
	if (password.compare("") == 0 || username.compare("") == 0) {
		MessageBox(NULL, "Falto llenar la info", "ERROR", MB_ICONERROR);
		return;
	}

	if (oUser != NULL) {
		bool isUserFound = false;
		while (aUser != NULL) {
			if (aUser->username.compare(username) == 0 && aUser->password.compare(password) == 0)
				isUserFound = true;
				break;
			aUser = aUser->next;
		}

		if (isUserFound) {
			logged = aUser;
			aUser = oUser;
			createEnviosMisEnvios();
			DestroyWindow(hwnd);
			isExitLogin = false;
		}
		else {
			aUser = oUser;
			MessageBox(NULL, "Credenciales no aceptadas, revise sus datos", "ERROR", MB_ICONERROR);
			return;
		}
	}
	else {
		MessageBox(NULL, "No hay usuarios en el proyecto", "ERROR", MB_ICONERROR);
		return;
	}

}




void handleRegisterClickButton(HWND hwnd) {
	ShowWindow(hwnd, SW_HIDE);
	createRegisterDialog();
}




void createModalDialog(HWND hwnd) {
	int result = DialogBox(hGlobalInstance, MAKEINTRESOURCE(REGISTRO_SCREEN), hwnd, fRegistroDialog);

	//In getMessage();
	// EndDialog(hwnd, INT_ID_EXIT);

	//Back to hom

}




//Creacion de ventanas


void createLoginDialog() {
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(LOGIN_SCREEN), NULL, fLoginDialog);
	ShowWindow(ventana, SW_SHOW);
}

void createVendedorInfoDialog() {
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(SCREEN_INFORMACION_VENDEDOR), NULL, fInfoVendedorDialog);
	ShowWindow(ventana, SW_SHOW);
}


void createRegisterDialog() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(REGISTRO_SCREEN), NULL, fRegistroDialog);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}

void createEnviosMisEnvios() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(ENVIOS_MIS_ENVIOS), NULL, fEnviosMisEnvios);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}



//Callbacks de los dialogos


BOOL CALLBACK fLoginDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

	case WM_INITDIALOG: {
		//Cargar usuarios
		loadUsers();
	}break;

	case WM_COMMAND:
	{


		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
			case BTN_LOGIN:
				handleLoginClickButton(hwnd);
				break;
			case BTN_LOGIN_SALIR:
				isExitLogin = true;
				cerrarVentana(hwnd);
				break;
			case BTN_REGISTRARSE:
				handleRegisterClickButton(hwnd);
				break;
			}
		}

	}
	break;
	case WM_CLOSE:
		isExitLogin = true;
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (isExitLogin) {
			freeMemory();
			PostQuitMessage(117);
		}
		break;
	}

	return false;
}


BOOL CALLBACK fRegistroDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

	case WM_COMMAND:
	{
		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
				case BTN_REGISTER_GO_BACK:
					createLoginDialog();
					cerrarVentana(hwnd);
				break;
				case BTN_REGISTER_SUBMIT:
					handleRegistrarUsuario(hwnd);
					break;
			}
		}


	}
	break;
	case WM_CLOSE:
		//closeRegister = true;
		//DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if(closeRegister) PostQuitMessage(2);
		break;
	}

	return false;
}



BOOL CALLBACK fInfoVendedorDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{
	case WM_COMMAND:
	{
	}
	break;
	case WM_CLOSE:
		//closeVendedorInfo = true;
		//DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (closeRegister) PostQuitMessage(2);
		break;
	}
	return false;
}

BOOL CALLBACK fEnviosMisEnvios(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{
	case WM_COMMAND:
	{
	}
	break;
	case WM_CLOSE:
		//closeVendedorInfo = true;
		//DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (closeRegister) PostQuitMessage(2);
		break;
	}
	return false;
}




//Mensajes

void mostrarMensaje(HWND hwnd, LPCSTR mensaje) {
	
	MessageBox(hwnd, mensaje,"Mensaje", MB_HELP);
}

//Helpers

void setEnable(HWND hwnd, UINT id, bool isEnable) {
	EnableWindow(GetDlgItem(hwnd, id), isEnable);
}



void cerrarVentana(HWND hwnd) {
	DestroyWindow(hwnd);
}

string getText(int ID_ELEMENT, HWND window) {
	HWND hwnd = GetDlgItem(window, ID_ELEMENT);
	int textLength = GetWindowTextLength(hwnd);
	if (textLength < 1)
		return "";
	char text[50];
	GetWindowText(hwnd, text, ++textLength);
	string sText(text);
	return sText;
}


void freeMemory() {
	if (oUser != NULL) {
		while (aUser != NULL) {
			User* temp = aUser;
			aUser = aUser->next;
			delete temp;
		}
		aUser = oUser = NULL;
	}
	else {
		return;
	}
}


//Notas

//Mes agregar 1 
// Anio agregar 1900

//Clase 25-03 --> Se ve el manejo del tiempo  01hr:00min


//Usuarios


void loadUsers() {
	archive.open("Usuarios.bin", ios::binary | ios::in | ios::ate);
	if (archive.is_open()) {
		int totalChar = archive.tellg();
		if (totalChar < 1) {
			return;
		}
		for (int i = 0; i < totalChar / sizeof(User); i++) {
			if (oUser == NULL) {
				User* temp = new User;
				oUser = new User;
				archive.seekg(i * sizeof(User));
				archive.read(reinterpret_cast<char*>(temp), sizeof(User));
				oUser->password.append(temp->password);
				oUser->name.append(temp->name);
				oUser->username.append(temp->username);
				oUser->userId = temp->userId;
				oUser->prev = NULL;
				oUser->next = NULL;
				aUser = oUser;
				delete reinterpret_cast<char*>(temp);
				continue;
			}
			else {
				while (aUser->next != NULL)
					aUser = aUser->next;
				User* temp = new User;
				aUser->next = new User;
				archive.seekg(i * sizeof(User));
				archive.read(reinterpret_cast<char*>(temp), sizeof(User));
				aUser->next->prev = aUser;
				aUser = aUser->next;
				aUser->next = NULL;
				aUser->password.append(temp->password);
				aUser->name.append(temp->name);
				aUser->username.append(temp->username);
				aUser->userId = temp->userId;
				aUser = oUser;
				delete reinterpret_cast<char*>(temp);
				continue;
			}
		}
		MessageBox(NULL, "Carga de usuarios exitosamente", "Exito", MB_ICONINFORMATION);
		archive.close();
		return;
	}
	else {
		MessageBox(NULL, "No se pudo abrir el archivo", "ERROR", MB_ICONERROR);
		return;
	}
}


void handleRegistrarUsuario(HWND hwnd) {
	string password = getText(TXT_R_PASSWORD, hwnd);
	string username = getText(TXT_R_USERNAME, hwnd);
	string name = getText(TXT_R_NOMBRE, hwnd);

	if (password.compare("") == 0 || username.compare("") == 0 || name.compare("") == 0) {
		MessageBox(NULL, "Ingrese los datos requeridos", "ERROR", MB_ICONERROR);
		return;
	}

	if (oUser != NULL) { //Ya existe por lo menos 1 usuario
		bool found = true;
		while (aUser->username.compare(username) != 0) {
			if (aUser->next == NULL) {
				found = false;
				break;
			}
			aUser = aUser->next;
		}
		aUser = oUser;
		if (found) {
			MessageBox(NULL, "Usuario repetido", "ERROR", MB_ICONERROR);
			return;
		}
	}

	if (oUser == NULL) { //La primera vez que registramos un Usuario
		oUser = new User;
		oUser->password.append(password);
		oUser->name.append(name);
		oUser->username.append(username);
		oUser->userId = GLOBAL_USER_ID++;
		oUser->prev = NULL;
		oUser->next = NULL;
	}
	else { //Tenemos mas de un usuario
		while (aUser->next != NULL)
			aUser = aUser->next;
		aUser->next = new User;
		aUser->next->prev = aUser;
		aUser = aUser->next;
		aUser->next = NULL;
		aUser->userId = GLOBAL_USER_ID++;
		aUser->password.append(password);
		aUser->name.append(name);
		aUser->username.append(username);
	}
	saveUser(oUser);
	saveUserId();
	freeMemory();
	aUser = oUser;
	createLoginDialog();
	DestroyWindow(hwnd);
}

void saveUserId() {
	archive.open("GlobalIdUser.txt", ios::out | ios::trunc);
	if (archive.is_open()) {
		archive << GLOBAL_USER_ID;
		MessageBox(NULL, "ID de Usuario Global Guardado exitosamente", "Exito", MB_ICONINFORMATION);
		archive.close();
		return;
	}
	else {
		MessageBox(NULL, "No se pudo abrir el archivo", "ERROR", MB_ICONERROR);
		return;
	}
}

void saveUser(User* origin) {
	archive.open("Usuarios.bin", ios::binary | ios::out | ios::trunc);
	if (archive.is_open()) {
		while (origin != NULL) {
			archive.write(reinterpret_cast<char*>(origin), sizeof(User));
			origin = origin->next;
		}
		MessageBox(NULL, "Usuarios guardados exitosamente", "Exito", MB_ICONINFORMATION);
		archive.close();
		return;
	}
	else {
		MessageBox(NULL, "No se pudo abrir el archivo", "ERROR", MB_ICONERROR);
		return;
	}
}
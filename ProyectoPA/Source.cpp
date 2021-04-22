#include <Windows.h>
#include "resource.h"

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

bool closeLogin = false;
bool closeRegister = false;
bool closeVendedorInfo = false;


HINSTANCE hGlobalInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	
	hGlobalInstance = hInstance;

	//Primera ventana
	//createLoginDialog();
	//createRegisterDialog();
	createEnviosMisEnvios();
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;
}



void handleLoginClickButton(HWND hwnd) {

	char nombre[50];
	HWND hTxtNombre = GetDlgItem(hwnd, TXT_NOMBRE);
	int iTxtLength = GetWindowTextLength(hTxtNombre);

	if (iTxtLength == 0) {
		MessageBox(hwnd, "Ingrese nombre", "Valor vac�o", MB_ICONEXCLAMATION);

	}
	GetWindowText(hTxtNombre, nombre, ++iTxtLength);
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

	case WM_COMMAND:
	{


		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
			case BTN_LOGIN:
				handleLoginClickButton(hwnd);
				break;
			case BTN_LOGIN_SALIR:
				closeLogin = true;
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
		//closeLogin = true;
		//DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (closeLogin) PostQuitMessage(1);
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


//Notas

//Mes agregar 1 
// Anio agregar 1900

//Clase 25-03 --> Se ve el manejo del tiempo  01hr:00min
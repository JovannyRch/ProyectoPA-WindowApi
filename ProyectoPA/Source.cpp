#include <Windows.h>
#include "resource.h"


BOOL CALLBACK fVentanaPrincipal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fRegisterWindow(HWND, UINT, WPARAM, LPARAM);

void handleLoginClickButton(HWND);
void cerrarVentana(HWND);
void setEnable(HWND, UINT, bool);
void handleRegisterClickButton(HWND);
void createRegisterWindow();

HINSTANCE hGlobalInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	
	hGlobalInstance = hInstance;

	HWND hVentanaPrincipal = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, fVentanaPrincipal);
	ShowWindow(hVentanaPrincipal, cShow);
	
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}


	return 1;
}

BOOL CALLBACK fVentanaPrincipal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(11);
			break;
	}

	return false;
}


void handleLoginClickButton(HWND hwnd) {

	char nombre[50];
	HWND hTxtNombre = GetDlgItem(hwnd, TXT_NOMBRE);
	int iTxtLength = GetWindowTextLength(hTxtNombre);

	if (iTxtLength == 0) {
		MessageBox(hwnd, "Ingrese nombre", "Valor vacío", MB_ICONEXCLAMATION);

	}
	GetWindowText(hTxtNombre, nombre, ++iTxtLength);
	MessageBox(hwnd, nombre, "titulo", MB_HELP);

}


void setEnable(HWND hwnd, UINT id, bool isEnable) {
	EnableWindow(GetDlgItem(hwnd, id), isEnable);
}


void handleRegisterClickButton(HWND hwnd) {
	ShowWindow(hwnd, SW_HIDE);
	createRegisterWindow();
}


void createRegisterWindow() {
	HWND registerwindow = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, fRegisterWindow);
	ShowWindow(registerwindow, SW_SHOW);
}

BOOL CALLBACK fRegisterWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

	case WM_COMMAND:
	{



	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(11);
		break;
	}

	return false;
}

void cerrarVentana(HWND hwnd) {

}
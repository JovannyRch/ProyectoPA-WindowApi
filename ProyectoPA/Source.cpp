#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include "resource.h"

using namespace std;

#define USERS_FILE "Usuarios.bin"
#define PRODUCT_FILE "Productos.bin"
#define GLOBAL_USER_ID_FILE "GlobalIdUser.txt"
#define GLOBAL_PRODUCT_ID_FILE "GlobalIdProduct.txt"

struct User {
	int userId;
	string username;
	string password;
	string name;
	User* prev;
	User* next;
}*oUser, * aUser, * logged;

struct Product {
	int productId;
	int stock;
	string name;
	int code;
	string brand;
	string description;
	float price;
	int userId;
	Product* prev;
	Product* next;
} *oProduct, *aProduct;

int GLOBAL_USER_ID = 1;
int GLOBAL_PRODUCT_ID = 1;
fstream archive;

//Protipos de los callbacks
BOOL CALLBACK fLoginDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fRegistroDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fInfoVendedorDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fEnviosMisEnvios(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosAltas(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosLista(HWND, UINT, WPARAM, LPARAM);

void handleLoginClickButton(HWND);
void cerrarVentana(HWND);
void setEnable(HWND, UINT, bool);
void handleRegisterClickButton(HWND);

//Creacion de las ventanas
void createRegisterDialog();
void createLoginDialog();
void createModalDialog(HWND);
void createVendedorInfoDialog();
void createEnviosMisEnvios();
void createProductosAltas();
void createProductosLista();


// Alertas
void mostrarMensaje(HWND, LPCSTR);


//Banderas

bool isExitLogin = false;
bool isExitRegister = false;
bool isInfoVendedorExit = false;

bool isExitProductosAlta = false;
bool isExitProductosBaja = false;
bool isExitProductosCambios = false;
bool isExitProductosLista = false;

bool isExitEnviosAlta = false;
bool isExitEnviosBajas = false;
bool isExitEnviosLista = false;
bool isExitEnviosCambios = false;


//Helpers
string getText(int, HWND);
void freeMemory();
void saveGlobalId(string, int);
bool isEmpty(string);
bool isNotEmpty(string);


//Usuarios
void loadUsers();
void saveUserId();
void loadUserId();
void saveUser(User*);

// Handlers Register Dialog
void handleRegistrarUsuario(HWND);

//Productos
void registrarProducto(HWND);
void loadProducts();
void saveProduct(Product *);
void loadProductId();

//Menu
void handleMenu(UINT, HWND);

HINSTANCE hGlobalInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdLine, int cShow) {
	
	
	hGlobalInstance = hInstance;
	oProduct = aProduct = NULL;
	oUser = aUser = logged = NULL;
	loadUserId();
	loadProductId();

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
			createProductosLista();
			//createProductosAltas();	
			//createEnviosMisEnvios();
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

void createProductosAltas() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(PRODUCTOS_ALTAS), NULL, fProductosAltas);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}

void createProductosLista() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(PRODUCTOS_LISTA), NULL, fProductosLista);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}



//Callbacks de los dialogos



BOOL CALLBACK fProductosLista(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

		case WM_INITDIALOG: {

			loadProducts();
			HWND hLbProducts = GetDlgItem(hwnd, LB_PRODUCTOS);
			int index = 0;
			while (aProduct != NULL) {
				SendMessage(hLbProducts, LB_ADDSTRING, NULL, (LPARAM)aProduct->name.c_str());
				SendMessage(hLbProducts, LB_SETITEMDATA, (WPARAM)index, (LPARAM)aProduct->productId);
				aUser = aUser->next;
				index++;
			}
			aProduct = oProduct;
		}break;
		case WM_COMMAND:
		{
			if (HIWORD(wparam) == BN_CLICKED) {
				switch (LOWORD(wparam))
				{
					default:
						handleMenu(LOWORD(wparam), hwnd);
						isExitProductosLista = false;
						break;
				}
			}
		}break;
	}
	return false;
}



BOOL CALLBACK fProductosAltas(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

			case WM_INITDIALOG: {
				loadProducts();
			}break;
			case WM_COMMAND:
			{
				if (HIWORD(wparam) == BN_CLICKED) {
					switch (LOWORD(wparam))
					{
						case BTN_PRODUCTOS_ALTAS_SUBMIT:
							registrarProducto(hwnd);
						break;
						default:
							handleMenu(LOWORD(wparam), hwnd);
							isExitProductosAlta = false;
							break;
					}
					
				}
			}break;
			case WM_CLOSE:
				isExitProductosAlta = true;
				DestroyWindow(hwnd);
				break;
			case WM_DESTROY:
				if (isExitProductosAlta) {
					PostQuitMessage(117);
				}
				break;
	}
	return false;
}

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
					cerrarVentana(hwnd);
					createLoginDialog();
				break;
				case BTN_REGISTER_SUBMIT:
					handleRegistrarUsuario(hwnd);
					break;
				default:
					handleMenu(LOWORD(wparam), hwnd);
					isExitRegister = false;
					break;
			}
		}


	}
	break;
	case WM_CLOSE:
		isExitRegister = true;
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (isExitRegister) {
			PostQuitMessage(2);
		}
		break;
	}

	return false;
}



BOOL CALLBACK fInfoVendedorDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{
	case WM_COMMAND:
	{
		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
			default:
				handleMenu(LOWORD(wparam), hwnd);
				isInfoVendedorExit = false;
				break;
			}
		}
	}
	break;
	case WM_CLOSE:
		isInfoVendedorExit = true;
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (isInfoVendedorExit) PostQuitMessage(2);
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
		isExitEnviosLista = true;
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if (isExitEnviosLista) PostQuitMessage(2);
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
	archive.open(USERS_FILE, ios::binary | ios::in | ios::ate);
	
	if (archive.is_open()) {
		
		int totalChar = archive.tellg();
		if (totalChar < 1) {
			//MessageBox(NULL, "No hay usuarios Registrados", "Exito", MB_ICONWARNING);
			archive.close();
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
				continue; // Linea sin sentido, si ingresa a la condicion no toma en cuenta el else y se pasa a la siguiente iteracion
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
				continue; // Linea sin sentido, saliendo del else se pasa a la siguiente iteracion
			}
		}
		archive.close();
		//MessageBox(NULL, "Carga de usuarios exitosamente", "Exito", MB_ICONINFORMATION);
	}
	else {
		MessageBox(NULL, "No se pudo abrir el archivo de usuarios", "ERROR", MB_ICONERROR);
	}
}



void loadProducts() {
	archive.open(PRODUCT_FILE, ios::binary | ios::in | ios::ate);

	if (archive.is_open()) {

		int totalChar = archive.tellg();
		if (totalChar < 1) {
			archive.close();
			return;
		}
		for (int i = 0; i < totalChar / sizeof(Product); i++) {
			if (oUser == NULL) {
				Product* temp = new Product;
				oProduct = new Product;
				archive.seekg(i * sizeof(Product));
				archive.read(reinterpret_cast<char*>(temp), sizeof(Product));
				oProduct->productId = temp->productId;
				oProduct->stock = temp->stock;
				oProduct->name.append(temp->name);
				oProduct->code  = temp->code;
				oProduct->brand.append(temp->brand);
				oProduct->description.append(temp->description);
				oProduct->userId = temp->userId;
				oProduct->price = temp->price;
				oProduct->prev = NULL;
				oProduct->next = NULL;
				aProduct = oProduct;
				delete reinterpret_cast<char*>(temp);
			}
			else {
				while (aProduct->next != NULL) {
					aProduct = aProduct->next;
				}
				Product* temp = new Product;
				aProduct->next = new Product;
				archive.seekg(i * sizeof(Product));
				archive.read(reinterpret_cast<char*>(temp), sizeof(Product));
				aProduct->next->prev = aProduct;
				aProduct = aProduct->next;
				aProduct->next = NULL;
				
				aProduct->productId = temp->productId;
				aProduct->stock = temp->stock;
				aProduct->name.append(temp->name);
				aProduct->code = temp->code;
				aProduct->brand.append(temp->brand);
				aProduct->description.append(temp->description);
				aProduct->userId = temp->userId;
				aProduct->price = temp->price;
			
				aProduct = oProduct;
				delete reinterpret_cast<char*>(temp);
			}
		}
		archive.close();
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
		while (aUser->next != NULL) {
			aUser = aUser->next;
		}
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
	archive.open(GLOBAL_USER_ID_FILE, ios::out | ios::trunc);
	if (archive.is_open()) {
		archive << GLOBAL_USER_ID;
		archive.close();
	}
}



void saveUser(User* origin) {
	archive.open(USERS_FILE, ios::binary | ios::out | ios::trunc);
	if (archive.is_open()) {
		while (origin != NULL) {
			archive.write(reinterpret_cast<char*>(origin), sizeof(User));
			origin = origin->next;
		}
		archive.close();
	}
}

void saveProduct(Product* origin) {
	archive.open(USERS_FILE, ios::binary | ios::out | ios::trunc);
	if (archive.is_open()) {
		while (origin != NULL) {
			archive.write(reinterpret_cast<char*>(origin), sizeof(Product));
			origin = origin->next;
		}
		archive.close();
	}
}

void saveGlobalId(string file, int value) {
	archive.open(file, ios::out | ios::trunc);
	if (archive.is_open()) {
		archive << value;
		archive.close();
	}
}

void registrarProducto(HWND hwnd) {
	string nombre = getText(ALTA_PRODUCTO_NOMBRE, hwnd);
	string inventarioForm = getText(ALTA_PRODUCTO_INVENTARIO, hwnd); //Int
	string marca = getText(ALTA_PRODUCTO_INVENTARIO, hwnd);
	string descripcion = getText(ALTA_PRODUCTO_DESCRIPCION, hwnd);
	string montoForm = getText(ALTA_PRODUCTO_MONTO, hwnd); // Float
	string codigoForm = getText(ALTA_PRODUCTO_CODIGO, hwnd);
	
	if (
		isNotEmpty(nombre) &&
		isNotEmpty(inventarioForm) && 
		isNotEmpty(marca) &&
		isNotEmpty(descripcion) && 
		isNotEmpty(montoForm) &&
		isNotEmpty(codigoForm) 
	) {
		if (oProduct != NULL) { //Ya existe por lo menos 1 registro
			bool found = false;
			while (aProduct != NULL) {
				if (aProduct->name.compare(nombre) == 0 && aProduct->userId == logged->userId) {
					found = true;
					break;
				}
				aProduct = aProduct->next;
			}
			aProduct = oProduct;
			if (found) {
				MessageBox(NULL, "Producto repetido, por favor ingrese otro nombre", "ERROR", MB_ICONERROR);
				return;
			}
		}

		if (oProduct == NULL) { //La primera vez que se hace un registro
			oProduct = new Product;
			oProduct->name.append(nombre);
			oProduct->brand.append(marca);
			oProduct->stock = stoi(inventarioForm);
			oProduct->code = stoi(codigoForm);
			oProduct->description.append(descripcion);
			oProduct->price = stof(montoForm);
			oProduct->userId = logged->userId;
			oProduct->productId = GLOBAL_PRODUCT_ID++;
			oProduct->prev = NULL;
			oProduct->next = NULL;
		}
		else { //Tenemos mas de un registro
			while (aProduct->next != NULL) {
				aProduct = aProduct->next;
			}
			aProduct->next = new Product;
			aProduct->next->prev = aProduct;
			aProduct = aProduct->next;
			aProduct->next = NULL;
			aProduct->productId = GLOBAL_PRODUCT_ID++;
			aProduct->name.append(nombre);
			aProduct->brand.append(marca);
			aProduct->stock = stoi(inventarioForm);
			aProduct->code = stoi(codigoForm);
			aProduct->description.append(descripcion);
			aProduct->price = stof(montoForm);
			aProduct->userId = logged->userId;
		}
		saveProduct(oProduct);
		saveGlobalId(GLOBAL_PRODUCT_ID_FILE, GLOBAL_PRODUCT_ID);
		aProduct = oProduct;
		MessageBox(NULL, "Producto guardado con exito", "ERROR", MB_ICONINFORMATION);
		//DestroyWindow(hwnd);
	}
	else {
		MessageBox(NULL, "Datos incompletos", "ERROR", MB_ICONERROR);
	}
}


bool isEmpty(string str) {
	return str.compare("") == 0;
}

bool isNotEmpty(string str) {
	return !isEmpty(str);
}


void loadUserId() {
	archive.open(GLOBAL_USER_ID_FILE, ios::in);
	if (archive.is_open()) {
		archive >> GLOBAL_USER_ID;
		archive.close();
	}
}

void loadProductId() {
	archive.open(GLOBAL_PRODUCT_ID_FILE, ios::in);
	if (archive.is_open()) {
		archive >> GLOBAL_PRODUCT_ID;
		archive.close();
		return;
	}
}


void handleMenu(UINT wparam, HWND hwnd) {
	switch (wparam)
	{
		
		case ID_ENVIOS_LISTA:
			createEnviosMisEnvios();
			break;
		case ID_ENVIOS_ALTAS:
			break;
		case ID_ENVIOS_BAJAS:
			break;
		case ID_ENVIOS_CAMBIOS:
			break;
		case ID_PRODUCTOS_ALTAS:
			createProductosAltas();
			break;
		case ID_PRODUCTOS_BAJAS:
			break;
		case ID_PRODUCTOS_VERTODOS:
			createProductosLista();
			break;
		case ID_PRODUCTOS_CAMBIOS:
			
			break;
		case ID_INFO_VENDEDOR:
			createVendedorInfoDialog();
			break;
		case MENU_EXIT:
			exit(3);
			break;
		default:
			MessageBox(NULL, "Default case", "Exito", MB_ICONWARNING);
			break;
	}
	DestroyWindow(hwnd);
}
#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "resource.h"

using namespace std;

#define USERS_FILE "Usuarios.bin"
#define PRODUCT_FILE "Productos.bin"
#define ENVIOS_FILE "Envios.bin"
#define GLOBAL_USER_ID_FILE "GlobalIdUser.txt"
#define GLOBAL_PRODUCT_ID_FILE "GlobalIdProduct.txt"
#define GLOBAL_ENVIO_ID_FILE "GlobalIdEnvio.txt"
#define BAJA 1
#define CAMBIO 2
#define ENVIADO "ENVIADO"
#define PENDIENTE "PENDIENTE DE ENVIO"

struct User {
	int userId;
	string username = "";
	string password = "";
	string fullname = "";
	string alias = "";
	string picture = "";
	User* prev = NULL;
	User* next = NULL;
}*oUser, * aUser, * logged;

struct Product {
	int productId = 0;
	int stock = 0;
	string name = "";
	int code = 0;
	string brand = "";
	string description = "";
	float price = 0.0;
	int userId = 0;
	Product* prev = NULL;
	Product* next = NULL;
} *oProduct, *aProduct, *productoActual;

struct Envio {
	int envioId = 0;
	struct Producto* producto;
	int productoId = 0;
	int cantidad = 0;
	float total = 0.0;
	string colonia = "";
	string ciudad = "";
	string estado = "";
	string mensaje = "";
	string fecha = "";
	string status = PENDIENTE;
};

int GLOBAL_USER_ID = 1;
int GLOBAL_PRODUCT_ID = 1;
int GLOBAL_ENVIO_ID = 1;
fstream archive;

//Protipos de los callbacks
BOOL CALLBACK fLoginDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fRegistroDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fInfoVendedorDialog(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fEnviosMisEnvios(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosAltas(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosLista(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosBajas(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fProductosCambios(HWND, UINT, WPARAM, LPARAM);

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
void createProductosBajas();
void createProductosCambios();
void buscarProdutoPorId(HWND, Product* , int);

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
string StringToUpper(string);
void setText(HWND, int, string);
string FloatToString(float);


//Usuarios
void loadUsers();
void saveUserId();
void loadUserId();
void saveUser(User*);
void updateUser(User*, User*);
void actualizarInformacionVendedor(HWND hwnd);

// Handlers Register Dialog
void handleRegistrarUsuario(HWND);

//Productos
void registrarProducto(HWND);
void loadProducts();
void saveProduct(Product *);
void loadProductId();
void deleteProduct(Product*, int);
void buscarProducto(HWND, Product*, int);
void eliminarProducto(HWND);
void updateProducto(Product*, Product*);
void guardarCambios(HWND);
HWND hLbProducts;

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
	if (isEmpty(password) || isEmpty(username)) {
		MessageBox(NULL, "Ingrese sus credenciales", "ERROR", MB_ICONERROR);
		return;
	}

	if (oUser != NULL) {
		bool isUserFound = false;
		while (aUser != NULL) {
			if (aUser->username.compare(username) == 0 && aUser->password.compare(password) == 0) {
				isUserFound = true;
				logged = aUser;
				break;
			}
				
			aUser = aUser->next;
		}
		if (isUserFound) {
			//if(isEmpty(logged->fullname) || isEmpty(logged->picture) || isEmpty(logged->alias)){
			if(isEmpty(logged->fullname) || isEmpty(logged->alias)){
				MessageBox(NULL, "Complete sus datos para poder continuar", "Información", MB_ICONINFORMATION);
				createVendedorInfoDialog();
			}
			else{
				createEnviosMisEnvios();
			}
			isExitLogin = false;
			DestroyWindow(hwnd);
		}
		else {
			MessageBox(NULL, "Credenciales no aceptadas, revise sus datos", "ERROR", MB_ICONERROR);
		}
		aUser = oUser;
	}
	else {
		MessageBox(NULL, "No se encontraron usuarios registrados", "ERROR", MB_ICONERROR);
	}

}




void handleRegisterClickButton(HWND hwnd) {
	ShowWindow(hwnd, SW_HIDE);
	createRegisterDialog();
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
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(REGISTRO_SCREEN), NULL, fRegistroDialog);
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

void createProductosBajas() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(PRODUCTOS_BAJAS), NULL, fProductosBajas);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}

void createProductosCambios() {
	HMENU menu = LoadMenu(hGlobalInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND ventana = CreateDialog(hGlobalInstance, MAKEINTRESOURCE(PRODUCTOS_CAMBIOS), NULL, fProductosCambios);
	SetMenu(ventana, menu);
	ShowWindow(ventana, SW_SHOW);
}

//Callbacks de los dialogos


/*
BOOL CALLBACK fPrototipo(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

	case WM_INITDIALOG: {
		
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
}*/

BOOL CALLBACK fProductosCambios(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
			case P_CAMBIOS_BUSCAR:
				buscarProducto(hwnd, oProduct, CAMBIO);
				break;
			case P_CAMBIOS_GUARDAR:
				guardarCambios(hwnd);
				break;
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


BOOL CALLBACK fProductosBajas(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
			case BTN_PRODUCTOS_BAJAS_ELIMINAR:
				buscarProducto(hwnd, oProduct, BAJA);
				break;
			case P_BAJAS_ELIMINAR:
				eliminarProducto(hwnd);
				break;
			default:
				handleMenu(LOWORD(wparam), hwnd);
				isExitProductosBaja = false;
				break;
			}
		}
	}break;
	}
	return false;
}

BOOL CALLBACK fProductosLista(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg)
	{

		case WM_INITDIALOG: {
			loadProducts();
			hLbProducts = GetDlgItem(hwnd, LB_PRODUCTOS);
			int index = 0;
			while (aProduct != NULL) {
				SendMessage(hLbProducts, LB_ADDSTRING, NULL, (LPARAM)aProduct->name.c_str());
				SendMessage(hLbProducts, LB_SETITEMDATA, (WPARAM)index, (LPARAM)aProduct->productId);
				aProduct = aProduct->next;
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
			if (HIWORD(wparam) == LBN_SELCHANGE) {
				int index = SendMessage(hLbProducts, LB_GETCURSEL, NULL, NULL);
				int productId = SendMessage(hLbProducts, LB_GETITEMDATA, index, NULL);
				buscarProdutoPorId(hwnd, oProduct, productId);
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
	case WM_INITDIALOG: {
		setText(hwnd, TXT_NOMBRE_VENDEDOR, logged->fullname);
		setText(hwnd, TXT_ALIAS_EMPRESA, logged->alias);
	}break;
	case WM_COMMAND:
	{
		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
				case BTN_GUARDAR_INFO_VENDEDOR:
					actualizarInformacionVendedor(hwnd);
					break;
				default:
					handleMenu(LOWORD(wparam), hwnd);
					isInfoVendedorExit = true;
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
	case WM_INITDIALOG: {
		HWND hLblUserName = GetDlgItem(hwnd, LBL_NOMBRE_USUARIO);
		SetWindowText(hLblUserName, logged->alias.c_str());
	}break;
	case WM_COMMAND:
	{
		if (HIWORD(wparam) == BN_CLICKED) {
			switch (LOWORD(wparam))
			{
			default:
				handleMenu(LOWORD(wparam), hwnd);
				break;
			}
		}
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
	char text[100];
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
			archive.close();
		
			return;
		}
		float totalUsuarios = totalChar / sizeof(User);
		for (int i = 0; i < totalChar / sizeof(User); i++) {
			if (oUser == NULL) {
				User* temp = new User;
				oUser = new User;
				archive.seekg(i * sizeof(User));
				archive.read(reinterpret_cast<char*>(temp), sizeof(User));
				oUser->password.assign(temp->password);
				oUser->fullname.assign(temp->fullname);
				oUser->username.assign(temp->username);
				oUser->alias.assign(temp->alias);
				oUser->userId = temp->userId;
				oUser->prev = NULL;
				oUser->next = NULL;
				aUser = oUser;
				delete reinterpret_cast<char*>(temp);
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
				aUser->password.assign(temp->password);
				aUser->fullname.assign(temp->fullname);
				aUser->username.assign(temp->username);
				aUser->alias.assign(temp->alias);
				aUser->userId = temp->userId;
				aUser = oUser;
				delete reinterpret_cast<char*>(temp);
			}
		}
		archive.close();
	}
	else {
		MessageBox(NULL, "No se pudo abrir el archivo de usuarios", "ERROR", MB_ICONERROR);

	}
	archive.close();
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

	if (isEmpty(password) || isEmpty(username)) {
		MessageBox(NULL, "Ingrese los datos requeridos", "ERROR", MB_ICONERROR);
		return;
	}

	if (oUser != NULL) { //Ya existe por lo menos 1 usuario
		bool found = false;
		while (aUser != NULL) {
			if (aUser->username.compare(username) == 0) {
				found = true;
				break;
			}
			aUser = aUser->next;
		}
		aUser = oUser;
		if (found) {
			MessageBox(NULL, "Nombre de usuario no disponible, intente con otro", "ERROR", MB_ICONERROR);
			return;
		}
	}

	if (oUser == NULL) { //La primera vez que registramos un Usuario
		oUser = new User;
		oUser->password.append(password);
		oUser->fullname.append("");
		oUser->alias.append("");
		oUser->picture.append("");
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
		aUser->fullname.append("");
		aUser->alias.append("");
		aUser->picture.append("");
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
	archive.open(PRODUCT_FILE, ios::binary | ios::out | ios::trunc);
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
	string marca = getText(ALTA_PRODUCTO_MARCA, hwnd);
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
	}
}


void handleMenu(UINT wparam, HWND hwnd) {
	freeMemory();
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
			createProductosBajas();
			break;
		case ID_PRODUCTOS_VERTODOS:
			createProductosLista();
			break;
		case ID_PRODUCTOS_CAMBIOS:
			createProductosCambios();
			break;
		case ID_INFO_VENDEDOR:
			createVendedorInfoDialog();
			break;
		case MENU_EXIT:
			exit(3);
			PostQuitMessage(120);
			break;
		default:
			//MessageBox(NULL, "Default case", "Exito", MB_ICONWARNING);
			break;
	}
	DestroyWindow(hwnd);
}

void actualizarInformacionVendedor(HWND hwnd) {
	string nombreCompleto = getText(TXT_NOMBRE_VENDEDOR, hwnd);
	string alias = getText(TXT_ALIAS_EMPRESA, hwnd);


	if (isEmpty(nombreCompleto) || isEmpty(alias)) {
		MessageBox(NULL, "Datos incompletos", "ERROR", MB_ICONERROR);
		return;
	}

	logged->fullname = nombreCompleto;
	logged->alias = StringToUpper(alias);
	updateUser(oUser, logged);
	DestroyWindow(hwnd);
	createEnviosMisEnvios();
}

void updateUser(User* origin, User* item) {
	User* aux;
	aux = origin;

	while (aux != NULL) {
		if (aux->userId == item->userId) {
			aux->fullname = item->fullname;
			aux->alias = item->alias;
			break;
		}
		aux = aux->next;
	}


	while (aux != NULL && aux->prev != NULL) {
		aux = aux->prev;
	}
	saveUser(aux);
}


void updateProducto(Product* origin, Product* item) {
	Product* aux;
	aux = origin;

	while (aux != NULL) {
		if (aux->productId == item->productId) {
			aux->description = item->description;
			aux->code = item->code;
			aux->brand = item->brand;
			aux->price = item->price;
			aux->stock = item->stock;
			break;
		}
		aux = aux->next;
	}


	while (aux != NULL && aux->prev != NULL) {
		aux = aux->prev;
	}
	saveProduct(aux);
}

void deleteProduct(Product *origin, int id) {
	
	Product* last = NULL;
	while (origin != NULL) {
		if (origin->productId == id) {
			Product* next = origin->next;
			Product* prev = origin->prev;
			prev->next = next;
			next->prev = prev;
			last = prev;
			MessageBox(NULL, "Producto eliminado con exito", "Mensaje", MB_ICONINFORMATION);
			break;
		}
		origin = origin->next;
	}
	int i = 0;
	while (last != NULL && last->prev != NULL) {
		last = last->prev;
	}

	

	saveProduct(last);
	loadProducts();
}


string StringToUpper(string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);
	return strToConvert;
}

void buscarProducto(HWND hwnd, Product * origin, int tipo) {
	string nombre = "";

	if (tipo == BAJA) {
		nombre = getText(PRODUCTOS_BAJAS_NOMBRE, hwnd);
	}
	else {
		nombre = getText(P_CAMBIOS_NOMBRE, hwnd);
	}

	if (isEmpty(nombre)) {
		MessageBox(NULL, "Ingrese nombre del producto", "ERROR", MB_ICONERROR);
		return;
	}

	productoActual = NULL;

	while (origin != NULL) {
		if (StringToUpper(origin->name).compare(StringToUpper(nombre)) == 0) {
			productoActual = origin;
			break;
		}
		origin = origin->next;
	}
	if (productoActual == NULL) {
		MessageBox(NULL, "Producto no encontrado", "Mensaje", MB_ICONINFORMATION);
		return;
	}

	//P_BAJAS_CANTIDAD
	if (tipo == BAJA) {
		setText(hwnd, P_BAJAS_CANTIDAD, FloatToString(productoActual->stock));
		setText(hwnd, P_BAJAS_CODIGO, FloatToString(productoActual->code));
		setText(hwnd, P_BAJAS_MONTO, FloatToString(productoActual->price));
		setText(hwnd, P_BAJAS_DESCRIPCION, productoActual->description);
		setText(hwnd, P_BAJAS_MARCA, productoActual->brand);
	}
	else {
		setText(hwnd, P_CAMBIOS_CANTIDAD, FloatToString(productoActual->stock));
		setText(hwnd, P_CAMBIOS_CODIGO, FloatToString(productoActual->code));
		setText(hwnd, P_CAMBIOS_MONTO, FloatToString(productoActual->price));
		setText(hwnd, P_CAMBIOS_DESCRIPCION, productoActual->description);
		setText(hwnd, P_CAMBIOS_MARCA, productoActual->brand);
	}

}



void eliminarProducto(HWND hwnd) {
	
	if (productoActual == NULL) {
		MessageBox(NULL, "Busque un producto para eliminar", "Mensaje", MB_ICONHAND);
	}
	else {
		int productId = productoActual->productId;
		deleteProduct(oProduct, productId);
	}
}

void setText(HWND hwnd, int id, string value) {
	HWND dialog = GetDlgItem(hwnd, id);
	SetWindowText(dialog, value.c_str());
}


string FloatToString(float number) {
	std::ostringstream buff;
	buff << number;
	return buff.str();
}

void guardarCambios(HWND hwnd) {
	string nombre = getText(P_CAMBIOS_NOMBRE, hwnd);
	string codigoForm = getText(P_CAMBIOS_CODIGO, hwnd);
	string montoForm = getText(P_CAMBIOS_MONTO, hwnd);
	string descripcion = getText(P_CAMBIOS_DESCRIPCION, hwnd);
	string marca = getText(P_CAMBIOS_MARCA, hwnd);


	if ( isEmpty(codigoForm) || isEmpty(montoForm) || isEmpty(descripcion) || isEmpty(marca))  {
		MessageBox(NULL, "Datos incompletos", "ERROR", MB_ICONERROR);
		return;
	}
	productoActual->name = nombre;
	productoActual->code = stoi(codigoForm);
	productoActual->price = stof(montoForm);
	productoActual->description = descripcion;
	productoActual->brand = marca;

	updateProducto(oProduct, productoActual);
	loadProducts();
	MessageBox(NULL, "Producto actualizado con exito", "Mensaje", MB_ICONINFORMATION);
}


void buscarProdutoPorId(HWND hwnd, Product* origin, int productId) {
	
	productoActual = NULL;
	while (origin != NULL) {
		if (productId == origin->productId) {
			productoActual = origin;
			break;
		}
		origin = origin->next;
	}
	if (productoActual != NULL) {
		setText(hwnd, P_LISTA_NOMBRE, productoActual->name);
		setText(hwnd, P_LISTA_CANTIDAD, FloatToString(productoActual->stock));
		setText(hwnd, P_LISTA_CODIGO, FloatToString(productoActual->code));
		setText(hwnd, P_LISTA_DESCRIPCION, productoActual->description);
		setText(hwnd, P_LISTA_MARCA, productoActual->brand);
		setText(hwnd, P_LISTA_MONTO, FloatToString(productoActual->price));
	}
}
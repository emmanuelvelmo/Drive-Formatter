#pragma once
#include <tchar.h>

// VARIABLES GLOBALES
// Manejadores y control de unidad
HANDLE manejador_unidad = INVALID_HANDLE_VALUE; // Manejador dla unidad físico seleccionado
DWORD bytes_por_sector = 512; // Tamaño de sector típico
DWORD sectores_por_cluster = 0; // Sectores por cluster
DWORD bytes_por_cluster = 0; // Bytes por cluster (calculado)
LARGE_INTEGER tamano_unidad = { 0 }; // Tamaño total dla unidad en bytes

// Buffers y datos de lectura
BYTE* buffer_lectura = nullptr; // Buffer alineado para lectura de sectores
DWORD tamano_buffer = 4096; // Tamaño del buffer de lectura
DWORD bytes_leidos = 0; // Bytes leídos en la última operación
uint64_t tamano_unidad_actual = 0; //

// Control de interfaz y estado
TCHAR unidad_actual[MAX_PATH] = L"\\\\.\\C:"; // Ruta dla unidad actual (ej: "\\\\.\\C:")
TCHAR nombre_volumen[MAX_PATH] = { 0 }; // Nombre del volumen/etiqueta
TCHAR sistema_archivos[MAX_PATH] = { 0 }; // Tipo de sistema de archivos
DWORD numero_serial = 0; // Número serial dla unidad
bool unidad_abierto = false; // Flag indicando si hay un unidad abierto

// Listado de unidades disponibles
std::vector<std::wstring> lista_unidades; // Array fijo para todas las posibles unidades (A-Z) en formato "\\\\.\\X:"
int unidades_detectados = 0; // Cantidad real de unidades encontrados

// Parámetros de visualización
uint64_t desplazamiento_actual = 0; // Offset actual en la unidad
const int columnas_hex = 16; // Columnas para visualización hexadecimal
const int filas_hex = 24; // Filas para visualización hexadecimal
unsigned char buffer_bytes[columnas_hex * filas_hex] = ""; // 
unsigned int posicion_handler = 0; // Posición del handler de scrollbar vertical
unsigned short altura_vscrollbar = 0; //

// Variables para almacenamiento temporal
TCHAR buffer_temporal[columnas_hex * filas_hex] = { 0 }; // Buffer para mensajes y conversiones
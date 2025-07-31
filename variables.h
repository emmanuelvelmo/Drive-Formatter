#pragma once // Prevenir inclusi�n m�ltiple del archivo de cabecera
#include <tchar.h> // Para soporte de caracteres anchos (Unicode/ANSI)

// VARIABLES GLOBALES
// Manejadores y control de dispositivo
HANDLE manejador_unidad = INVALID_HANDLE_VALUE; // Manejador de la unidad f�sica seleccionada
TCHAR unidad_actual[MAX_PATH] = L"\\\\.\\C:"; // Ruta completa a la unidad de disco actual
std::vector<std::wstring> lista_unidades; // Lista de todas las unidades disponibles en el sistema

// Propiedades del almacenamiento
uint64_t tamano_unidad_actual = 0; // Tama�o total de la unidad en bytes
unsigned int cantidad_sectores = 0; // Cantidad total de sectores de la unidad
uint64_t posicion_unidad = 0; // Posici�n actual de lectura/escritura en la unidad
uint64_t tamano_segmento = 0; // Tama�o de segmento para operaciones paralelas

// Buffers y operaciones
void* buffer_lectura = _aligned_malloc(512, 512); // Buffer alineado para operaciones de disco
DWORD bytes_leidos = 0; // Cantidad de bytes le�dos en la �ltima operaci�n
std::atomic<uint64_t> suma_bytes{ 0 }; // Contador de bytes procesados durante formateo

// Interfaz de usuario
float proporcion_scrollbar = 0; // Posici�n actual del scrollbar (valor normalizado)
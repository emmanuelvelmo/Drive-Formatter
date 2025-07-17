#include <windows.h> // Necesario para HINSTANCE, LPSTR, etc.
#include <tchar.h> // Para manejar caracteres TCHAR (compatibilidad Unicode/ANSI)
#include <stdio.h> // Para funciones de entrada/salida estándar (printf)
#include <iomanip> //
#include <malloc.h> //
#include "ventana_principal.h" //

using namespace System;

// El atributo STAThread debe estar antes de la declaración de WinMain
[STAThread]

// VARIABLES




// FUNCIONES
// Función para mostrar 256 bytes o menos en caja de texto



// Función para calcular posición en disco a partir del handler de scrollbar vertical
uint64_t posicion_byte_disco()
{

}

// Función para 
DWORD lista_drives()
{
    // Obtener un bitmask de las unidades de disco disponibles
    // Cada bit representa una letra de unidad (A-Z), donde 1 = presente, 0 = ausente
    DWORD drives = GetLogicalDrives();

    // Mostrar encabezado
    printf("Unidades de disco disponibles:\n");

    // Recorrer todas las letras posibles de unidades (A-Z)
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        // Verificar si el bit correspondiente a esta letra está activado (1)
        // (1 << (letter - 'A')) desplaza el bit 1 a la posición de la letra actual
        if (drives & (1 << (letter - 'A')))
        {
            // Crear la ruta raíz de la unidad (ej: "C:\")
            TCHAR rootPath[] = { letter, ':', '\\', '\0' };

            // Buffer para almacenar el nombre del volumen (etiqueta del disco)
            TCHAR volumeName[MAX_PATH + 1] = { 0 }; // Inicializado a ceros

            // Obtener información del volumen (disco)
            if (GetVolumeInformation(
                rootPath, // Ruta raíz de la unidad
                volumeName, // Buffer para el nombre del volumen
                MAX_PATH, // Tamaño del buffer
                NULL, // No necesitamos el número de serie
                NULL, // No necesitamos la longitud máxima de nombres
                NULL, // No necesitamos flags del sistema de archivos
                NULL, // No necesitamos el tipo de sistema de archivos
                0))
            { // Flags (0 para ninguno)

                // Si se obtuvo la información, mostrar letra y nombre del volumen
                printf("%c: (%s)\n", letter, volumeName);
            }
            else
            {
                // Si no se pudo obtener información, mostrar solo la letra
                printf("%c: (Sin nombre de volumen)\n", letter);
            }
        }


}

// Función para
void lectura_disco()
{
    private:
        HANDLE manejador_disco; // Manejador para acceder al dispositivo físico
        DWORD tam_sector; // Tamaño del sector en bytes (usualmente 512)

    public:
        // Constructor inicializa manejador inválido y sector tamaño por defecto
        lector_disco() : manejador_disco(INVALID_HANDLE_VALUE), tam_sector(512) {}

        // Destructor cierra el manejador si está abierto
        ~lector_disco()
        {
            if (manejador_disco != INVALID_HANDLE_VALUE)
            {
                CloseHandle(manejador_disco); // Cierra el handle abierto
            }
        }

        // Abre el disco en modo lectura sin buffer y con acceso compartido total
        bool abrir(const wchar_t* ruta)
        {
            manejador_disco = CreateFileW(
                ruta, // Ruta al dispositivo (ej. "\\\\.\\D:")
                GENERIC_READ, // Solo lectura
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
                nullptr, // Sin atributos de seguridad especiales
                OPEN_EXISTING, // Abrir solo si existe
                FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, // Acceso sin cache, secuencial
                nullptr // Sin plantilla
            );

            if (manejador_disco == INVALID_HANDLE_VALUE) return false; // Si falla abrir, retornar falso

            tam_sector = 512; // Asignar tamaño sector fijo (mejorable)

            return true; // Éxito en apertura
        }

        // Lee datos sin procesar desde offset (en bytes) directamente al buffer
        bool leer_bruto(uint64_t offset, void* buffer, size_t tam)
        {
            LARGE_INTEGER posicion; // Variable para el desplazamiento de lectura
            posicion.QuadPart = offset; // Establece el offset de lectura en bytes

            // Mueve el puntero de archivo al offset especificado
            SetFilePointer(manejador_disco, posicion.LowPart, &posicion.HighPart, FILE_BEGIN);

            DWORD bytes_leidos;
            // Lee tamaño 'tam' bytes desde disco en buffer, devuelve true si leyó exactamente 'tam'
            return ReadFile(manejador_disco, buffer, static_cast<DWORD>(tam), &bytes_leidos, nullptr) && bytes_leidos == tam;
        }

        // Muestra en pantalla un volcado hexadecimal en filas x columnas desde offset
        void mostrar_volcado_hex(uint64_t offset, size_t filas, size_t columnas)
        {
            size_t tam = filas * columnas; // Total bytes a mostrar
            // Calcula tamaño alineado al tamaño del sector para lectura sin buffer
            size_t tam_alineado = ((tam + tam_sector - 1) / tam_sector) * tam_sector;

            // Reserva buffer alineado a sector (requisito para lectura sin buffering)
            void* buffer = _aligned_malloc(tam_alineado, tam_sector);

            // Lee datos del disco desde offset al buffer alineado
            leer_bruto(offset, buffer, tam_alineado);

            uint8_t* datos = static_cast<uint8_t*>(buffer);

            // Recorre filas
            for (size_t fila = 0; fila < filas; ++fila)
            {
                // Recorre columnas
                for (size_t col = 0; col < columnas; ++col)
                {
                    // Imprime byte en hex con formato 2 dígitos, rellenando con 0 si necesario
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)datos[fila * columnas + col] << " ";
                }

                std::cout << "\n"; // Salto de línea al terminar fila
            }
            _aligned_free(buffer); // Libera el buffer alineado
        }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //
    System::Windows::Forms::Application::EnableVisualStyles();
    //
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

    // Crear y ejecutar el formulario principal
    DriveFormatter::ventana_principal^ form = gcnew DriveFormatter::ventana_principal();
    //
    System::Windows::Forms::Application::Run(form);















    return 0;
}
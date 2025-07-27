#include <tchar.h> // Para manejar caracteres TCHAR (compatibilidad Unicode/ANSI)
#include <stdio.h> // Para funciones de entrada/salida est�ndar (printf)
#include <iomanip> //
#include <vector> //
#include <cmath> //
#include <malloc.h> //
#include "ventana_principal.h" //
#include "variables.h" //

namespace DriveFormatter
{
    // FUNCIONES
    // Funci�n para obtener 384 bytes o menos desde la unidad (seg�n la posici�n del scrollbar vertical)
    void ventana_principal::leer_bytes_unidad()
    {
        // 
        LARGE_INTEGER pos_val;

        pos_val.QuadPart = posicion_unidad; // 

        // Posici�n inicial de la unidad a leer
        SetFilePointerEx(manejador_unidad, // Manejador del archivo o disco
            pos_val, // Desplazamiento (offset) en bytes
            nullptr, // Devuelve la nueva posici�n (puede ser nullptr)
            FILE_BEGIN // C�mo interpretar el offset (inicio, actual, final)
        );

        // Realizar la lectura
        if (!ReadFile(
            manejador_unidad, // El identificador de la unidad abierta con CreateFile()
            buffer_lectura, // Puntero al buffer donde se guardar�n los datos le�dos
            512, // N�mero de bytes a leer (debe ser m�ltiplo del tama�o de sector)
            &bytes_leidos, // Variable global que almacena cu�ntos bytes se leyeron realmente
            NULL // NULL para operaci�n s�ncrona (si no, usar estructura OVERLAPPED para I/O as�ncrono)
        ))
        {
            // Si ReadFile falla, llenamos el buffer con ceros
            memset(buffer_lectura, 0, 512);
        }
    }

    // Funci�n para mostrar 384 bytes en caja de texto
    void ventana_principal::actualizar_caja_texto(uint64_t posicion_referencia)
    {
        // Establecer la posici�n actual de lectura
        posicion_unidad = posicion_referencia;

        // Obtener 384 bytes o menos desde la unidad (rellenar con ceros para asegurar 2 cifras)
        leer_bytes_unidad();

        // Preparar el texto hexadecimal para mostrar
        System::String^ texto_hex = gcnew System::String(""); // 

        BYTE* buffer = static_cast<BYTE*>(buffer_lectura); // 

        // 
        for (DWORD i = 0; i < 384; ++i)
        {
            texto_hex += System::String::Format("{0:X2} ", buffer[i]);
        }

        // Mostrar el texto en el control EDIT
        textBox1->Text = texto_hex;
    }

    // Funci�n para
    void ventana_principal::actualizar_barra_progreso()
    {
        // Calcular progreso actual


        // Mostrar progreso en barra

    }

    // Funci�n para formatear unidad completa
    void ventana_principal::formatear_unidad()
    {
        // Acceder a unidad actual seleccionada


        // Reemplazar todos los bytes en unidad por 0x00
        //for ()
        //{
            //


            // Actualizar la barar de progreso
            //actualizar_barra_progreso();
        //}

        // Reiniciar barra de progreso a cero


        // Reiniciar a sector 0
        proporcion_scrollbar = 0;

        // Colocar handler al inicio
        vScrollBar1->Value = 0;

        // Actualizar vista hexadecimal
        actualizar_caja_texto(0);
    }

    // Funci�n para calcular posici�n en unidad a partir del handler de scrollbar vertical
    void ventana_principal::posicion_unidad_vscrollbar()
    {
        // Capturar posici�n de handler
        proporcion_scrollbar = static_cast<float>(vScrollBar1->Value) / 100.0f;

        // Calcular byte en la unidad (sector seg�n posici�n del scrollbar vertical)
        uint64_t posicion_calculada = round(proporcion_scrollbar * cantidad_sectores) * 512;

        // Actualizar vista hexadecimal con la unidad actual seleccionado
        actualizar_caja_texto(posicion_calculada);
    }

    // Funci�n para registrar datos de la unidad seleccionada (ej. tama�o en bytes)
    void ventana_principal::datos_unidad_actual()
    {
        // Cerrar manejador si estaba abierto previamente
        if (manejador_unidad != INVALID_HANDLE_VALUE)
        {
            CloseHandle(manejador_unidad);

            manejador_unidad = INVALID_HANDLE_VALUE;
        }

        // Abrir la unidad f�sica en modo lectura
        manejador_unidad = CreateFileW(
            unidad_actual, // Ruta al dispositivo (ej. "\\\\.\\C:")
            GENERIC_READ | GENERIC_WRITE, // Permisos de lectura y escritura
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
            nullptr, // Sin atributos de seguridad
            OPEN_EXISTING, // Abrir solo si existe
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, // Acceso sin cache
            nullptr // Sin plantilla de archivo
        );

        // Obtener tama�o de la unidad (alternativa simple sin winioctl.h)
        LARGE_INTEGER tam_unidad; // 

        GetFileSizeEx(manejador_unidad, &tam_unidad); // 

        tamano_unidad_actual = tam_unidad.QuadPart; // 

        // Calcular el n�mero de sectores en la unidad
        cantidad_sectores = round(tamano_unidad_actual / 512);
    }

    // Actualizar combobox con la lista de unidades disponibles
    void ventana_principal::actualizar_combobox()
    {
        // Limpiar contenido previo del combobox
        comboBox1->Items->Clear();

        // Recorremos s�lo las unidades existentes
        for (const auto& unidad : lista_unidades)
        {
            // Formatear a "X:\" usando el dato del vector
            wchar_t display[10];

            swprintf(display, 10, L"%c:\\", unidad[4]);  // Extrae la letra

            // Agregar al combobox
            comboBox1->Items->Add(gcnew System::String(display));
        }

        // Auto-selecci�n si hay elementos
        comboBox1->SelectedIndex = 0;

        // Obtener tama�o de bytes de unidad
        datos_unidad_actual();

        // Actualizar vista hexadecimal con la unidad actual seleccionado
        actualizar_caja_texto(0);
    }

    // Funci�n para detectar y registrar unidades disponibles
    void ventana_principal::directorios_unidades()
    {
        // Reiniciar a sector 0
        proporcion_scrollbar = 0;

        // Colocar handler al inicio
        vScrollBar1->Value = 0;

        lista_unidades.clear();  // Limpiar el vector

        DWORD drives = GetLogicalDrives(); // 

        for (char letter = 'A'; letter <= 'Z'; ++letter)
        {
            if (drives & (1 << (letter - 'A')))
            {
                // A�adir al vector din�mico
                wchar_t unidad[10]; // 

                swprintf(unidad, 10, L"\\\\.\\%c:", letter); // 

                lista_unidades.push_back(unidad); // 
            }
        }

        // Actualizar combobox
        actualizar_combobox();
    }

    // Funci�n para 
    void ventana_principal::cambiar_unidad()
    {
        // Obtener la ruta completa de la unidad seleccionada desde lista_unidades
        wcscpy_s(unidad_actual, MAX_PATH, lista_unidades[comboBox1->SelectedIndex].c_str());

        // Reiniciar a sector 0
        proporcion_scrollbar = 0;

        // Colocar handler al inicio
        vScrollBar1->Value = 0;

        // Obtener tama�o de bytes de unidad
        datos_unidad_actual();

        // Actualizar vista hexadecimal
        actualizar_caja_texto(0);
    }
}
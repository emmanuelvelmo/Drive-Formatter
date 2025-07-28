#include <tchar.h> // Para manejar caracteres TCHAR (compatibilidad Unicode/ANSI)
#include <stdio.h> // Para funciones de entrada/salida estándar (printf)
#include <iomanip> // 
#include <vector> // 
#include <cmath> // 
#include <malloc.h> //
#include <windows.h> // 
#include "ventana_principal.h" // 
#include "variables.h" // 

namespace DriveFormatter
{
    // FUNCIONES
    // Función para obtener 384 bytes o menos desde la unidad (según la posición del scrollbar vertical)
    void ventana_principal::leer_bytes_unidad()
    {
        // 
        LARGE_INTEGER pos_val; // 

        pos_val.QuadPart = posicion_unidad; // 

        // Posición inicial de la unidad a leer
        SetFilePointerEx(manejador_unidad, // Manejador del archivo o disco
            pos_val, // Desplazamiento (offset) en bytes
            nullptr, // Devuelve la nueva posición (puede ser nullptr)
            FILE_BEGIN // Cómo interpretar el offset (inicio, actual, final)
        );

        // Realizar la lectura
        if (!ReadFile(
            manejador_unidad, // El identificador de la unidad abierta con CreateFile()
            buffer_lectura, // Puntero al buffer donde se guardarán los datos leídos
            512, // Número de bytes a leer (debe ser múltiplo del tamaño de sector)
            &bytes_leidos, // Variable global que almacena cuántos bytes se leyeron realmente
            NULL // NULL para operación síncrona (si no, usar estructura OVERLAPPED para I/O asíncrono)
        ))
        {
            // Si ReadFile falla, llenamos el buffer con ceros
            memset(buffer_lectura, 0, 512);
        }
    }

    // Función para mostrar 384 bytes en caja de texto
    void ventana_principal::actualizar_caja_texto(uint64_t posicion_referencia)
    {
        // Establecer la posición actual de lectura
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

    // Función para 
    void ventana_principal::actualizar_barra_progreso(uint64_t pos_val)
    {
        // Calcular progreso actual
        unsigned short progreso_val = round(100.0f * (static_cast<float>(pos_val) / tamano_unidad_actual));

        // Mostrar progreso en barra
        progressBar1->Value = progreso_val;
    }

    // 
    void ventana_principal::habilitar_gui()
    {
        // Reiniciar barra de progreso a cero
        progressBar1->Value = 0;

        // Reiniciar a sector 0
        proporcion_scrollbar = 0;

        // Colocar handler al inicio
        vScrollBar1->Value = 0;

        // Actualizar vista hexadecimal
        actualizar_caja_texto(0);

        // Rehabilitar controles
        comboBox1->Enabled = true;
        button1->Enabled = true;
        textBox1->Enabled = true;
        vScrollBar1->Enabled = true;
    }

    // 
    void ventana_principal::hilo_formateo()
    {
        // Definir tamaño de bloque (múltiplo de 512 bytes)
        const DWORD tamano_bloque = 64 * 1024; // Bloques de 64KB

        // Crear buffer de ceros alineado (ahora mucho más grande)
        void* buffer_ceros = _aligned_malloc(tamano_bloque, 512);

        // Llenamos el buffer con ceros
        memset(buffer_ceros, 0, tamano_bloque);

        // 
        LARGE_INTEGER pos_val = { 0 }; // 

        // Reemplazar todos los bytes en unidad por 0x00
        while (pos_val.QuadPart < tamano_unidad_actual)
        {
            // Calcular cuántos bytes escribir en esta iteración
            uint64_t bytes_restantes = tamano_unidad_actual - pos_val.QuadPart;
            DWORD bytes_a_escribir = static_cast<DWORD>(min(static_cast<uint64_t>(tamano_bloque), bytes_restantes));

            // Posicionar puntero
            SetFilePointerEx(
                manejador_unidad, // Manejador del archivo o disco
                pos_val, // Desplazamiento (offset) en bytes
                nullptr, // Devuelve la nueva posición (puede ser nullptr)
                FILE_BEGIN // Cómo interpretar el offset (inicio, actual, final)
            );

            DWORD bytes_escritos; // 

            // Escribir bloque de ceros
            WriteFile(
                manejador_unidad, // 
                buffer_ceros, // 
                bytes_a_escribir, // 
                &bytes_escritos, // 
                nullptr // 
            );

            // Avanzar al siguiente bloque
            pos_val.QuadPart += bytes_escritos;

            // Actualizar barra de progreso cada 1MB (polling)
            if (pos_val.QuadPart % (1024 * 1024) == 0)
            {
                this->Invoke(gcnew System::Action<uint64_t>(this, &ventana_principal::actualizar_barra_progreso), static_cast<uint64_t>(pos_val.QuadPart)); // 
            }
        }

        // Liberar memoria
        _aligned_free(buffer_ceros);

        // Asegurar que la barra llegue al 100%
        this->Invoke(gcnew System::Action<uint64_t>(this, &ventana_principal::actualizar_barra_progreso), tamano_unidad_actual);

        // Mostrar mensaje de finalización
        System::Windows::Forms::MessageBox::Show("Operation completed", "Message Box", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);

        // Habilitar controles
        this->Invoke(gcnew System::Action(this, &ventana_principal::habilitar_gui));
    }







    // Función para formatear unidad completa
    void ventana_principal::formatear_unidad()
    {
        // Mostrar mensaje de advertencia
        if (System::Windows::Forms::MessageBox::Show("All data on the selected disk will be erased", "Warning Message", System::Windows::Forms::MessageBoxButtons::YesNo, System::Windows::Forms::MessageBoxIcon::Warning) == System::Windows::Forms::DialogResult::Yes)
        {
            // Deshabilitar controles
            comboBox1->Enabled = false;
            button1->Enabled = false;
            textBox1->Enabled = false;
            vScrollBar1->Enabled = false;

            // Ejecutar formateo en un hilo dedicado (evita que la GUI se congele)
            hilo_secundario = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &ventana_principal::hilo_formateo)); // 
            hilo_secundario->IsBackground = true; // Hilo en segundo plano
            hilo_secundario->Start(); // 
        }
    }

    // Función para calcular posición en unidad a partir del handler de scrollbar vertical
    void ventana_principal::posicion_unidad_vscrollbar()
    {
        // Capturar posición de handler
        proporcion_scrollbar = static_cast<float>(vScrollBar1->Value) / 100.0f;

        // Calcular byte en la unidad (sector según posición del scrollbar vertical)
        uint64_t posicion_calculada = round(proporcion_scrollbar * cantidad_sectores) * 512;

        // Actualizar vista hexadecimal con la unidad actual seleccionado
        actualizar_caja_texto(posicion_calculada);
    }

    // Función para registrar datos de la unidad seleccionada (ej. tamaño en bytes)
    void ventana_principal::datos_unidad_actual()
    {
        // Cerrar manejador si estaba abierto previamente
        if (manejador_unidad != INVALID_HANDLE_VALUE)
        {
            CloseHandle(manejador_unidad); // 

            manejador_unidad = INVALID_HANDLE_VALUE; // 
        }

        // Abrir la unidad física en modo lectura
        manejador_unidad = CreateFileW(
            unidad_actual, // Ruta al dispositivo (ej. "\\\\.\\C:")
            GENERIC_READ | GENERIC_WRITE, // Permisos de lectura y escritura
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
            nullptr, // Sin atributos de seguridad
            OPEN_EXISTING, // Abrir solo si existe
            FILE_FLAG_NO_BUFFERING | // Sin buffering del sistema
            FILE_FLAG_WRITE_THROUGH | // Escritura directa al hardware
            FILE_FLAG_SEQUENTIAL_SCAN | // Acceso secuencial optimizado
            FILE_FLAG_OVERLAPPED, // Habilitar I/O asíncrono
            nullptr // Sin plantilla de archivo
        );

        // Obtener tamaño de la unidad
        GET_LENGTH_INFORMATION tam_unidad;
        DWORD bytes_retorno;

        // 
        DeviceIoControl(
            manejador_unidad, // Handle del disco
            IOCTL_DISK_GET_LENGTH_INFO, // Código de control
            NULL, // 
            0, // 
            &tam_unidad, sizeof(tam_unidad), // Buffer de salida
            &bytes_retorno, // Bytes devueltos
            NULL // No overlapped I/O
        );

        tamano_unidad_actual = tam_unidad.Length.QuadPart; // Tamaño real en bytes

        // Calcular el número de sectores en la unidad
        cantidad_sectores = round(tamano_unidad_actual / 512);
    }

    // Actualizar combobox con la lista de unidades disponibles
    void ventana_principal::actualizar_combobox()
    {
        // Limpiar contenido previo del combobox
        comboBox1->Items->Clear();

        // Recorremos sólo las unidades existentes
        for (const auto& unidad : lista_unidades)
        {
            // Formatear a "X:\" usando el dato del vector
            wchar_t display[10];

            swprintf(display, 10, L"%c:\\", unidad[4]);  // Extrae la letra

            // Agregar al combobox
            comboBox1->Items->Add(gcnew System::String(display));
        }

        // Auto-selección si hay elementos
        comboBox1->SelectedIndex = 0;

        // Obtener tamaño de bytes de unidad
        datos_unidad_actual();

        // Actualizar vista hexadecimal con la unidad actual seleccionado
        actualizar_caja_texto(0);
    }

    // Función para detectar y registrar unidades disponibles
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
                // Añadir al vector dinámico
                wchar_t unidad[10]; // 

                swprintf(unidad, 10, L"\\\\.\\%c:", letter); // 

                lista_unidades.push_back(unidad); // 
            }
        }

        // Actualizar combobox
        actualizar_combobox();
    }

    // Función para 
    void ventana_principal::cambiar_unidad()
    {
        // Obtener la ruta completa de la unidad seleccionada desde lista_unidades
        wcscpy_s(unidad_actual, MAX_PATH, lista_unidades[comboBox1->SelectedIndex].c_str());

        // Reiniciar a sector 0
        proporcion_scrollbar = 0;

        // Colocar handler al inicio
        vScrollBar1->Value = 0;

        // Obtener tamaño de bytes de unidad
        datos_unidad_actual();

        // Actualizar vista hexadecimal
        actualizar_caja_texto(0);
    }
}
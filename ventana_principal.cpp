#include <tchar.h> // Para manejar caracteres TCHAR (compatibilidad Unicode/ANSI)
#include <stdio.h> // Para funciones de entrada/salida estándar (printf)
#include <iomanip> // Para manipuladores de flujo de entrada/salida (setprecision, setw, etc.)
#include <vector> // Para contenedores dinámicos de tipo vector
#include <cmath> // Para funciones matemáticas (round, fmod, etc.)
#include <thread> // Para manejo de hilos y concurrencia
#include <malloc.h> // Para funciones de asignación de memoria (_aligned_malloc, _aligned_free)
#include <windows.h> // Para API de Windows (CreateFile, WriteFile, etc.)
#using <System.dll> // Referencia al ensamblado System.dll para funcionalidades .NET
#include "ventana_principal.h" // Archivo de cabecera con la definición de la clase ventana_principal
#include "variables.h" // Archivo de cabecera con declaraciones de variables globales

// Espacio de nombres para encapsular la aplicación DriveFormatter
namespace DriveFormatter
{
    // AMPLIACIÓN DE FUNCIONES
    // Función para obtener 384 bytes o menos desde la unidad (según la posición del scrollbar vertical)
    void ventana_principal::leer_bytes_unidad()
    {
        // Estructura para manejar posiciones de 64 bits en archivos grandes
        LARGE_INTEGER pos_val; // Variable para almacenar la posición de lectura

        pos_val.QuadPart = posicion_unidad; // Asignar posición actual de la unidad

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
        System::String^ texto_hex = gcnew System::String(""); // Cadena para almacenar la representación hexadecimal

        BYTE* buffer = static_cast<BYTE*>(buffer_lectura); // Convertir buffer a puntero de bytes

        // Convertir cada byte a formato hexadecimal y agregarlo al texto
        for (DWORD i = 0; i < 384; ++i)
        {
            texto_hex += System::String::Format("{0:X2} ", buffer[i]);
        }

        // Mostrar el texto en el control EDIT
        textBox1->Text = texto_hex;
    }

    // Función para actualizar la barra de progreso durante el formateo
    void ventana_principal::actualizar_barra_progreso(uint64_t pos_val)
    {
        // Calcular progreso actual
        unsigned short progreso_val = min(100, round(100.0f * (static_cast<float>(pos_val) / tamano_unidad_actual)));

        // Mostrar progreso en barra
        progressBar1->Value = progreso_val;
    }

    // Función para rehabilitar los controles GUI después del formateo
    void ventana_principal::habilitar_gui()
    {
        // Reiniciar contador de sobrescritura
        suma_bytes = 0;

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

    // Función para sobrescribir con ceros un segmento específico de la unidad
    void ventana_principal::sobrescribir_segmentos(uint64_t posicion_inicio, uint64_t posicion_fin)
    {
        // Abrir la unidad física
        HANDLE hilo_unidad = CreateFileW(
            unidad_actual, // Ruta al dispositivo (ej. "\\\\.\\C:")
            GENERIC_READ | GENERIC_WRITE, // Permisos de lectura y escritura
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
            nullptr, // Sin atributos de seguridad
            OPEN_EXISTING, // Abrir solo si existe
            FILE_FLAG_NO_BUFFERING | // Sin buffering del sistema
            FILE_FLAG_WRITE_THROUGH | // Escritura directa al hardware
            FILE_FLAG_SEQUENTIAL_SCAN, // Acceso secuencial optimizado
            nullptr // Sin plantilla de archivo
        );

        // Definir tamaño de bloque (múltiplo de 512 bytes)
        const DWORD tamano_bloque = 64 * 1024; // Bloques de 64KB

        // Crear buffer de ceros alineado
        void* buffer_ceros = _aligned_malloc(tamano_bloque, 512);

        // Llenamos el buffer con ceros
        memset(buffer_ceros, 0, tamano_bloque);

        // Ajustar inicio y fin para el segmento final si existe descuadre
        if (posicion_fin > tamano_unidad_actual)
        {
            posicion_fin = tamano_unidad_actual; // Ajustar al final

            // Hacer el segmento múltiplo de 512 (ajustar inicio)
            if (fmod(posicion_fin - posicion_inicio, 512.0) > 0.0)
            {
                // Retroceder ligeramente los bytes necesarios para volverlo múltiplo de 512
                posicion_inicio -= 512 - (fmod(posicion_fin - posicion_inicio, 512.0));

                // Ajustar el progreso en la variable global
                suma_bytes = posicion_inicio;
            }
        }

        // Estructura para manejar la posición actual en el archivo
        LARGE_INTEGER pos_val = { posicion_inicio };

        // Reemplazar todos los bytes en unidad por 0x00
        while (pos_val.QuadPart < posicion_fin)
        {
            // Calcular cuántos bytes escribir en esta iteración
            uint64_t bytes_restantes = posicion_fin - pos_val.QuadPart;

            // Determinar el tamaño del bloque a escribir (no exceder el buffer)
            DWORD bytes_a_escribir = static_cast<DWORD>(min(static_cast<uint64_t>(tamano_bloque), bytes_restantes));

            // Posicionar puntero
            SetFilePointerEx(
                hilo_unidad, // Manejador del archivo o disco
                pos_val, // Desplazamiento (offset) en bytes
                nullptr, // Devuelve la nueva posición (puede ser nullptr)
                FILE_BEGIN // Cómo interpretar el offset (inicio, actual, final)
            );

            DWORD bytes_escritos; // Variable para almacenar la cantidad de bytes escritos

            // Escribir bloque de ceros
            WriteFile(
                hilo_unidad, // Manejador de la unidad
                buffer_ceros, // Buffer con datos a escribir (ceros)
                bytes_a_escribir, // Número de bytes a escribir
                &bytes_escritos, // Bytes realmente escritos
                nullptr // Operación síncrona
            );

            // Avanzar al siguiente bloque
            pos_val.QuadPart += bytes_escritos;

            // Sumar a variable global (desde todos los hilos, proceso atómico)
            suma_bytes += bytes_escritos;

            // Actualizar barra de progreso cada 1MB (polling)
            if (suma_bytes % (1024 * 1024) == 0)
            {
                this->Invoke(gcnew System::Action<uint64_t>(this, &ventana_principal::actualizar_barra_progreso), static_cast<uint64_t>(suma_bytes)); // Invocar actualización de progreso en hilo principal
            }
        }

        // Liberar memoria
        _aligned_free(buffer_ceros);

        // Cerrar acceso
        CloseHandle(hilo_unidad);
    }

    // Función estática de la que se generan hilos, ejecuta a la función objetivo no estática
    void ventana_principal::funcion_intermediaria(System::Object^ ref_clase)
    {
        clase_intermediaria^ manej_clas_interm = (clase_intermediaria^)ref_clase; // Manejador para clase intermediaria, conversión de Object^ a ref class para la referencia pasada como parámetro

        manej_clas_interm->instancia_ventana->sobrescribir_segmentos(manej_clas_interm->inicio_val, manej_clas_interm->fin_val); // Ejecutar y mandar parámetros a la función no estática objetivo desde cada manejador en cada clase
    }

    // Función principal que maneja el formateo usando múltiples hilos
    void ventana_principal::hilo_formateo()
    {
        // Detectar número de hilos disponibles
        unsigned short num_hilos = System::Environment::ProcessorCount;

        // Tamaño de cada segmento de la unidad en relación al número de hilos disponibles
        float tamano_segmento_float = tamano_unidad_actual / num_hilos;

        // Ajustar tamaño de segmento a múltiplos de 512
        if (fmod(tamano_segmento_float, 512.0) > 0.0)
        {
            tamano_segmento = (unsigned int(tamano_segmento_float / 512) + 1) * 512; // Ligeramente mayor ajustado a 512
        }
        else
        {
            tamano_segmento = tamano_segmento_float; // El mismo tamaño
        }

        // Lista para almacenar los hilos
        System::Collections::Generic::List<System::Threading::Thread^>^ lista_hilos = gcnew System::Collections::Generic::List<System::Threading::Thread^>();

        // Asignar valores de inicio y fin distintos a instancias de clase intermediaria para cada hilo de operación
        for (unsigned short iter_seg = 0; iter_seg < num_hilos; iter_seg++)
        {
            // Sobrescribir cada segmento en hilos dedicados (sobrescritura en paralelo)
            // Generar y pasar parámetros a la clase intermediaria entre la función estática (compatible con Thread CLR) y no estática (llamada desde la función estática, función objetivo que contiene la lógica)
            clase_intermediaria^ inst_interm = gcnew clase_intermediaria(this, tamano_segmento * iter_seg, tamano_segmento * (iter_seg + 1));

            // Generar múltiples hilos de la función estática compatible
            System::Threading::Thread^ hilo_gen = gcnew System::Threading::Thread(gcnew System::Threading::ParameterizedThreadStart(&ventana_principal::funcion_intermediaria));

            // Configurar hilo como de segundo plano
            hilo_gen->IsBackground = true; // Ejecutar en segundo plano

            lista_hilos->Add(hilo_gen); // Almacenar hilo en lista de hilos

            hilo_gen->Start(inst_interm); // Pasar por párametro (referencia) la instancia de la clase intermediaria que ha capturado los valores de inico y fin de segmento
        }

        // Esperar a que todos los hilos terminen
        for each (System::Threading::Thread ^ hilo_iter in lista_hilos)
        {
            hilo_iter->Join();
        }

        // Libera las referencias a los objetos Thread
        lista_hilos->Clear();

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
            hilo_secundario = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &ventana_principal::hilo_formateo)); // Crear hilo para formateo
            hilo_secundario->IsBackground = true; // Hilo en segundo plano
            hilo_secundario->Start(); // Iniciar hilo de formateo
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
            CloseHandle(manejador_unidad); // Cerrar handle anterior

            manejador_unidad = INVALID_HANDLE_VALUE; // Resetear handle
        }

        // Abrir la unidad física
        manejador_unidad = CreateFileW(
            unidad_actual, // Ruta al dispositivo (ej. "\\\\.\\C:")
            GENERIC_READ | GENERIC_WRITE, // Permisos de lectura y escritura
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Compartir acceso
            nullptr, // Sin atributos de seguridad
            OPEN_EXISTING, // Abrir solo si existe
            FILE_FLAG_NO_BUFFERING | // Sin buffering del sistema
            FILE_FLAG_WRITE_THROUGH | // Escritura directa al hardware
            FILE_FLAG_SEQUENTIAL_SCAN, // Acceso secuencial optimizado
            nullptr // Sin plantilla de archivo
        );

        // Obtener tamaño de la unidad
        GET_LENGTH_INFORMATION tam_unidad;
        DWORD bytes_retorno;

        // Obtener información de longitud del dispositivo
        DeviceIoControl(
            manejador_unidad, // Handle del disco
            IOCTL_DISK_GET_LENGTH_INFO, // Código de control
            NULL, // Sin buffer de entrada
            0, // Tamaño del buffer de entrada
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

            swprintf(display, 10, L"%c:\\", unidad[4]); // Extrae la letra

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

        DWORD drives = GetLogicalDrives(); // Obtener máscara de bits de unidades lógicas

        for (char letter = 'A'; letter <= 'Z'; ++letter)
        {
            if (drives & (1 << (letter - 'A')))
            {
                // Añadir al vector dinámico
                wchar_t unidad[10]; // Buffer para almacenar la ruta de la unidad

                swprintf(unidad, 10, L"\\\\.\\%c:", letter); // Formatear ruta física de la unidad

                lista_unidades.push_back(unidad); // Agregar unidad al vector
            }
        }

        // Actualizar combobox
        actualizar_combobox();
    }

    // Función para cambiar la unidad seleccionada y actualizar la vista
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

    // Función para liberar memoria al cerrar programa
    void ventana_principal::liberar_memoria()
    {
        _aligned_free(buffer_lectura); // Liberar buffer de lectura alineado
        CloseHandle(manejador_unidad); // Cerrar handle de la unidad
    }
}
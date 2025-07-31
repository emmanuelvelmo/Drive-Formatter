#pragma comment(lib, "user32.lib")  // Enlaza con la biblioteca 'user32.dll' para funciones de la API de Windows
#include <windows.h>  // Proporciona definiciones esenciales de Windows (HINSTANCE, LPSTR, etc.)
#include "ventana_principal.h"  // Declaración de la clase principal del formulario

// Usa el espacio de nombres System para acceder a las clases de .NET
using namespace System;

// El atributo STAThread debe estar antes de la declaración de WinMain
[STAThread]

// PUNTO DE PARTIDA
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Configuraciones del programa
    System::Windows::Forms::Application::EnableVisualStyles(); // Configura la aplicación para usar estilos visuales modernos
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false); // Optimiza el renderizado de texto para compatibilidad con GDI+

    // Crear y ejecutar el formulario principal
    DriveFormatter::ventana_principal^ form_val = gcnew DriveFormatter::ventana_principal(); // Crea una instancia del formulario principal de la aplicación
    System::Windows::Forms::Application::Run(form_val); // Ejecuta el bucle principal de mensajes de Windows Forms

    return 0;
}
#include "ventana_principal.h"
#include <windows.h> // Necesario para HINSTANCE, LPSTR, etc.

using namespace System;

// El atributo STAThread debe estar antes de la declaración de WinMain
[STAThread]

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    System::Windows::Forms::Application::EnableVisualStyles();
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

    // Crear y ejecutar el formulario principal
    DriveFormatter::ventana_principal^ form = gcnew DriveFormatter::ventana_principal();
    System::Windows::Forms::Application::Run(form);

    return 0;
}
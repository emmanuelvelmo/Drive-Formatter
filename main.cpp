#pragma comment(lib, "user32.lib") // 
#include <windows.h> // Necesario para HINSTANCE, LPSTR, etc.
#include "ventana_principal.h" //

//
using namespace System;

// El atributo STAThread debe estar antes de la declaración de WinMain
[STAThread]

// PUNTO DE PARTIDA
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // CONFIGURACIONES DEL PROGRAMA
    //
    System::Windows::Forms::Application::EnableVisualStyles();
    //
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

    // Crear y ejecutar el formulario principal
    //
    DriveFormatter::ventana_principal^ form_val = gcnew DriveFormatter::ventana_principal();
    //
    System::Windows::Forms::Application::Run(form_val);

    return 0; // 
}
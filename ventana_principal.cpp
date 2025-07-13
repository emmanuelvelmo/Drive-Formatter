#include "ventana_principal.h"

// Atributo esencial para aplicaciones Windows Forms que usa COM (Component Object Model)
// STA (Single Threaded Apartment) garantiza que los componentes COM se ejecuten en un entorno seguro para hilos
[System::STAThreadAttribute]

// Punto de entrada principal de la aplicación
// args: Argumentos pasados al programa desde la línea de comandos
int main(array<System::String^>^ args)
{
    // Habilita los estilos visuales de Windows XP/Vista/7/etc. para los controles de la interfaz
    System::Windows::Forms::Application::EnableVisualStyles();

    // Configura el modo de renderizado de texto compatible con versiones anteriores
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

    // Crea una instancia del formulario principal (ventana_principal)
    USBDriveFormatter::ventana_principal form;

    // Ejecuta el bucle de mensajes de la aplicación con el formulario principal
    // % es el operador de direccionamiento en C++/CLI que obtiene la dirección administrada
    System::Windows::Forms::Application::Run(% form);

    return 0;
}
#pragma once // Evita inclusión múltiple del archivo de cabecera
#include <cmath> // Biblioteca para funciones matemáticas
#using <System.dll> // Referencia al ensamblado System.dll para funcionalidades .NET
#include <windows.h> // Para tipos y funciones de WinAPI

// Espacio de nombres para encapsular la aplicación DriveFormatter
namespace DriveFormatter
{
    // Clase del formulario principal de la aplicación
    public ref class ventana_principal : public System::Windows::Forms::Form
    {
        public:
            // CONSTRUCTOR DE LA VENTANA
            ventana_principal(void)
            {
                // Función para iniciar componentes de la GUI
                InitializeComponent();

                // Deshabilitar redimensionamiento y botón de maximizar
                this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
                this->MaximizeBox = false;

                // Activar manejo de eventos de teclado y botones
                this->KeyPreview = true;
                this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ventana_principal::control_r);
                this->button1->Click += gcnew System::EventHandler(this, &ventana_principal::boton_formateo);
                this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &ventana_principal::click_combobox);
                this->vScrollBar1->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &ventana_principal::handler_vscrollbar);

                // LÓGICA DEL PROGRAMA AL INICIAR
                // Obtener directorios de los unidades montadas al iniciar programa
                directorios_unidades();
            }

        protected:
            // DESTRUCTOR DE LA VENTANA
            ~ventana_principal()
            {
                // Liberar memoria
                liberar_memoria();
            }

        public:
            // FUNCIONES A USAR EN CPP
            void leer_bytes_unidad(); // Lee los bytes de la unidad seleccionada
            void actualizar_caja_texto(uint64_t posicion_referencia); // Actualiza el contenido del TextBox con datos hexadecimales
            void actualizar_barra_progreso(uint64_t pos_val); // Actualiza el porcentaje de la barra de progreso
            void formatear_unidad(); // Inicia el proceso de formateo de la unidad seleccionada
            void posicion_unidad_vscrollbar(); // Maneja la posición del scroll vertical para navegar por sectores
            void datos_unidad_actual(); // Obtiene información de la unidad actualmente seleccionada
            void actualizar_combobox(); // Refresca la lista de unidades en el ComboBox
            void directorios_unidades(); // Enumera y carga las unidades disponibles en el sistema
            void cambiar_unidad(); // Cambia la unidad mostrada cuando se selecciona otra en el ComboBox
            void habilitar_gui(); // Habilita o deshabilita controles de la interfaz durante operaciones
            void hilo_formateo(); // Función que se ejecuta en hilo separado para el formateo
            void liberar_memoria(); // Libera recursos y memoria utilizada por la aplicación
            void sobrescribir_segmentos(uint64_t posicion_inicio, uint64_t posicion_fin); // Sobrescribe segmentos específicos de la unidad
            static void funcion_intermediaria(System::Object^ datos_val); // Función estática para crear hilos de trabajo

        private:
            // CONTROLES DE LA INTERFAZ GRÁFICA
            System::Windows::Forms::Label^ label1; // Etiqueta que indica "Select Device:"
            System::Windows::Forms::ComboBox^ comboBox1; // ComboBox para seleccionar unidad de disco
            System::Windows::Forms::Button^ button1; // Botón para iniciar el formateo
            System::Windows::Forms::ProgressBar^ progressBar1; // Barra que muestra progreso del formateo
            System::Windows::Forms::TextBox^ textBox1; // Caja de texto para mostrar los datos en formato hexadecimal
            System::Windows::Forms::VScrollBar^ vScrollBar1; // Scrollbar vertical para navegar por sectores

            // VARIABLES PARA MANEJO DE HILOS
            System::Threading::Thread^ hilo_secundario; // Hilo principal para operaciones de formateo
            System::Collections::Generic::List<System::Threading::Thread^>^ lista_hilos; // Lista para gestionar múltiples hilos de trabajo

            // REGIÓN GENERADA AUTOMÁTICAMENTE POR EL DISEÑADOR DE FORMULARIOS
            #pragma region Windows Form Designer generated code
            // Método generado automáticamente para inicializar los componentes del formulario
            void InitializeComponent(void)
            {
                this->label1 = (gcnew System::Windows::Forms::Label());
                this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
                this->button1 = (gcnew System::Windows::Forms::Button());
                this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
                this->textBox1 = (gcnew System::Windows::Forms::TextBox());
                this->vScrollBar1 = (gcnew System::Windows::Forms::VScrollBar());
                this->SuspendLayout();
                // 
                // label1
                // 
                this->label1->AutoSize = true;
                this->label1->Location = System::Drawing::Point(12, 9);
                this->label1->Name = L"label1";
                this->label1->Size = System::Drawing::Size(77, 13);
                this->label1->TabIndex = 0;
                this->label1->Text = L"Select Device:";
                // 
                // comboBox1
                // 
                this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
                this->comboBox1->FormattingEnabled = true;
                this->comboBox1->Location = System::Drawing::Point(15, 29);
                this->comboBox1->Name = L"comboBox1";
                this->comboBox1->Size = System::Drawing::Size(233, 21);
                this->comboBox1->TabIndex = 1;
                // 
                // button1
                // 
                this->button1->Location = System::Drawing::Point(254, 28);
                this->button1->Name = L"button1";
                this->button1->Size = System::Drawing::Size(75, 23);
                this->button1->TabIndex = 2;
                this->button1->Text = L"Format";
                this->button1->UseVisualStyleBackColor = true;
                // 
                // progressBar1
                // 
                this->progressBar1->Location = System::Drawing::Point(15, 392);
                this->progressBar1->Name = L"progressBar1";
                this->progressBar1->Size = System::Drawing::Size(312, 20);
                this->progressBar1->TabIndex = 3;
                // 
                // textBox1
                // 
                this->textBox1->BackColor = System::Drawing::Color::White;
                this->textBox1->Font = (gcnew System::Drawing::Font(L"Consolas", 8));
                this->textBox1->Location = System::Drawing::Point(15, 62);
                this->textBox1->Multiline = true;
                this->textBox1->Name = L"textBox1";
                this->textBox1->ReadOnly = true;
                this->textBox1->Size = System::Drawing::Size(295, 318);
                this->textBox1->TabIndex = 4;
                // 
                // vScrollBar1
                // 
                this->vScrollBar1->Location = System::Drawing::Point(310, 62);
                this->vScrollBar1->Name = L"vScrollBar1";
                this->vScrollBar1->Size = System::Drawing::Size(17, 318);
                this->vScrollBar1->TabIndex = 5;
                // 
                // ventana_principal
                // 
                this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
                this->ClientSize = System::Drawing::Size(341, 428);
                this->Controls->Add(this->vScrollBar1);
                this->Controls->Add(this->textBox1);
                this->Controls->Add(this->progressBar1);
                this->Controls->Add(this->button1);
                this->Controls->Add(this->comboBox1);
                this->Controls->Add(this->label1);
                this->Name = L"ventana_principal";
                this->Text = L"Drive Formatter";
                this->ResumeLayout(false);
                this->PerformLayout();
            }
            #pragma endregion

        // ATAJOS Y ACCIONES
        // Recargar lista de unidades con Ctrl+R
        private:
            System::Void control_r(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
            {
                if (e->Control && e->KeyCode == System::Windows::Forms::Keys::R)
                {
                    directorios_unidades();
                }
            }

            // Ejecutar formateo de la unidad seleccionada
            System::Void boton_formateo(System::Object^ sender, System::EventArgs^ e)
            {
                formatear_unidad();
            }

            // Cambiar unidad mostrada al cambiar selección en el ComboBox
            System::Void click_combobox(System::Object^ sender, System::EventArgs^ e)
            {
                cambiar_unidad();
            }

            // Manejador del evento Scroll
            System::Void handler_vscrollbar(System::Object^ sender, System::Windows::Forms::ScrollEventArgs^ e)
            {
                posicion_unidad_vscrollbar();
            }
        };

        // Clase para conectar método estático con no estático (método estático -> clase con referencia a clase principal (ésta) -> método no estático del cual generar hilos)
        ref class clase_intermediaria
        {
            public:
                // Referencia a clase con el método no estático
                DriveFormatter::ventana_principal^ instancia_ventana;

                // Variables no globales (evita condiciones de carrrera)
                uint64_t inicio_val; // Posición de inicio para operaciones de sobrescritura
                uint64_t fin_val; // Posición de fin para operaciones de sobrescritura

                // Constructor de la clase (captura parámetros)
                clase_intermediaria(DriveFormatter::ventana_principal^ inst_val, uint64_t ini_val, uint64_t fin_val)
                {
                    instancia_ventana = inst_val;
                    inicio_val = ini_val;
                    fin_val = fin_val;
                }
        };
}
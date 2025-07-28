#pragma once
#include <cmath> // 
#include <windows.h> // Para tipos y funciones de WinAPI

namespace DriveFormatter
{
    // Clase del formulario principal de la aplicación
    public ref class ventana_principal : public System::Windows::Forms::Form
    {
        public:
            // CONSTRUCTOR
            ventana_principal(void)
            {
                // Función para iniciar componentes de la GUI
                InitializeComponent();

                // Deshabilitar redimensionamiento y botón de maximizar
                this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
                this->MaximizeBox = false;

                // Activar manejo de eventos de teclado y botones
                this->KeyPreview = true;
                this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ventana_principal::ventana_principal_KeyDown);
                this->button1->Click += gcnew System::EventHandler(this, &ventana_principal::button1_Click);
                this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &ventana_principal::comboBox1_SelectedIndexChanged);
                this->vScrollBar1->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &ventana_principal::OnVScrollMoved);

                // LÓGICA DEL PROGRAMA AL INICIAR
                // Obtener directorios de los unidades montadas al iniciar programa
                directorios_unidades();
            }

        protected:
            // DESTRUCTOR
            ~ventana_principal()
            {
                if (componentes_val)
                {
                    // Liberar recursos de componentes del formulario
                    delete componentes_val;

                    // Liberar memoria
                    //_aligned_free(buffer);
                    //CloseHandle(manejador_disco);
                }
            }

        public:
            // FUNCIONES A USAR EN CPP
            void leer_bytes_unidad(); // 
            void actualizar_caja_texto(uint64_t posicion_referencia); // 
            void actualizar_barra_progreso(uint64_t pos_val); // 
            void formatear_unidad(); // 
            void posicion_unidad_vscrollbar(); // 
            void datos_unidad_actual(); // 
            void actualizar_combobox(); // 
            void directorios_unidades(); // 
            void cambiar_unidad(); // 
            void habilitar_gui(); // 
            void hilo_formateo(); // 

        private:
            System::Windows::Forms::Label^ label1; // Etiqueta que indica "Select Device:"
            System::Windows::Forms::ComboBox^ comboBox1; // ComboBox para seleccionar unidad de disco
            System::Windows::Forms::Button^ button1; // Botón para iniciar el formateo
            System::Windows::Forms::ProgressBar^ progressBar1; // Barra que muestra progreso del formateo
            System::Windows::Forms::TextBox^ textBox1; // Caja de texto para mostrar los datos en formato hexadecimal
            System::Windows::Forms::VScrollBar^ vScrollBar1; // Scrollbar vertical para navegar por sectores
            System::ComponentModel::Container^ componentes_val; // Contenedor de componentes del formulario
            
            // 
            System::Threading::Thread^ hilo_secundario;

            // 
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
            // 
            #pragma endregion

            // ATAJOS Y ACCIONES
            // Recargar lista de unidades con Ctrl+R
            private:
                System::Void ventana_principal_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
                {
                    if (e->Control && e->KeyCode == System::Windows::Forms::Keys::R)
                    {
                        directorios_unidades();
                    }
                }

                // Ejecutar formateo de la unidad seleccionada
                System::Void button1_Click(System::Object^ sender, System::EventArgs^ e)
                {
                    formatear_unidad();
                }

                // Cambiar unidad mostrada al cambiar selección en el ComboBox
                System::Void comboBox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
                {
                    cambiar_unidad();
                }

                // Manejador del evento Scroll
                System::Void OnVScrollMoved(System::Object^ sender, System::Windows::Forms::ScrollEventArgs^ e)
                {
                    posicion_unidad_vscrollbar();
                }
    };
}
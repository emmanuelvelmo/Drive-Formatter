#pragma once
#include <windows.h>

// Declaraciones externas
extern void directorios_unidades();
extern void formatear_unidad();
extern void cambiar_unidad();

//
namespace DriveFormatter
{
    //
    public ref class ventana_principal : public System::Windows::Forms::Form
    {
        //
        public:
            ventana_principal(void)
            {
                InitializeComponent();

                // Deshabilitar redimensionamiento y botón de maximizar
                this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
                this->MaximizeBox = false;

                // Activar eventos
                this->KeyPreview = true;
                this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &ventana_principal::ventana_principal_KeyDown);
                this->button1->Click += gcnew System::EventHandler(this, &ventana_principal::button1_Click);
                this->combobox1->SelectedIndexChanged += gcnew System::EventHandler(this, &ventana_principal::combobox1_SelectedIndexChanged);
            }

        //
        protected:
            ~ventana_principal()
            {
                if (components)
                {
                    delete components;
                }
            }
        
        //
        private:
            System::Windows::Forms::Label^ label1;
            System::Windows::Forms::ComboBox^ combobox1;
            System::Windows::Forms::Button^ button1;
            System::Windows::Forms::ProgressBar^ progressBar1;
            System::Windows::Forms::TextBox^ textBox1;
            System::Windows::Forms::VScrollBar^ vScrollBar1;
            System::ComponentModel::Container^ components;

        #pragma region Windows Form Designer generated code
        //
        void InitializeComponent(void)
        {
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->combobox1 = (gcnew System::Windows::Forms::ComboBox());
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
            // combobox1
            // 
            this->combobox1->FormattingEnabled = true;
            this->combobox1->Location = System::Drawing::Point(15, 29);
            this->combobox1->Name = L"combobox1";
            this->combobox1->Size = System::Drawing::Size(244, 21);
            this->combobox1->TabIndex = 1;
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(265, 28);
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
            this->progressBar1->Size = System::Drawing::Size(325, 23);
            this->progressBar1->TabIndex = 3;
            // 
            // textBox1
            // 
            this->textBox1->Location = System::Drawing::Point(15, 62);
            this->textBox1->Multiline = true;
            this->textBox1->Name = L"textBox1";
            this->textBox1->ReadOnly = true;
            this->textBox1->Size = System::Drawing::Size(308, 318);
            this->textBox1->TabIndex = 4;
            // 
            // vScrollBar1
            // 
            this->vScrollBar1->Location = System::Drawing::Point(323, 62);
            this->vScrollBar1->Name = L"vScrollBar1";
            this->vScrollBar1->Size = System::Drawing::Size(17, 318);
            this->vScrollBar1->TabIndex = 5;
            // 
            // ventana_principal
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(354, 432);
            this->Controls->Add(this->vScrollBar1);
            this->Controls->Add(this->textBox1);
            this->Controls->Add(this->progressBar1);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->combobox1);
            this->Controls->Add(this->label1);
            this->Name = L"ventana_principal";
            this->Text = L"Drive Formatter";
            this->ResumeLayout(false);
            this->PerformLayout();
        }
        #pragma endregion
        
        // ATAJOS Y ACCIONES
        //
        private: System::Void ventana_principal_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
        {
            if (e->Control && e->KeyCode == System::Windows::Forms::Keys::R)
            {
                directorios_unidades();
            }
        }

        //
        private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e)
        {
            formatear_unidad();
        }

        //
        private: System::Void combobox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
        {
            cambiar_unidad();
        }
    };
}
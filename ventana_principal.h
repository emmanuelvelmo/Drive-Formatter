#pragma once
namespace DriveFormatter
{
    public ref class ventana_principal : public System::Windows::Forms::Form
    {
    public:
        ventana_principal(void)
        {
            InitializeComponent();
        }
    protected:
        ~ventana_principal()
        {
            if (components)
            {
                delete components;
            }
        }

    protected:
    protected:
    private:
        System::ComponentModel::Container^ components;
#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->SuspendLayout();
            // 
            // ventana_principal
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(284, 261);
            this->Name = L"ventana_principal";
            this->Text = L"ventana_principal";
            this->ResumeLayout(false);

        }
#pragma endregion
    };
}
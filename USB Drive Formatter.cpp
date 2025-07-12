#include "framework.h"
#include "USB Drive Formatter.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

#define MAXIMO_CARACTERES 100

// Variables globales
HINSTANCE instancia_aplicacion;
WCHAR titulo_ventana[MAXIMO_CARACTERES] = L"Formateador USB"; // Valor por defecto
WCHAR clase_ventana[MAXIMO_CARACTERES] = L"ClasePrincipalUSB";

// Controles
HWND hComboUnidades;
HWND hBotonFormatear;
HWND hBarraProgreso;
HWND hAreaFormateo;

// Prototipos de funciones
ATOM RegistrarClaseVentana(HINSTANCE hInstance);
BOOL IniciarInstancia(HINSTANCE, int);
LRESULT CALLBACK ProcesarMensajes(HWND, UINT, WPARAM, LPARAM);
void CrearControles(HWND hWnd);
void ListarDispositivosUSB();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    instancia_aplicacion = hInstance;

    // Registrar clase de ventana
    if (!RegistrarClaseVentana(hInstance))
    {
        MessageBox(NULL, L"Error registrando clase de ventana", L"Error", MB_ICONERROR);
        return FALSE;
    }

    // Crear ventana principal
    HWND hWnd = CreateWindowW(
        clase_ventana,
        titulo_ventana,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL, L"Error creando ventana principal", L"Error", MB_ICONERROR);
        return FALSE;
    }

    // Crear controles
    CrearControles(hWnd);

    // Listar dispositivos
    ListarDispositivosUSB();

    // Mostrar y actualizar ventana
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Bucle principal de mensajes CON DEPURACIÓN
    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Mensaje de depuración
            OutputDebugString(L"Bucle principal ejecutándose...\n");
            Sleep(10); // Pequeña pausa para evitar uso excesivo de CPU
        }
    }

    return (int)msg.wParam;
}

ATOM RegistrarClaseVentana(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ProcesarMensajes;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = clase_ventana;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wcex);
}

void CrearControles(HWND hWnd)
{
    // ComboBox para unidades USB
    hComboUnidades = CreateWindowW(
        L"COMBOBOX",
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT | WS_TABSTOP,
        50, 50, 300, 150,
        hWnd,
        (HMENU)IDC_COMBO_UNIDADES,
        instancia_aplicacion,
        NULL
    );

    // Botón de formatear
    hBotonFormatear = CreateWindowW(
        L"BUTTON",
        L"Formatear",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        360, 50, 100, 25,
        hWnd,
        (HMENU)IDC_FORMATEADOR_USB,
        instancia_aplicacion,
        NULL
    );

    // Área de información
    hAreaFormateo = CreateWindowW(
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
        50, 100, 500, 300,
        hWnd,
        (HMENU)IDC_AREA_FORMATEO,
        instancia_aplicacion,
        NULL
    );

    // Barra de progreso vertical
    hBarraProgreso = CreateWindowW(
        L"SCROLLBAR",
        L"",
        WS_CHILD | WS_VISIBLE | SBS_VERT,
        560, 100, 20, 300,
        hWnd,
        (HMENU)IDC_BARRA_PROGRESO,
        instancia_aplicacion,
        NULL
    );
}

void ListarDispositivosUSB()
{
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, NULL, NULL, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, L"No se pudieron enumerar dispositivos USB", L"Error", MB_ICONERROR);
        return;
    }

    SP_DEVINFO_DATA DeviceInfoData = { 0 };
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        WCHAR deviceName[256];
        DWORD size = 0;

        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_DEVICEDESC,
            NULL,
            (PBYTE)deviceName,
            sizeof(deviceName),
            &size))
        {
            SendMessage(hComboUnidades, CB_ADDSTRING, 0, (LPARAM)deviceName);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
}

LRESULT CALLBACK ProcesarMensajes(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_FORMATEADOR_USB:
            MessageBox(hWnd, L"Funcionalidad de formateo será implementada aquí", L"Acción", MB_OK);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Dibujar texto en el área de formateo
        RECT rect;
        GetClientRect(hAreaFormateo, &rect);
        DrawText(hdc, L"Información de formateo aparecerá aquí", -1, &rect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
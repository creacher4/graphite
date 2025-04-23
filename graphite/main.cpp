#include "core/Application.h"

#include <Windows.h>

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    (void)hPrevInstance;

    Application app(hInstance);
    app.Run();
    return 0;
}

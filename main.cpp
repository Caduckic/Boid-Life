#ifdef WIN32
    #include <windows.h>
#endif

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "engine.hpp"

#ifdef WIN32
    int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
        HWND myConsole = GetConsoleWindow();
        ShowWindow(myConsole,0);
#else
    int main() {
#endif
    Engine engine;
    engine.run();
    return 0;
}
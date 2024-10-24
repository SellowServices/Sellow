#include "pch.h"
// HelloWorldDLL.cpp
#include <windows.h>
#include <string>

// Function to write "hello, world!" into an open Notepad window
extern "C" __declspec(dllexport) void WriteHelloWorld() {
    MessageBoxA(NULL, "Startin.", "Warning", MB_OK | MB_ICONWARNING);

    // Find the Notepad window
    HWND hwnd = FindWindowA("Notepad", NULL);
    if (hwnd) {
        MessageBoxA(NULL, "Notepad is open.", "Warning", MB_OK | MB_ICONWARNING);
        // Bring the Notepad window to the foreground
        SetForegroundWindow(hwnd);

        // The text to send
        std::string text = "hello, world!\n";

        // Send each character to the Notepad window
        for (char c : text) {
            SendMessage(hwnd, WM_CHAR, (WPARAM)c, 0);
        }
    }
    else {
        MessageBoxA(NULL, "Notepad is not open.", "Error", MB_OK | MB_ICONERROR);
    }
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Initialization code can go here
        MessageBoxA(NULL, "DLL Loaded!", "Info", MB_OK | MB_ICONINFORMATION);
        WriteHelloWorld();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        // Cleanup code can go here
        break;
    }
    return TRUE;
}

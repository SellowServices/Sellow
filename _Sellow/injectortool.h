#pragma once

#ifndef INJECTORTOOL_H
#define INJECTORTOOL_H

#include <windows.h>
#include <wchar.h>
#include <sys/stat.h>
#include <libloaderapi.h>
#include <iostream>

class InjectorTool {
public:
    // Constructor that initializes the DLL name and window title
    InjectorTool(const wchar_t* dll_name, const wchar_t* window_title);

    // Public method to perform the injection
    bool inject(); // Returns true on success, false on failure

private:
    const wchar_t* dll_name;           // Path to the DLL to inject
    const wchar_t* window_title;       // Title of the target window
    DWORD proc_id;                     // Process ID of the target application
    HANDLE h_process;                  // Handle to the opened process
    wchar_t dll_path[MAX_PATH];        // Buffer for the DLL path
    void* allocated_memory;             // Pointer to the allocated memory in the target process

    // Private methods used for the injection process
    void getProcID();                  // Gets the process ID of the target window
    void error(const wchar_t* error_title, const wchar_t* error_msg); // Handles errors
    bool DLLExists(const wchar_t* file_name); // Checks if the specified DLL exists
    void openProcess();                // Opens the target process
    void allocateMemory();             // Allocates memory in the target process
    void writeMemory();                // Writes the DLL path into the allocated memory
    void createRemoteThread();         // Creates a remote thread to load the DLL
    void cleanup();                    // Cleans up allocated resources
};

#endif // INJECTORTOOL_H

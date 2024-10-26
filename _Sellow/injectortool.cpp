#include "injectortool.h"
#include <stdexcept> // For std::runtime_error
#include <sys/stat.h> // For _stat
#include <qDebug>

InjectorTool::InjectorTool(const wchar_t* dll_name, const wchar_t* window_title)
    : dll_name(dll_name), window_title(window_title), proc_id(0), h_process(nullptr) {}

bool InjectorTool::inject() {
    try {
        if (!DLLExists(dll_name)) {
            error(L"DLLExists", L"DLL doesn't exist!");
            return false;
        }

        if (!GetFullPathNameW(dll_name, MAX_PATH, dll_path, nullptr)) {
            error(L"GetFullPath", L"Failed to get full path of DLL!");
            return false;
        }

        getProcID();
        if (proc_id == 0) {
            error(L"GetProcessID", L"Failed to get process ID!");
            return false;
        }

        openProcess();
        allocateMemory();
        writeMemory();
        createRemoteThread();

        cleanup();
        return true;

    } catch (const std::exception& e) {
        qDebug() << "Exception in inject(): " << e.what();
        cleanup();
        return false;
    }
}

void InjectorTool::getProcID() {
    // Get the window handle and the process ID
    HWND hwnd = FindWindow(NULL, window_title);
    if (hwnd != NULL) {
        GetWindowThreadProcessId(hwnd, &proc_id);
    } else {
        proc_id = 0; // Reset proc_id if the window is not found
    }
}

void InjectorTool::error(const wchar_t* error_title, const wchar_t* error_msg) {
    // Retrieve the last error code and prepare the error message
    DWORD last_error = GetLastError();
    wchar_t full_msg[512];
    swprintf(full_msg, 512, L"%s\nError Code: %lu", error_msg, last_error);
    MessageBoxW(0, full_msg, error_title, 0);
    throw std::runtime_error("DLL injection failed."); // Consider throwing an exception
}

bool InjectorTool::DLLExists(const wchar_t* file_name) {
    struct _stat buffer;
    return (_wstat(file_name, &buffer) == 0); // Check if the file exists
}

void InjectorTool::openProcess() {
    // Attempt to open the target process with all access rights
    h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
    if (!h_process) {
        error(L"OpenProcess", L"Failed to open a handle to the process!");
    }
}

void InjectorTool::allocateMemory() {
    // Allocate memory in the target process for the DLL path
    allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory) {
        error(L"AllocateMemory", L"Failed to allocate memory!");
    }
}

void InjectorTool::writeMemory() {
    // Write the DLL path into the allocated memory in the target process
    if (!WriteProcessMemory(h_process, allocated_memory, dll_path, (wcslen(dll_path) + 1) * sizeof(wchar_t), nullptr)) {
        error(L"WriteProcessMemory", L"Failed to write to process memory!");
    }
}

void InjectorTool::createRemoteThread() {
    // Create a remote thread to load the DLL in the target process
    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, allocated_memory, 0, nullptr);
    if (!h_thread) {
        error(L"CreateRemoteThread", L"Failed to create remote thread!");
    }
    CloseHandle(h_thread); // Close the thread handle
}

void InjectorTool::cleanup() {
    // Clean up resources by closing the process handle and freeing allocated memory
    if (h_process) {
        CloseHandle(h_process);
    }
    if (allocated_memory) {
        VirtualFreeEx(h_process, allocated_memory, 0, MEM_RELEASE);
    }
}

#include "InjectorTool.h"
#include <libloaderapi.h>
#include <iostream>

InjectorTool::InjectorTool(const wchar_t* dll_name, const wchar_t* window_title)
    : dll_name(dll_name), window_title(window_title), proc_id(0), h_process(nullptr) {}

void InjectorTool::inject() {
    if (!DLLExists(dll_name)) {
        error(L"DLLExists", L"DLL Doesn't exist!");
    }

    if (!GetFullPathNameW(dll_name, MAX_PATH, dll_path, nullptr)) {
        error(L"GetFullPath", L"Failed to get full path of DLL!");
    }

    getProcID();
    if (proc_id == 0) {
        error(L"GetProcessID", L"Failed to get process ID!");
    }

    openProcess();
    allocateMemory();
    writeMemory();
    createRemoteThread();

    cleanup();
    MessageBoxW(0, L"Successfully Injected!", L"Success", 0);
}

void InjectorTool::getProcID() {
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &proc_id);
}

void InjectorTool::error(const wchar_t* error_title, const wchar_t* error_msg) {
    DWORD last_error = GetLastError();
    wchar_t full_msg[512];
    swprintf(full_msg, 512, L"%s\nError Code: %lu", error_msg, last_error);
    MessageBoxW(0, full_msg, error_title, 0);
    exit(-1);
}

bool InjectorTool::DLLExists(const wchar_t* file_name) {
    struct _stat buffer;
    return (_wstat(file_name, &buffer) == 0);
}

void InjectorTool::openProcess() {
    h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);
    if (!h_process) {
        error(L"OpenProcess", L"Failed to open a handle to process!");
    }
}

void InjectorTool::allocateMemory() {
    allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory) {
        error(L"AllocateMemory", L"Failed to allocate memory!");
    }
}

void InjectorTool::writeMemory() {
    if (!WriteProcessMemory(h_process, allocated_memory, dll_path, (wcslen(dll_path) + 1) * sizeof(wchar_t), nullptr)) {
        error(L"WriteProcessMemory", L"Failed to write to process memory!");
    }
}

void InjectorTool::createRemoteThread() {
    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, allocated_memory, 0, nullptr);
    if (!h_thread) {
        error(L"CreateRemoteThread", L"Failed to create remote thread!");
    }
    CloseHandle(h_thread);
}

void InjectorTool::cleanup() {
    CloseHandle(h_process);
    VirtualFreeEx(h_process, allocated_memory, 0, MEM_RELEASE);
}
#pragma once

#include <windows.h>
#include <wchar.h>
#include <sys/stat.h>

class InjectorTool {
public:
    InjectorTool(const wchar_t* dll_name, const wchar_t* window_title);
    void inject();

private:
    const wchar_t* dll_name;
    const wchar_t* window_title;
    DWORD proc_id;
    HANDLE h_process;
    wchar_t dll_path[MAX_PATH];
    void* allocated_memory;

    void getProcID();
    void error(const wchar_t* error_title, const wchar_t* error_msg);
    bool DLLExists(const wchar_t* file_name);
    void openProcess();
    void allocateMemory();
    void writeMemory();
    void createRemoteThread();
    void cleanup();
};
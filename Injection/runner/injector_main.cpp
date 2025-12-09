#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

void usage() {
    std::wcout << L"Usage: injector.exe <PID> <DLL_PATH>" << std::endl;
}

// Helper to convert string to wstring
std::wstring s2ws(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        usage();
        return 1;
    }

    DWORD pid = static_cast<DWORD>(std::atoi(argv[1]));
    std::string dllPathStr = argv[2];
    
    // Convert to absolute path if needed, but for now assume absolute path is passed
    // It's safer to use wide strings for paths in Windows
    std::wstring dllPath = s2ws(dllPathStr);

    std::wcout << L"Target PID: " << pid << std::endl;
    std::wcout << L"DLL Path: " << dllPath << std::endl;

    // 1. Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // 2. Allocate memory for DLL path in target process
    // Size includes null terminator
    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remoteMemory = VirtualAllocEx(hProcess, NULL, pathSize, MEM_COMMIT, PAGE_READWRITE);
    if (!remoteMemory) {
        std::cerr << "Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 3. Write DLL path to target process memory
    if (!WriteProcessMemory(hProcess, remoteMemory, dllPath.c_str(), pathSize, NULL)) {
        std::cerr << "Failed to write to target process memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 4. Create remote thread to load the DLL
    // Get address of LoadLibraryW
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, loadLibraryAddr, remoteMemory, 0, NULL);
    if (!hThread) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 5. Wait for injection to complete
    WaitForSingleObject(hThread, INFINITE);

    // Get exit code to check if LoadLibrary succeeded
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    
    // Clean up
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    if (exitCode == 0) {
        std::cerr << "Remote LoadLibraryW failed." << std::endl;
        return 1;
    }

    std::cout << "Injection Successful!" << std::endl;
    return 0;
}

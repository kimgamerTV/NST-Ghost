#include "injection.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <shlobj.h> // For getting temp directory
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

// --- MinHook Globals & Detours (Windows Only) ---
#ifdef _WIN32

// Typedef for the original CreateFileW function
typedef HANDLE(WINAPI* CreateFileW_t)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
);

// Pointer to the original CreateFileW
CreateFileW_t fpCreateFileW = NULL;

// Helper to check string ending
bool EndsWithW(const std::wstring& fullString, const std::wstring& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    return false;
}

// Helper to get Temp directory
std::wstring GetTempDirectoryW() {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath);
}

// Helper to read file content (using WinAPI to avoid issues with specialized file access)
std::string ReadFileContent(const std::wstring& path) {
    HANDLE hFile = fpCreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return "";

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        CloseHandle(hFile);
        return "";
    }

    std::vector<char> buffer(fileSize);
    DWORD bytesRead;
    ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL);
    CloseHandle(hFile);

    return std::string(buffer.begin(), buffer.end());
}

// Our Detour Function
HANDLE WINAPI DetourCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
) {
    if (lpFileName != NULL) {
        std::wstring fileName(lpFileName);
        
        // Intercept index.html to inject our payload
        if (EndsWithW(fileName, L"index.html")) {
            // Check if we've already created our injected file to avoid infinite loops 
            // if we were to open it again (though we use a different name)
            
            // 1. Read the Original File
            // We use the original function pointer to query it, to avoid recursion loop if we used a high level wrapper
            std::string content = ReadFileContent(fileName);
            
            if (!content.empty()) {
                // 2. Inject Payload
                // Simple injection: before </body> or </html>
                std::string payload = "\n<script>\n"
                                      "console.log('[NST] Injected Successfully');\n"
                                      "// Connect to our IPC Interface here\n"
                                      "</script>\n";
                
                size_t insertPos = content.rfind("</body>");
                if (insertPos == std::string::npos) insertPos = content.rfind("</html>");
                
                if (insertPos != std::string::npos) {
                    content.insert(insertPos, payload);
                } else {
                    content += payload; // fallback
                }

                // 3. Write to Temp File
                std::wstring tempPath = GetTempDirectoryW() + L"nst_index_injected.html";
                
                HANDLE hTemp = fpCreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hTemp != INVALID_HANDLE_VALUE) {
                    DWORD bytesWritten;
                    WriteFile(hTemp, content.c_str(), content.length(), &bytesWritten, NULL);
                    CloseHandle(hTemp);

                    // 4. Redirect the call to our temp file
                    // We must call the Original function with the NEW path
                    return fpCreateFileW(
                        tempPath.c_str(), 
                        dwDesiredAccess, 
                        dwShareMode, 
                        lpSecurityAttributes, 
                        OPEN_EXISTING, // Force open existing for temp
                        dwFlagsAndAttributes, 
                        hTemplateFile
                    );
                }
            }
        }
    }

    // Call original for everything else
    return fpCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void Payload::SetupHooks() {
    if (MH_Initialize() != MH_OK) {
        // OutputDebugStringA("MinHook Initialization Failed");
        return;
    }

    if (MH_CreateHook(
            (LPVOID)CreateFileW, 
            (LPVOID)&DetourCreateFileW, 
            reinterpret_cast<LPVOID*>(&fpCreateFileW)) != MH_OK) {
        // OutputDebugStringA("CreateHook Failed");
        return;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        // OutputDebugStringA("EnableHook Failed");
        return;
    }
}

void Payload::TeardownHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
#endif
// ----------------------------------------

Payload& Payload::instance()
{
    static Payload instance;
    return instance;
}

Payload::Payload() : m_running(false), m_socketFd(INVALID_SOCKET)
{
}

Payload::~Payload()
{
    stop();
}

void Payload::start()
{
    if (m_running) return;
    
#ifdef _WIN32
    SetupHooks();
#endif

    m_running = true;
    m_thread = std::thread(&Payload::workerThread, this);
    m_thread.detach(); // Detach execution so it runs independently
}

void Payload::stop()
{
    m_running = false;

#ifdef _WIN32
    TeardownHooks();
#endif

    if (m_socketFd != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socketFd);
#else
        close(m_socketFd);
#endif
        m_socketFd = INVALID_SOCKET;
    }
}

void Payload::workerThread()
{
    // Wait a bit for game initialization
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    while (m_running) {
        connectAndLoop();
        // If disconnected, retry after delay
        if (m_running) {
             std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}

void Payload::connectAndLoop()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
#endif

    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd == INVALID_SOCKET) {
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(14478); 
    
    // Convert IPv4 and IPv6 addresses from text to binary form
#ifdef _WIN32
    InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
#else
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
#endif

    if (connect(m_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
#ifdef _WIN32
        closesocket(m_socketFd);
#else
        close(m_socketFd);
#endif
        return;
    }

    // Handshake
    std::string handshake = "HELLO_FROM_NATIVE_PAYLOAD";
    send(m_socketFd, handshake.c_str(), handshake.length(), 0);

    char buffer[4096];
    while (m_running) {
        // Blocking read
        int bytesReceived = recv(m_socketFd, buffer, 4096, 0);
        if (bytesReceived <= 0) {
            break; // Disconnected
        }
        
        std::string receivedData(buffer, bytesReceived);
        // Process received data
    }

#ifdef _WIN32
    closesocket(m_socketFd);
    WSACleanup();
#else
    close(m_socketFd);
#endif
}

void Payload::log(const std::string& message)
{
    if (m_socketFd != INVALID_SOCKET) {
        send(m_socketFd, message.c_str(), message.length(), 0);
    }
}

void StartPayload()
{
    Payload::instance().start();
}

// Entry Point for Windows
#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        StartPayload();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

// Entry Point for Linux
#ifdef __linux__
__attribute__((constructor))
void library_init()
{
    StartPayload();
}
#endif

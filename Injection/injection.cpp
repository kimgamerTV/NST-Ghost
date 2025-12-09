#include "injection.h"
#include <iostream>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
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
    m_running = true;
    m_thread = std::thread(&Payload::workerThread, this);
    m_thread.detach(); // Detach execution so it runs independently
}

void Payload::stop()
{
    m_running = false;
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
        // Process received data (e.g. translated text)
        // Hook logic would act here to replace game text
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

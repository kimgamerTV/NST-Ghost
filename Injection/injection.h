#ifndef INJECTION_H
#define INJECTION_H

#include "Injection_global.h"
#include <QObject>
#include <thread>
#include <atomic>
#include <string>
#include <map>
#include <mutex>

#ifdef _WIN32
    #include <windows.h>
    #include "min_hook/MinHook.h"
#endif

class Payload
{
public:
    static Payload& instance();
    void start();
    void stop();
    void log(const std::string& message);

private:
    Payload();
    ~Payload();
    void workerThread();
    void connectAndLoop();
    
    // Hooking internals
#ifdef _WIN32
    static void SetupHooks();
    static void TeardownHooks();
#endif

    std::atomic<bool> m_running;
    std::thread m_thread;
#ifdef _WIN32
    uintptr_t m_socketFd;
#else
    int m_socketFd;
#endif
}; 

// C-style export for manual loading if needed
extern "C" INJECTION_EXPORT void StartPayload();

#endif // INJECTION_H

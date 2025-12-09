#ifndef INJECTION_H
#define INJECTION_H

#include "Injection_global.h"
#include <string>
#include <thread>
#include <atomic>

class INJECTION_EXPORT Payload
{
public:
    static Payload& instance();
    void start();
    void stop();

    // Send formatted message to the IPC server
    void log(const std::string& message);

private:
    Payload();
    ~Payload();
    Payload(const Payload&) = delete;
    Payload& operator=(const Payload&) = delete;

    void workerThread();
    void connectAndLoop();

    std::atomic<bool> m_running;
    std::thread m_thread;
    int m_socketFd;
};

// C-style export for manual loading if needed
extern "C" INJECTION_EXPORT void StartPayload();

#endif // INJECTION_H

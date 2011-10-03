#include "thread.h"


    PingThread::PingThread(QObject *parent, QString host) : QThread(parent) {
        this->hostname = host;
    }

    PingThread::~PingThread() {
        if (isRunning()) {
        quit();
        wait();
        }
    }

    void PingThread::run()
    {
        this->ping.ErrorValue = 0;
        this->ping.Time = QDateTime::currentDateTime();

        WSAData wsaData;
        if (WSAStartup(MAKEWORD(1, 1), &wsaData) == 0)
        {
            // Load the ICMP.DLL
            HINSTANCE hIcmp = LoadLibraryA("ICMP.DLL");
            if (hIcmp == 0) {
                this->ping.Message = "Unable to locate ICMP.DLL!";
                this->ping.ErrorValue = 2;
                return;
            }

            // Look up an IP address for the given host name
            struct hostent* phe;
            if ((phe = gethostbyname(hostname.toLocal8Bit().data())) == 0) {
                this->ping.Message = "Could not find IP address for " + hostname;
                this->ping.ErrorValue = 3;
                return;
            }

            // Get handles to the functions inside ICMP.DLL that we'll need
            typedef HANDLE (WINAPI* pfnHV)(VOID);
            typedef BOOL (WINAPI* pfnBH)(HANDLE);
            typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD,
                    PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD); // evil, no?
            pfnHV pIcmpCreateFile;
            pfnBH pIcmpCloseHandle;
            pfnDHDPWPipPDD pIcmpSendEcho;
            pIcmpCreateFile = (pfnHV)GetProcAddress(hIcmp,
                    "IcmpCreateFile");
            pIcmpCloseHandle = (pfnBH)GetProcAddress(hIcmp,
                    "IcmpCloseHandle");
            pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(hIcmp,
                    "IcmpSendEcho");
            if ((pIcmpCreateFile == 0) || (pIcmpCloseHandle == 0) ||
                    (pIcmpSendEcho == 0)) {
                this->ping.Message = "Failed to get proc addr for function.";
                this->ping.ErrorValue = 4;
                return;
            }

            // Open the ping service
            HANDLE hIP = pIcmpCreateFile();
            if (hIP == INVALID_HANDLE_VALUE) {
                this->ping.Message = "Unable to open ping service.";
                this->ping.ErrorValue = 5;
                return;
            }

            // Build ping packet
            char acPingBuffer[64];
            memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
            PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(
                    GMEM_FIXED | GMEM_ZEROINIT,
                    sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));

            if (pIpe == 0) {
                this->ping.Message = "Failed to allocate global ping packet buffer.";
                this->ping.ErrorValue = 6;
                return;
            }
            pIpe->Data = acPingBuffer;
            pIpe->DataSize = sizeof(acPingBuffer);

            // Send the ping packet
            DWORD dwStatus = pIcmpSendEcho(hIP, *((DWORD*)phe->h_addr_list[0]),
                    acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
                    sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), 5000);
            if (dwStatus != 0) {
                this->ping.Host = QString::number(int(LOBYTE(LOWORD(pIpe->Address)))) + "." +
                        QString::number(int(HIBYTE(LOWORD(pIpe->Address)))) + "." +
                        QString::number(int(LOBYTE(HIWORD(pIpe->Address)))) + "." +
                        QString::number(int(HIBYTE(HIWORD(pIpe->Address))));
                this->ping.PingTime = pIpe->RoundTripTime;

            }
            else {
                this->ping.Message = "Error obtaining info from ping packet.";
                this->ping.ErrorValue = 1;
            }

            // Shut down...
            GlobalFree(pIpe);
            FreeLibrary(hIcmp);
            WSACleanup();
        }
        else
        {
            this->ping.ErrorValue = 7;
        }

        return;
    }


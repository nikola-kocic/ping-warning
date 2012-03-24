#include "ping.h"

Ping::Ping(QObject *parent)
    : QObject(parent)
    , m_hostname("www.google.com")
{
}

void Ping::setHostName(const QString &hostname)
{
    m_hostname = hostname;
}

void Ping::run()
{
    Packet packetData;
    packetData.ErrorValue = 0;
    packetData.Time = QDateTime::currentDateTime();

    WSAData wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) == 0)
    {
        // Load the ICMP.DLL
        HINSTANCE hIcmp = LoadLibraryA("ICMP.DLL");
        if (hIcmp == 0)
        {
            packetData.Message = "Unable to locate ICMP.DLL!";
            packetData.ErrorValue = 2;
            emit finished(packetData);
            return;
        }

        // Look up an IP address for the given host name
        struct hostent* phe;
        if ((phe = gethostbyname(m_hostname.toLocal8Bit().data())) == 0)
        {
            packetData.Message = "Could not find IP address for " + m_hostname;
            packetData.ErrorValue = 3;
            emit finished(packetData);
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
        pIcmpCreateFile = (pfnHV)GetProcAddress(hIcmp, "IcmpCreateFile");
        pIcmpCloseHandle = (pfnBH)GetProcAddress(hIcmp, "IcmpCloseHandle");
        pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(hIcmp, "IcmpSendEcho");
        if ((pIcmpCreateFile == 0) || (pIcmpCloseHandle == 0) || (pIcmpSendEcho == 0))
        {
            packetData.Message = "Failed to get proc addr for function.";
            packetData.ErrorValue = 4;
            emit finished(packetData);
            return;
        }

        // Open the ping service
        HANDLE hIP = pIcmpCreateFile();
        if (hIP == INVALID_HANDLE_VALUE)
        {
            packetData.Message = "Unable to open ping service.";
            packetData.ErrorValue = 5;
            emit finished(packetData);
            return;
        }

        // Build ping packet
        char acPingBuffer[64];
        memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
        PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)GlobalAlloc(
                    GMEM_FIXED | GMEM_ZEROINIT,
                    sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer));

        if (pIpe == 0)
        {
            packetData.Message = "Failed to allocate global ping packet buffer.";
            packetData.ErrorValue = 6;
            emit finished(packetData);
            return;
        }
        pIpe->Data = acPingBuffer;
        pIpe->DataSize = sizeof(acPingBuffer);

        // Send the ping packet
        DWORD dwStatus = pIcmpSendEcho(hIP, *((DWORD*)phe->h_addr_list[0]),
                                       acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
                                       sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), 5000);
        if (dwStatus != 0)
        {
            packetData.Host = QString::number(int(LOBYTE(LOWORD(pIpe->Address)))) + "." +
                    QString::number(int(HIBYTE(LOWORD(pIpe->Address)))) + "." +
                    QString::number(int(LOBYTE(HIWORD(pIpe->Address)))) + "." +
                    QString::number(int(HIBYTE(HIWORD(pIpe->Address))));
            packetData.PingTime = pIpe->RoundTripTime;

        }
        else
        {
            packetData.Message = "Error obtaining info from ping packet.";
            packetData.ErrorValue = 1;
        }

        // Shut down...
        GlobalFree(pIpe);
        FreeLibrary(hIcmp);
        WSACleanup();
    }
    else
    {
        packetData.ErrorValue = 7;
    }

    emit finished(packetData);
    return;
}


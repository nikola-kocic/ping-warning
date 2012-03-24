#ifndef THREAD_H
#define THREAD_H

#include "icmpdefs.h"
#include <QString>
#include <QDateTime>

struct Packet
{
    QString Message;
    unsigned ErrorValue;
    unsigned long PingTime;
    QString Host;
    QDateTime Time;
};

class Ping
{
public:
    static Packet pingHost(const QString &hostname);
};

#endif // THREAD_H

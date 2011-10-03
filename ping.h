#ifndef THREAD_H

#define THREAD_H

#include "icmpdefs.h"
#include <QString>
#include <QDateTime>

class Packet
{
public:
    QString Message;
    unsigned ErrorValue;
    unsigned long PingTime;
    QString Host;
    QDateTime Time;
};

class Ping : public QObject
{
    Q_OBJECT

public :
    Ping(QObject *parent = 0) {}
    void run(const QString &hostname = "www.google.com");
    Packet ping;

signals:
    void finished();
};

#endif // THREAD_H


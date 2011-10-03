#ifndef THREAD_H

#define THREAD_H

#include "icmpdefs.h"
#include <QString>
#include <QDateTime>
#include <QThread>

class Packet
{
public:
    QString Message;
    unsigned ErrorValue;
    unsigned long PingTime;
    QString Host;
    QDateTime Time;
};

class PingThread : public QThread
{
    Q_OBJECT

public :
    PingThread(QObject *parent = 0, QString host = "www.google.com");
    ~PingThread();
    void run();
    Packet ping;

private:
    QString hostname;
};

#endif // THREAD_H


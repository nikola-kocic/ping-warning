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
    Ping(QObject *parent = 0, const QString& host = "www.google.com");

private:
    QString hostname;

public slots:
    void run();

signals:
    void finished(Packet);
};

#endif // THREAD_H


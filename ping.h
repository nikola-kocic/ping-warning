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
    Ping(const QString& host = "www.google.com", QObject *parent = 0);

private:
    QString m_hostname;

public slots:
    void run();

signals:
    void finished(Packet);
};

#endif // THREAD_H


#include "myapp.h"

#include <QFile>
#include <QDateTime>
#include <QTextStream>

void  MyApp::commitData(QSessionManager &sm)
{
    if(this->pingActive==true)
    {
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n\n");
    }
    qApp;
}

void Log(QString text)
{
    QFile log("ping.txt");
    log.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&log);
    out << text + "\n";
    log.close();
}

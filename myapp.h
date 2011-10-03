#ifndef MYAPP_H
#define MYAPP_H

#include <QApplication>

#if defined(qMyApp)
#undef qMyApp
#endif
#define qMyApp (static_cast<MyApp *>(QCoreApplication::instance()))

void Log(QString text);

class MyApp : public QApplication
{
        Q_OBJECT
public:
        MyApp( int &argc, char **argv ) : QApplication( argc, argv ) {
            pingActive = false;
        }
        ~MyApp() {}
        bool pingActive;
        void commitData(QSessionManager &sm);
        static QCoreApplication *instance() { return static_cast<MyApp *>(QCoreApplication::instance()); }
};

#endif // MYAPP_H

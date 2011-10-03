#include "mainwindow.h"
#include "myapp.h"

int main(int argc, char *argv[])
{
    MyApp a(argc, argv);
//    QApplication a(argc, argv);
    MainWindow w(0);
    w.show();

    return a.exec();
}

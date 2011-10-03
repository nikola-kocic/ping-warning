#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ping.h"
#include "myapp.h"

#include <QMainWindow>
#include <QtCore/QTimer>
#include <QThread>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ping *ping;

private slots:
    void on_pushButtonStart_clicked();
    void onTimer();
    void alert();
    void threadDone(Packet);

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    unsigned limit;
    QString host;
    QThread *pingThread;

public slots:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

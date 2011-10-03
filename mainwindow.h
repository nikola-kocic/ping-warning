#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "thread.h"
#include "myapp.h"

#include <QMainWindow>
#include <QtCore/QTimer>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    PingThread *thread;

private slots:
    void on_pushButtonStart_clicked();
    void onTimer();
    void alert();
    void threadDone();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    unsigned limit;
    QString host;

public slots:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ping.h"

#include <QMainWindow>
#include <QtCore/QTimer>
#include <QThread>
#include <QSessionManager>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onCommitData(QSessionManager &sm);
    void on_pushButtonStart_clicked();
    void onTimer();
    void alert();
    void threadDone(Packet);

private:
    void Log(QString text);
    Ui::MainWindow *ui;
    Ping *m_ping;
    QTimer *m_timer_ping;
    unsigned m_limit_ms;
    QString m_host_name;
    QThread *m_thread_ping;
    bool m_flag_ping_active;

public slots:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

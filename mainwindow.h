#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ping.h"

#include <QMainWindow>
#include <QTimer>
#include <QSessionManager>
#include <QFutureWatcher>

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
    void pingFinished(int i);
    void onCommitData(QSessionManager &sm);
    void on_pushButtonStart_clicked();
    void onTimer();
    void alert();

private:
    void Log(QString text);
    void cleanup();
    Ui::MainWindow *ui;
    static const int TIMER_INTERVAL = 1000;
    QTimer *m_timer_ping;
    unsigned m_limit_ms;
    QString m_host_name;
    QFutureWatcher<Packet> *m_watcher_ping;
    bool m_flag_ping_active;

public slots:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

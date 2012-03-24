#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrentRun>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_timer_ping(new QTimer(this))
    , m_limit_ms(100)
    , m_host_name("")
    , m_watcher_ping(new QFutureWatcher<Packet>(this))
    , m_flag_ping_active(false)
{
    ui->setupUi(this);
    connect(m_watcher_ping, SIGNAL(resultReadyAt(int)), this, SLOT(pingFinished(int)));
    connect(m_timer_ping, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), this, SLOT(onCommitData(QSessionManager&)), Qt::DirectConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonStart_clicked()
{
    QString pingInfo = "*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging ";

    if (m_flag_ping_active == false)
    {
        m_limit_ms = (unsigned)ui->spinBoxPingLimit->value();
        m_host_name = ui->lineEditHost->text();

        m_flag_ping_active = true;
        m_timer_ping->start(TIMER_INTERVAL);
        ui->pushButtonStart->setText("Stop");
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPingLimit->setEnabled(false);
        pingInfo += "Started ***";
    }
    else
    {
        m_flag_ping_active = false;
        m_timer_ping->stop();
        ui->pushButtonStart->setText("Start");
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPingLimit->setEnabled(true);
        pingInfo += "Stopped ***\n";
    }

    ui->plainTextEditTimeouts->appendPlainText(pingInfo);
    ui->plainTextEditPing->appendPlainText(pingInfo);

    Log(pingInfo);

    ui->statusBar->showMessage(pingInfo);
}

void MainWindow::onTimer()
{
    m_timer_ping->stop();
    m_watcher_ping->setFuture(QtConcurrent::run(Ping::pingHost, m_host_name));
}

void MainWindow::pingFinished(int i)
{
    Packet p = m_watcher_ping->resultAt(i);
    if (m_flag_ping_active == true)
    {
        if (p.ErrorValue == 0)
        {
            QString pingInfo = p.Time.toString(Qt::SystemLocaleShortDate) + ", "
                    + p.Host + ", " + QString::number(p.PingTime);

            ui->plainTextEditPing->appendPlainText(pingInfo);
            if (p.PingTime > m_limit_ms)
            {
                this->alert();
                ui->plainTextEditTimeouts->appendPlainText(pingInfo);
                Log(pingInfo);
            }
        }
        else
        {
            this->alert();
            QString pingInfo = p.Time.toString(Qt::SystemLocaleShortDate) + ", " + p.Message;

            ui->plainTextEditTimeouts->appendPlainText(pingInfo);
            ui->plainTextEditPing->appendPlainText(pingInfo);

            Log(pingInfo);
        }
        m_timer_ping->start(TIMER_INTERVAL);
    }
}

void MainWindow::alert()
{
    if (ui->checkBoxAlert->isChecked())
    {
        QApplication::beep();
        QApplication::alert(this);
    }
}

void MainWindow::cleanup()
{
    if (m_flag_ping_active == true)
    {
        m_flag_ping_active = false;
        m_timer_ping->stop();
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    cleanup();
}

void MainWindow::onCommitData(QSessionManager &sm)
{
    Q_UNUSED(sm);
    cleanup();
}

void MainWindow::Log(QString text)
{
    QFile log("ping.txt");
    log.open(QIODevice::Append | QIODevice::Text);
    log.write(text.toUtf8() + "\n");
    log.close();
}

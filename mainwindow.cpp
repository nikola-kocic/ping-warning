#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TIMER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_ping(0)
    , m_timer_ping(new QTimer(this))
    , m_limit_ms(100)
    , m_host_name("")
    , m_thread_ping(new QThread())
    , m_flag_ping_active(false)
{
    qRegisterMetaType<Packet>("Packet");
    ui->setupUi(this);

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
        m_host_name =  ui->lineEditHost->text();


        m_ping = new Ping(this->m_host_name);
        m_ping->moveToThread(m_thread_ping);
        connect(m_thread_ping, SIGNAL(started()), m_ping, SLOT(run()));
        connect(m_ping, SIGNAL(finished(Packet)), this, SLOT(threadDone(Packet)));


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
        m_ping->disconnect();
        delete m_ping;
    }

    ui->plainTextEditTimeouts->appendPlainText(pingInfo);
    ui->plainTextEditPing->appendPlainText(pingInfo);

    Log(pingInfo);

    ui->statusBar->showMessage(pingInfo);
}

void MainWindow::onTimer()
{
    m_timer_ping->stop();
    m_thread_ping->start();
}


void MainWindow::threadDone(Packet p)
{
    m_thread_ping->exit();
    if (m_flag_ping_active == true) {
        if (p.ErrorValue == 0)
        {
            QString pingInfo = "Time: " + p.Time.toString(Qt::SystemLocaleShortDate) +
                    ", Host: " + p.Host +
                    ", Ping: " + QString::number(p.PingTime);

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_flag_ping_active == true)
    {
        m_flag_ping_active = false;
        m_timer_ping->stop();
        m_thread_ping->exit();
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n");
    }
}

void MainWindow::onCommitData(QSessionManager &sm)
{
    if (m_flag_ping_active == true)
    {
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n\n");
    }
}

void MainWindow::Log(QString text)
{
    QFile log("ping.txt");
    log.open(QIODevice::Append | QIODevice::Text);
    log.write(text.toUtf8() + "\n");
    log.close();
}

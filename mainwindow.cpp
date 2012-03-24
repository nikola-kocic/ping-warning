#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TIMER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ping(0)
    , timer(new QTimer(this))
    , limit(100)
    , host("")
    , pingThread(new QThread())
    , m_flag_ping_active(false)
{
    qRegisterMetaType<Packet>("Packet");
    ui->setupUi(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), this, SLOT(onCommitData(QSessionManager&)), Qt::DirectConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButtonStart_clicked()
{
    QString pingInfo = "*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging ";

    if(m_flag_ping_active == false)
    {
        limit = (unsigned)ui->spinBoxPingLimit->value();
        host =  ui->lineEditHost->text();


        ping = new Ping(this->host);
        ping->moveToThread(pingThread);
        connect(pingThread, SIGNAL(started()), ping, SLOT(run()));
        connect(ping, SIGNAL(finished(Packet)), this, SLOT(threadDone(Packet)));


        m_flag_ping_active = true;
        timer->start(TIMER_INTERVAL);
        ui->pushButtonStart->setText("Stop");
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPingLimit->setEnabled(false);
        pingInfo += "Started ***";
    }
    else
    {
        m_flag_ping_active = false;
        timer->stop();
        ui->pushButtonStart->setText("Start");
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPingLimit->setEnabled(true);
        pingInfo += "Stopped ***\n";
        ping->disconnect();
        delete ping;
    }

    ui->plainTextEditTimeouts->appendPlainText(pingInfo);
    ui->plainTextEditPing->appendPlainText(pingInfo);

    Log(pingInfo);

    ui->statusBar->showMessage(pingInfo);
}

void MainWindow::onTimer()
{
    timer->stop();
    pingThread->start();
}


void MainWindow::threadDone(Packet p)
{
    pingThread->exit();
    if(m_flag_ping_active == true) {
        if(p.ErrorValue == 0)
        {
            QString pingInfo = "Time: " + p.Time.toString(Qt::SystemLocaleShortDate) +
                    ", Host: " + p.Host +
                    ", Ping: " + QString::number(p.PingTime);

            ui->plainTextEditPing->appendPlainText(pingInfo);
            if(p.PingTime > limit)
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
        timer->start(TIMER_INTERVAL);
    }
}

void MainWindow::alert()
{
    if(ui->checkBoxAlert->isChecked())
    {
        QApplication::beep();
        QApplication::alert(this);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_flag_ping_active == true)
    {
        m_flag_ping_active = false;
        timer->stop();
        pingThread->exit();
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n");
    }
}

void MainWindow::onCommitData(QSessionManager &sm)
{
    if(m_flag_ping_active == true)
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

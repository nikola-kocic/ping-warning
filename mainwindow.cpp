#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TIMER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qMyApp->pingActive = false;
    ui->setupUi(this);
    timer = new QTimer(this);
    thread = new QThread();
    ping = new Ping(this);
    ping->moveToThread(thread);
    connect(ping, SIGNAL(finished()), this, SLOT(threadDone()));

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::threadDone()
{
    if(qMyApp->pingActive == true) {
        if(ping->packetData.ErrorValue == 0)
        {
            QString pingInfo = "Time: " + ping->packetData.Time.toString(Qt::SystemLocaleShortDate) +
                    ", Host: " + ping->packetData.Host +
                    ", Ping: " + QString::number(ping->packetData.PingTime);

            ui->plainTextEditPing->appendPlainText(pingInfo);
            if(ping->packetData.PingTime > limit)
            {
                this->alert();
                ui->plainTextEditTimeouts->appendPlainText(pingInfo);
                Log(pingInfo);
            }
        }
        else
        {
            this->alert();
            QString pingInfo = ping->packetData.Time.toString(Qt::SystemLocaleShortDate) + ", " + ping->packetData.Message;

            ui->plainTextEditTimeouts->appendPlainText(pingInfo);
            ui->plainTextEditPing->appendPlainText(pingInfo);

            Log(pingInfo);
        }
        timer->start(TIMER_INTERVAL);
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
    QString ping = "*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging ";

    if(qMyApp->pingActive == false)
    {
        limit = (unsigned)ui->spinBoxPingLimit->value();
        host =  ui->lineEditHost->text();
        qMyApp->pingActive = true;
        timer->start(TIMER_INTERVAL);
        ui->pushButtonStart->setText("Stop");
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPingLimit->setEnabled(false);
        ping += "Started ***";
    }
    else
    {
        qMyApp->pingActive = false;
        timer->stop();
        ui->pushButtonStart->setText("Start");
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPingLimit->setEnabled(true);
        ping += "Stopped ***\n";
    }

    ui->plainTextEditTimeouts->appendPlainText(ping);
    ui->plainTextEditPing->appendPlainText(ping);

    Log(ping);

    ui->statusBar->showMessage(ping);
}

void MainWindow::onTimer()
{
    timer->stop();

    ping->run(this->host);


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(qMyApp->pingActive == true)
    {
        qMyApp->pingActive = false;
        timer->stop();
        thread->exit();
        Log("*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging Stopped ***\n");
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

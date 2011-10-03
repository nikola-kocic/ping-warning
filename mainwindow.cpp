#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TIMER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<Packet>("Packet");
    qMyApp->pingActive = false;
    ui->setupUi(this);
    timer = new QTimer(this);
    thread = new QThread();

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::threadDone(Packet p)
{
    thread->exit();
    if(qMyApp->pingActive == true) {
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

void MainWindow::on_pushButtonStart_clicked()
{
    QString pingInfo = "*** " + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + ", Logging ";

    if(qMyApp->pingActive == false)
    {
        limit = (unsigned)ui->spinBoxPingLimit->value();
        host =  ui->lineEditHost->text();


        ping = new Ping(0, this->host);
        ping->moveToThread(thread);
        connect(thread, SIGNAL(started()), ping, SLOT(run()));
        connect(ping, SIGNAL(finished(Packet)), this, SLOT(threadDone(Packet)));


        qMyApp->pingActive = true;
        timer->start(TIMER_INTERVAL);
        ui->pushButtonStart->setText("Stop");
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPingLimit->setEnabled(false);
        pingInfo += "Started ***";
    }
    else
    {
        qMyApp->pingActive = false;
        timer->stop();
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
    qWarning("ontimer");
    timer->stop();

    thread->start();
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

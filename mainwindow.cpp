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
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::threadDone()
{
    if(qMyApp->pingActive == true) {
        if(thread->ping.ErrorValue == 0)
        {
            QString ping = "Time: " + thread->ping.Time.toString(Qt::SystemLocaleShortDate) +
                    ", Host: " + thread->ping.Host +
                    ", Ping: " + QString::number(thread->ping.PingTime);

            ui->plainTextEditPing->appendPlainText(ping);
            if(thread->ping.PingTime > limit)
            {
                this->alert();
                ui->plainTextEditTimeouts->appendPlainText(ping);
                Log(ping);
            }
        }
        else
        {
            this->alert();
            QString ping = thread->ping.Time.toString(Qt::SystemLocaleShortDate) + ", " + thread->ping.Message;

            ui->plainTextEditTimeouts->appendPlainText(ping);
            ui->plainTextEditPing->appendPlainText(ping);

            Log(ping);
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

    thread = new PingThread(this, this->host);
    connect(thread, SIGNAL(finished()), this, SLOT(threadDone()));
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

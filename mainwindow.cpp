#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)/*, socket(new QUdpSocket(this)) */{
    ui->setupUi(this);
    port = 8000;
    //socket = new QUdpSocket(this);
    /*socket->bind(QHostAddress::LocalHost, port);

    connect(socket, SIGNAL(readyRead()), SLOT(read()));*/
}

MainWindow::~MainWindow() {
    delete ui;
}
/*
void MainWindow::SendMessage() {
    //void send(QString str, qint8 type) {
    QString str = "Hello, robot!";
    qint8 type = 0;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << qint64(0);
    out << qint8(type);
    out << str;
    out.device()->seek(qint64(0));
    out << qint64(data.size() - sizeof(qint64));
    socket->writeDatagram(data, QHostAddress::LocalHost, port);
}*/

void MainWindow::on_sendButton_clicked()
{
    //SendMessage();
}

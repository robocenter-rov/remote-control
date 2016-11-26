#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QtNetwork/QUdpSocket>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void sendMessage();

public slots:
    void readMessage();

private slots:
    void onSendButtonClicked();

private:
    Ui::MainWindow *_ui;
    QUdpSocket *_socket;
};

#endif // MAINWINDOW_H

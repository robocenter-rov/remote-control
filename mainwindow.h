#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void SendMessage();
public slots:
    void ReadyRead();
private slots:
    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
};

#endif // MAINWINDOW_H

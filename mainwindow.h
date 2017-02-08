#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "messages.h"
#include <QByteArray>
#include <QDataStream>
#include <QCamera>
#include <QCameraInfo>
#include <QWidget>
#include <QtMultimediaWidgets>
#include <QtGui/QtGui>
#include "robocamera.h"
#include "communicator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private:   
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);

    Ui::MainWindow *_ui;
    RoboCamera *_mainCamera;
    Communicator *_communicator;
};

#endif // MAINWINDOW_H

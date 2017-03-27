#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
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
private slots:
    void updateDepth();    
    void onStartButtonClick(bool);
    void onTaskTimeout();
private:   
    void cameraInit();
    void loadQSS();
    void clearScene();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, QColor msgColor = QColor(0, 204, 102));

    Ui::MainWindow *_ui;
    RoboCamera *_mainCamera;
    RoboCamera *_extraCamera;
    Communicator *_communicator;
    QTimer *_depthTimer;
    QTimer *_taskTimer;
    double _currentDepth; // Temp var. DO: why _ui->mainView->scene->height() return different values after redrawing depth
    double _sceneHeight, _sceneWidth; // Temp vars too
};

#endif // MAINWINDOW_H

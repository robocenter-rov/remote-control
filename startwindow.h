#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

private slots:
    void onMainPilotButtonClicked();
private:
    void loadQSS();

    Ui::StartWindow *ui;
    MainWindow *_mainWindow;
};

#endif // STARTWINDOW_H

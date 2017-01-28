#include "basetool.h"

BaseTool::BaseTool(QWidget *parent)
{
    _iconsPath = (QCoreApplication::applicationDirPath() + "../remote-control/icons");
    _button = new QPushButton(parent);
}

BaseTool::~BaseTool()
{
    delete _button;
}

#include <iostream>
void BaseTool::onToolButtonClick(bool checked)
{
    std::cout << typeid(*this).name();
}

LineTool::LineTool(QWidget *parent):
    BaseTool(parent)
{
    _button->setText("Line");
    _button->setIconSize(QSize(30, 30));
    connect(_button, SIGNAL(clicked(bool)), this, SLOT(onToolButtonClick(bool)));
    //_button->setIcon();
}

void LineTool::draw()
{

}

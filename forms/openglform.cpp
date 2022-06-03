#include "openglform.h"
#include "ui_openglform.h"
#include "SmlGLWindowTriangle.h"

openGLForm::openGLForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::openGLForm)
{
    ui->setupUi(this);

    ///////////////////////////////////////////////////
    _glwin = new SmlGLWindowTriangle(nullptr);
    _glwin->setFlags(Qt::FramelessWindowHint);
    _glwin->create();
    _cont = QWidget::createWindowContainer(_glwin, this, Qt::FramelessWindowHint);

    ui->verticalLayout->addWidget(_cont);
}

openGLForm::~openGLForm()
{
    delete ui;
}

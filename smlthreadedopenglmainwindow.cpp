#include "smlthreadedopenglmainwindow.h"
#include "./ui_smlthreadedopenglmainwindow.h"
#include "./forms/openglform.h"
#include "./forms/testmiscform.h"

SmlThreadedOpenglMainWindow::SmlThreadedOpenglMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SmlThreadedOpenglMainWindow)
{
    ui->setupUi(this);

    ui->verticalLayoutOpenGL->addWidget(new openGLForm{this});
    ui->verticalLayoutMiscTest->addWidget(new TestMiscForm{this});
}

SmlThreadedOpenglMainWindow::~SmlThreadedOpenglMainWindow()
{
    delete ui;
}


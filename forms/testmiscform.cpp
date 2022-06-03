#include "testmiscform.h"
#include "ui_testmiscform.h"
#include "SmlAxisCoord.test.h"

TestMiscForm::TestMiscForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestMiscForm)
{
    ui->setupUi(this);
}

TestMiscForm::~TestMiscForm()
{
    delete ui;
}

void TestMiscForm::on_pushButtonTestProjMat_clicked()
{
 SmartLib::AxisCoordTest::Case0_projection();
}


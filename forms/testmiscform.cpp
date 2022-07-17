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
    ui->pushButtonTestProjMat->setEnabled(false);
    SmartLib::AxisCoordTest::Case0_projection();
    ui->pushButtonTestProjMat->setEnabled(true);
}


void TestMiscForm::on_pushButtonTesGlmColumRow_clicked()
{
    ui->pushButtonTesGlmColumRow->setEnabled(false);
    SmartLib::AxisCoordTest::Case1_glm_colum_row();
    ui->pushButtonTesGlmColumRow->setEnabled(true);
}


void TestMiscForm::on_pushButtonTestTBN_clicked()
{
    ui->pushButtonTestTBN->setEnabled(false);
    SmartLib::AxisCoordTest::Case2_TBN();
    ui->pushButtonTestTBN->setEnabled(true);
}


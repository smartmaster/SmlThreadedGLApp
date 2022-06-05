#ifndef TESTMISCFORM_H
#define TESTMISCFORM_H

#include <QWidget>

namespace Ui {
class TestMiscForm;
}

class TestMiscForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestMiscForm(QWidget *parent = nullptr);
    ~TestMiscForm();

private slots:
    void on_pushButtonTestProjMat_clicked();

    void on_pushButtonTesGlmColumRow_clicked();

    void on_pushButtonTestTBN_clicked();

private:
    Ui::TestMiscForm *ui;
};

#endif // TESTMISCFORM_H

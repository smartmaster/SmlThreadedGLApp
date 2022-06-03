#ifndef OPENGLFORM_H
#define OPENGLFORM_H

#include <QWidget>
#include <QWindow>

namespace Ui {
class openGLForm;
}

class openGLForm : public QWidget
{
    Q_OBJECT

public:
    explicit openGLForm(QWidget *parent = nullptr);
    ~openGLForm();

private:
    Ui::openGLForm *ui;
    QWindow* _glwin{ nullptr };
    QWidget* _cont{ nullptr };
};

#endif // OPENGLFORM_H

#ifndef SMLTHREADEDOPENGLMAINWINDOW_H
#define SMLTHREADEDOPENGLMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SmlThreadedOpenglMainWindow; }
QT_END_NAMESPACE

class SmlThreadedOpenglMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SmlThreadedOpenglMainWindow(QWidget *parent = nullptr);
    ~SmlThreadedOpenglMainWindow();

private:
    Ui::SmlThreadedOpenglMainWindow *ui;
};
#endif // SMLTHREADEDOPENGLMAINWINDOW_H

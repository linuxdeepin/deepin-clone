#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class TitleLabel;
class MainWindow : public DMainWindow
{
public:
    enum Status {
        SelectAction,
        SelectFile,
        WaitConfirm,
        Working,
        End
    };

    explicit MainWindow(QWidget *parent = 0);

private:
    void init();
    void setStatus(Status status);
    void next();

    Status m_currentStatus = SelectAction;

    TitleLabel *m_title;
    QStackedWidget *m_contentWidget;
    QPushButton *m_bottomButton;
};

#endif // MAINWINDOW_H

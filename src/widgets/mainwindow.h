#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectactionpage.h"

#include <DMainWindow>
#include <dpageindicator.h>

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class CloneJob;

DWIDGET_USE_NAMESPACE

class IconLabel;
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
    void setContent(QWidget *widget);
    QWidget *content() const;

    bool isError() const;

    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    Status m_currentStatus = (Status)(End + 1);
    SelectActionPage::Action m_currentMode = SelectActionPage::Backup;
    SelectActionPage::Mode m_operateObject = SelectActionPage::Disk;

    CloneJob *m_job;

    IconLabel *m_title;
    QStackedWidget *m_contentWidget;
    QPushButton *m_bottomButton;
    DPageIndicator *m_pageIndicator;
};

#endif // MAINWINDOW_H

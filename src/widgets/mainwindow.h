#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectactionpage.h"

#include <DMainWindow>
#include <dpageindicator.h>
#include <DVideoWidget>

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QMediaPlayer;
QT_END_NAMESPACE

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

    enum ButtonAction {
        None,
        Next,
        Cancel,
        Retry,
        ShowBackupFile,
        Quit,
        RestartSystem,
        RestartToLiveSystem
    };

    explicit MainWindow(QWidget *parent = 0);

    void startWithFile(const QString &source, const QString &target);

private:
    void init();
    void setStatus(Status status);
    void next();
    void setContent(QWidget *widget);
    QWidget *content() const;
    void onButtonClicked();
    void showErrorMessage(const QString &title, const QString &message);

    bool isError() const;

    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    Status m_currentStatus = (Status)(End + 1);
    SelectActionPage::Action m_currentMode = SelectActionPage::Backup;
    SelectActionPage::Mode m_operateObject = SelectActionPage::Disk;
    ButtonAction m_buttonAction = None;

    QString m_sourceFile;
    QString m_targetFile;

    IconLabel *m_title;
    QLabel *m_subTitle;
    QStackedWidget *m_contentWidget;
    QPushButton *m_cancelButton;
    QPushButton *m_bottomButton;
    QMediaPlayer *m_player;
    DVideoWidget *m_loadingIndicator;
    DPageIndicator *m_pageIndicator;
};

#endif // MAINWINDOW_H

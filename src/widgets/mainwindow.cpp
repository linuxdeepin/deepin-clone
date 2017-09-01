#include "mainwindow.h"
#include "clonejob.h"
#include "iconlabel.h"
#include "selectfilepage.h"
#include "helper.h"
#include "workingpage.h"
#include "endpage.h"

#include <DDesktopServices>
#include <ddialog.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    init();
    setStatus(SelectAction);
}

void MainWindow::startWithFile(const QString &source, const QString &target)
{
    bool s_is_block = Helper::isBlockSpecialFile(m_sourceFile);
    bool s_is_disk = Helper::isDiskDevice(source);

    bool t_is_block = Helper::isBlockSpecialFile(target);
    bool t_is_disk = Helper::isDiskDevice(target);

    if (s_is_block) {
        if (t_is_block) {
            m_currentMode = SelectActionPage::Clone;
        } else {
            m_currentMode = SelectActionPage::Backup;
        }
    } else {
        m_currentMode = SelectActionPage::Restore;
    }

    if (s_is_disk || t_is_disk)
        m_operateObject = SelectActionPage::Disk;
    else
        m_operateObject = SelectActionPage::Partition;

    m_sourceFile = source;
    m_targetFile = target;

    setStatus(Working);
}

void MainWindow::init()
{
    m_title = new IconLabel(this);
    m_title->setObjectName("MainTitle");
    m_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_contentWidget = new QStackedWidget(this);
    m_cancelButton = new QPushButton(this);
    m_cancelButton->setFixedSize(width() * 0.36, height() * 0.055);
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->hide();
    m_bottomButton = new QPushButton(this);
    m_bottomButton->setFixedSize(width() * 0.36, height() * 0.055);
    m_pageIndicator = new DPageIndicator(this);
    m_pageIndicator->setPageCount(3);
    m_pageIndicator->setCurrentPage(0);
    m_pageIndicator->setPointColor(QColor("#2ca7f8"));
    m_pageIndicator->setSecondaryPointColor(QColor(150, 172, 189, 0.2 * 255));
    m_pageIndicator->setFixedSize(50, 20);

    QWidget *main_widget = new QWidget(this);

    setCentralWidget(main_widget);

    QVBoxLayout *layout = new QVBoxLayout(main_widget);

    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addWidget(m_contentWidget, 0, Qt::AlignHCenter);
    layout->addWidget(m_cancelButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_bottomButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_pageIndicator, 0, Qt::AlignHCenter);

    connect(m_cancelButton, &QPushButton::clicked, this, [this] {
        setStatus(SelectAction);
    });
    connect(m_bottomButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
}

void MainWindow::setStatus(MainWindow::Status status)
{
    switch (m_currentStatus) {
    case SelectAction: {
        m_currentMode = static_cast<SelectActionPage*>(content())->action();
        m_operateObject = static_cast<SelectActionPage*>(content())->mode();
        break;
    }
    case SelectFile: {
        SelectFilePage *page = static_cast<SelectFilePage*>(content());

        m_sourceFile = page->source();
        m_targetFile = page->target();

        if (m_sourceFile.isEmpty()) {
            showErrorMessage(tr("Error"), tr("Source file not found"));

            return;
        }

        if (m_targetFile.isEmpty()) {
            showErrorMessage(tr("Error"), tr("Target file is empty"));

            return;
        }

        if (m_sourceFile == m_targetFile) {
            showErrorMessage(tr("The source file and the destination file can not be the same file"),
                             tr("Please re-select"));

            return;
        }

        break;
    }
    default:
        break;
    }

    m_title->setIcon(QIcon());
    m_cancelButton->setVisible(status == WaitConfirm);

    switch (status) {
    case SelectAction: {
        setContent(new SelectActionPage());
        m_title->setTitle(tr("Plase Select Action"));
        m_bottomButton->setText(tr("Next"));
        m_buttonAction = Next;
        break;
    } case SelectFile: {
        SelectActionPage *page = qobject_cast<SelectActionPage*>(content());
        setContent(new SelectFilePage(m_operateObject, m_currentMode));

        if (page) {
            m_title->setTitle(page->selectedItemTitle());
            m_title->setIcon(page->selectedItemIcon(), m_title->sizeHint().height() * 2);
        }

        if (m_currentMode == SelectActionPage::Backup) {
            m_bottomButton->setText(tr("Begin Backup"));
        } else if (m_currentMode == SelectActionPage::Clone) {
            m_bottomButton->setText(tr("Begin Clone"));
        } else {
            m_bottomButton->setText(tr("Begin Restore"));
        }

        m_buttonAction = Next;
        break;
    }
    case WaitConfirm: {
        if (!Helper::isBlockSpecialFile(m_targetFile))
            return setStatus(Working);

        EndPage *page = new EndPage(EndPage::Warning);

        page->setTitle(tr("您确定要继续吗？"));
        page->setMessage(tr("还原(克隆)分区(磁盘)会格式化目标位置的所有数据，此过程不可逆也不可取消，为了您的数据安全，你一定要仔细检查您的操作，确定没问题后再继续"));
        setContent(page);
        m_title->setTitle(tr("提醒"));
        m_bottomButton->setText(tr("Containue"));
        m_buttonAction = Next;
        break;
    }
    case Working: {
        WorkingPage *page = new  WorkingPage(m_sourceFile, m_targetFile);

        connect(page, &WorkingPage::finished, this, &MainWindow::next);
        setContent(page);

        if (m_currentMode == SelectActionPage::Backup)
            m_title->setTitle(tr("正在备份"));
        else if (m_currentMode == SelectActionPage::Clone)
            m_title->setTitle(tr("正在克隆"));
        else
            m_title->setTitle(tr("正在还原"));

        m_bottomButton->setText(tr("Cancel"));
        m_buttonAction = Cancel;
        break;
    }
    case End: {
        bool is_error = isError();

        WorkingPage *worker = qobject_cast<WorkingPage*>(content());
        EndPage *page = new EndPage(is_error ? EndPage::Fail : EndPage::Success);

        if (is_error) {
            if (m_currentMode == SelectActionPage::Backup) {
                m_title->setTitle(tr("备份失败"));
            } else if (m_currentMode == SelectActionPage::Clone) {
                m_title->setTitle(tr("克隆失败"));
            } else {
                m_title->setTitle(tr("还原失败"));
            }

            page->setMessage(worker->errorString());
            m_bottomButton->setText(tr("Retry"));
            m_buttonAction = Retry;
        } else {
            if (m_currentMode == SelectActionPage::Backup) {
                m_title->setTitle(tr("备份完成"));
                m_bottomButton->setText(tr("Display Backup File"));
                m_buttonAction = ShowBackupFile;
            } else if (m_currentMode == SelectActionPage::Clone) {
                m_title->setTitle(tr("克隆完成"));
                m_bottomButton->setText(tr("Ok"));
                m_buttonAction = Quit;
            } else {
                m_title->setTitle(tr("还原完成"));
                m_bottomButton->setText(tr("Restart System"));
                m_buttonAction = RestartSystem;
            }
        }

        page->setTitle(m_title->title());
        setContent(page);
        break;
    }
    default:
        break;
    }

    m_currentStatus = status;
}

void MainWindow::next()
{
    if (m_currentStatus < End) {
        setStatus(Status(m_currentStatus + 1));
    }
}

void MainWindow::setContent(QWidget *widget)
{
    if (m_contentWidget->count() > 0)
        m_contentWidget->removeWidget(content());

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentWidget->addWidget(widget);
}

QWidget *MainWindow::content() const
{
    return m_contentWidget->count() > 0 ? m_contentWidget->widget(0) : 0;
}

void MainWindow::onButtonClicked()
{
    switch (m_buttonAction) {
    case Next:
        return next();
    case Cancel: {
        if (WorkingPage *page = qobject_cast<WorkingPage*>(content())) {
            page->cancel();
        }
        return setStatus(SelectAction);
    }
    case Retry: {
        return setStatus(Working);
    }
    case ShowBackupFile: {
        DDesktopServices::showFileItem(m_targetFile);
        return;
    }
    case Quit: {
        return qApp->quit();
    }
    case RestartSystem: {
        return qApp->quit();
    }
    default:
        break;
    }
}

void MainWindow::showErrorMessage(const QString &title, const QString &message)
{
    DDialog dialog(title, message, this);

    dialog.addButton(tr("Ok"), true);
    dialog.exec();
}

bool MainWindow::isError() const
{
    if (WorkingPage *page = qobject_cast<WorkingPage*>(content())) {
        return page->isError();
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    const QMargins &content_margins = layout()->contentsMargins();

    m_contentWidget->setFixedWidth(e->size().width() - content_margins.left() - content_margins.right());
}

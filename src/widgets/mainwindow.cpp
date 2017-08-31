#include "mainwindow.h"
#include "clonejob.h"
#include "iconlabel.h"
#include "selectfilepage.h"
#include "helper.h"
#include "workingpage.h"

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
    layout->addWidget(m_bottomButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_pageIndicator, 0, Qt::AlignHCenter);

    connect(m_bottomButton, &QPushButton::clicked, this, &MainWindow::next);
}

void MainWindow::setStatus(MainWindow::Status status)
{
    m_title->setIcon(QIcon());
    m_bottomButton->setVisible(status != WaitConfirm);

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
        break;
    }
    default:
        break;
    }

    switch (status) {
    case SelectAction: {
        setContent(new SelectActionPage());
        m_title->setTitle(tr("Plase Select Action"));
        m_bottomButton->setText(tr("Next"));
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
        break;
    }
    case WaitConfirm: {
        if (m_sourceFile.isEmpty())
            return;

        if (m_targetFile.isEmpty())
            return;

        m_title->setTitle(tr("提醒"));
        break;
    }
    case Working: {
        setContent(new  WorkingPage(m_sourceFile, m_targetFile));

        if (m_currentMode == SelectActionPage::Backup)
            m_title->setTitle(tr("正在备份"));
        else if (m_currentMode == SelectActionPage::Clone)
            m_title->setTitle(tr("正在克隆"));
        else
            m_title->setTitle(tr("正在还原"));

        m_bottomButton->setText(tr("Cancel"));

        break;
    }
    case End: {
        if (isError()) {
            m_bottomButton->setText(tr("Retry"));
        } else if (m_currentMode == SelectActionPage::Backup) {
            m_title->setTitle(tr("备份完成"));
            m_bottomButton->setText(tr("Display Backup File"));
        } else if (m_currentMode == SelectActionPage::Clone) {
            m_title->setTitle(tr("克隆完成"));
            m_bottomButton->setText(tr("Ok"));
        } else {
            m_title->setTitle(tr("还原完成"));
            m_bottomButton->setText(tr("Restart System"));
        }

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
        if (m_currentStatus == SelectFile && !Helper::isBlockSpecialFile(m_targetFile)) {
            setStatus(Status(m_currentStatus + 2));
        } else {
            setStatus(Status(m_currentStatus + 1));
        }
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

#include "mainwindow.h"
#include "clonejob.h"
#include "iconlabel.h"
#include "selectfilepage.h"
#include "helper.h"
#include "workingpage.h"
#include "endpage.h"

#include "ddiskinfo.h"
#include "dpartinfo.h"

#include <DDesktopServices>
#include <ddialog.h>
#include <anchors.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QDebug>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    init();
    setStatus(SelectAction);
}

QString parseSerialUrl(const QString &urlString, MainWindow *window = 0)
{
    if (urlString.isEmpty())
        return QString();

    const QUrl url(urlString);
    const QString &serial_number = url.host();
    const int part_index = url.port();
    const QString &path = url.path();
    const QString &device = Helper::findDiskBySerialNumber(serial_number, part_index);
    const QString &device_url = part_index >= 0 ? QString("serial://%1:%2").arg(serial_number).arg(part_index) : "serial://" + serial_number;

    if (device.isEmpty()) {
        if (window)
            window->showErrorMessage(QObject::tr("The \"%1\" device not found").arg(device_url));

        return device;
    }

    if (path.isEmpty())
        return device;

    QDir mount_point(Helper::mountPoint(device));

    if (mount_point.absolutePath().isEmpty()) {
        if (part_index >= 0)
            mount_point.setPath(QString("/mnt/%1-%2").arg(serial_number).arg(part_index));
        else
            mount_point.setPath(QString("/mnt/%1").arg(serial_number));

        if (!Helper::mountDevice(device, mount_point.absolutePath())) {
            if (window)
                window->showErrorMessage(QObject::tr("Mount the device \"%1\" to \"%2\" failed").arg(device_url).arg(mount_point.absolutePath()));

            return QString();
        }
    }

    if (mount_point.absolutePath() == "/")
        return path;

    return mount_point.absolutePath() + path;
}

QString toSerialUrl(const QString &file)
{
    if (file.isEmpty())
        return QString();

    if (Helper::isBlockSpecialFile(file)) {
        DDiskInfo info;

        if (Helper::isDiskDevice(file))
            info = DDiskInfo::getInfo(file);
        else
            info = DDiskInfo::getInfo(Helper::parentDevice(file));

        if (info.serial().isEmpty())
            return QString();

        int index = Helper::partitionIndex(file);

        if (index < 0)
            return "serial://" + info.serial();

        return QString("serial://%1:%2").arg(info.serial()).arg(index);
    }

    QFileInfo file_info(file);

    while (!file_info.exists() && file_info.absoluteFilePath() != "/") {
        file_info.setFile(file_info.absolutePath());
    }

    QStorageInfo info(file_info.absoluteFilePath());
    QString url = toSerialUrl(info.device());

    if (info.rootPath() == "/")
        return url + QFileInfo(file).absoluteFilePath();

    return url + QFileInfo(file).absoluteFilePath().mid(info.rootPath().length());
}

void MainWindow::startWithFile(const QString &source, const QString &target)
{
    m_sourceFile = source;
    m_targetFile = target;

    if (source.startsWith("serial://")) {
        m_sourceFile = parseSerialUrl(source, this);

        if (m_sourceFile.isEmpty())
            return;
    }

    if (target.startsWith("serial://")) {
        m_targetFile = parseSerialUrl(target, this);

        if (m_targetFile.isEmpty())
            return;
    }

    bool s_is_block = Helper::isBlockSpecialFile(m_sourceFile);
    bool s_is_disk = Helper::isDiskDevice(m_sourceFile);

    bool t_is_block = Helper::isBlockSpecialFile(m_targetFile);
    bool t_is_disk = Helper::isDiskDevice(m_targetFile);

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

    if (m_currentStatus == SelectAction)
        m_currentStatus = (Status)(End + 1);

    setStatus(WaitConfirm);
}

void MainWindow::init()
{
    m_title = new IconLabel(this);
    m_title->setObjectName("MainTitle");
    m_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_subTitle = new QLabel(this);
    m_subTitle->setObjectName("SubTitle");
    m_contentWidget = new QStackedWidget(this);
    m_cancelButton = new QPushButton(this);
    m_cancelButton->setFixedSize(width() * 0.36, height() * 0.055);
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->hide();
    m_bottomButton = new QPushButton(this);
    m_bottomButton->setFixedSize(width() * 0.36, height() * 0.055);
    m_loadingIndicator = new DVideoWidget(m_bottomButton);
    m_player = new QMediaPlayer(m_loadingIndicator);

    int scale_mon = 1;

    if (qEnvironmentVariableIsEmpty("QT_HIGHDPI_DISABLE_2X_IMAGE_LOADING"))
        scale_mon = qCeil(qApp->devicePixelRatio());

    m_player->setMedia(QMediaContent(QUrl(QString("qrc:/icons/loading@%1x.mov").arg(scale_mon == 1 ? 1 : 2))));
    m_loadingIndicator->setSource(m_player);
    m_loadingIndicator->resize(m_bottomButton->height() * 0.8, m_bottomButton->height() * 0.8);
    m_loadingIndicator->hide();
    Anchors<DVideoWidget>(m_loadingIndicator).setCenterIn(m_bottomButton);
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
    layout->addWidget(m_subTitle, 0, Qt::AlignHCenter);
    layout->addWidget(m_cancelButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_bottomButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_pageIndicator, 0, Qt::AlignHCenter);

    connect(m_cancelButton, &QPushButton::clicked, this, [this] {
        setStatus(SelectAction);
    });
    connect(m_bottomButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_player, &QMediaPlayer::stateChanged, this, [this] (QMediaPlayer::State newState) {
        if (newState == QMediaPlayer::StoppedState) {
            m_player->play();
        }
    });
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

        if (!QFile::exists(m_sourceFile)) {
            showErrorMessage(tr("Source file not found"));

            return;
        }

        if (m_targetFile.isEmpty()) {
            showErrorMessage(tr("Target file is empty"));

            return;
        }

        if (m_sourceFile == m_targetFile) {
            showErrorMessage(tr("The source file and the destination file can not be the same file, Please re-select"));

            return;
        }

        break;
    }
    default:
        break;
    }

    centralWidget()->setEnabled(false);
    m_loadingIndicator->show();
    m_player->play();

    m_title->setIcon(QIcon());
    m_subTitle->setText(QString());
    m_cancelButton->setVisible(status == WaitConfirm);

    switch (status) {
    case SelectAction: {
        m_currentMode = SelectActionPage::Backup;
        m_operateObject = SelectActionPage::Disk;

        setContent(new SelectActionPage());
        m_title->setTitle(tr("Plase Select Action"));
        m_bottomButton->setText(tr("Next"));
        m_buttonAction = Next;
        m_pageIndicator->setCurrentPage(0);
        break;
    } case SelectFile: {
        m_sourceFile.clear();
        m_targetFile.clear();

        SelectActionPage *page = qobject_cast<SelectActionPage*>(content());
        SelectFilePage *new_page = new SelectFilePage(m_operateObject, m_currentMode);
        QString sub_title;
        QString button_text;

        setContent(new_page);

        if (page) {
            m_title->setTitle(page->selectedItemTitle());
            m_title->setIcon(page->selectedItemIcon(), m_title->sizeHint().height() * 2);
        }

        if (m_currentMode == SelectActionPage::Backup) {
            button_text = tr("Begin Backup");
        } else if (m_currentMode == SelectActionPage::Clone) {
            button_text = tr("Begin Clone");

            if (m_operateObject == SelectActionPage::Disk)
                sub_title = tr("克隆磁盘会删除目标磁盘内的所有数据，请一定一定确认后再继续");
            else
                sub_title = tr("克隆分区会删除目标分区内的所有数据，请一定一定确认后再继续");
        } else {
            button_text = tr("Begin Restore");

            if (m_operateObject == SelectActionPage::Disk)
                sub_title = tr("恢复操作会删除目标磁盘内的所有数据，请一定一定确认后再继续");
            else
                sub_title = tr("恢复操作会删除目标分区内的所有数据，请一定一定确认后再继续");
        }

        auto on_file_changed = [new_page, sub_title, button_text, this] {
            const QString &source = new_page->source();
            const QString &target = new_page->target();

            QString busy_device;

            if (Helper::isMounted(source)) {
                if (Helper::umountDevice(source)) {
                    if (!Helper::isBlockSpecialFile(m_targetFile)) {
                        QFileInfo info(m_targetFile);

                        if (!info.absoluteDir().exists()) {
                            m_subTitle->setText(tr("The %1 directory not exists").arg(info.absolutePath()));
                            m_targetFile.clear();
                        }
                    }
                } else {
                    busy_device = source;
                }
            }

            if (busy_device.isEmpty() && Helper::isMounted(target)) {
                if (Helper::umountDevice(target)) {
                    if (!Helper::isBlockSpecialFile(m_sourceFile)) {
                        if (!QFile::exists(m_sourceFile)) {
                            m_subTitle->setText(tr("The %1 file not found").arg(m_sourceFile));
                            m_sourceFile.clear();
                        }
                    }
                } else {
                    busy_device = target;
                }
            }

            if (!busy_device.isEmpty()) {
                m_subTitle->setText(tr("设备\"%1\"被占用，重启到Live系统继续操作").arg(busy_device));
                m_bottomButton->setText(tr("重启并继续"));
                m_buttonAction = RestartToLiveSystem;
                m_bottomButton->setEnabled(true);
                m_sourceFile = new_page->source();
                m_targetFile = new_page->target();
                m_bottomButton->setEnabled(m_sourceFile != m_targetFile);
            } else {
                m_subTitle->setText(sub_title);
                m_bottomButton->setText(button_text);
                m_buttonAction = Next;
                m_bottomButton->setEnabled(!source.isEmpty() && !target.isEmpty());
            }
        };

        m_buttonAction = Next;
        m_pageIndicator->setCurrentPage(1);

        connect(new_page, &SelectFilePage::sourceChanged, new_page, on_file_changed);
        connect(new_page, &SelectFilePage::targetChanged, new_page, on_file_changed);
        on_file_changed();
        break;
    }
    case WaitConfirm: {
        if (!Helper::isBlockSpecialFile(m_targetFile))
            return setStatus(Working);

        EndPage *page = new EndPage(EndPage::Warning);

        page->setTitle(tr("您确定要继续吗？"));
        page->setMessage(tr("继续%1会格式化\"%2\"%3的所有数据，此过程不可逆也不可取消，为了您的数据安全，一定要仔细检查您的操作，确定没问题后再继续").arg(currentModeString()).arg(m_targetFile).arg(operateObjectString()));
        setContent(page);
        m_title->setTitle(tr("提醒"));
        m_bottomButton->setText(tr("Containue"));
        m_buttonAction = Next;
        m_pageIndicator->setCurrentPage(1);
        break;
    }
    case Working: {
        QString busy_device;

        if (Helper::isMounted(m_sourceFile)) {
            if (!Helper::umountDevice(m_sourceFile))
                busy_device = m_sourceFile;
        }

        if (busy_device.isEmpty() && Helper::isMounted(m_targetFile)) {
            if (!Helper::umountDevice(m_targetFile))
                busy_device = m_targetFile;
        }

        if (!busy_device.isEmpty()) {
            showErrorMessage(tr("无法继续操作，设备\"%1\"被占用").arg(busy_device));
            setStatus(SelectFile);

            return;
        }

        if (!Helper::isBlockSpecialFile(m_sourceFile)) {
            if (!QFile::exists(m_sourceFile)) {
                showErrorMessage(tr("The %1 file not found").arg(m_sourceFile));
                return setStatus(SelectFile);
            }
        }

        if (!Helper::isBlockSpecialFile(m_targetFile)) {
            QFileInfo info(m_targetFile);

            if (!info.absoluteDir().exists()) {
                showErrorMessage(tr("The %1 directory not exists").arg(info.absolutePath()));

                return setStatus(SelectFile);
            }
        }

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
        m_pageIndicator->setCurrentPage(2);
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

            page->setTitle("抱歉，任务失败");
            page->setMessage(worker->errorString());
            m_bottomButton->setText(tr("Retry"));
            m_buttonAction = Retry;
        } else {
            page->setTitle(tr("恭喜您，任务完成"));

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

        setContent(page);
        m_pageIndicator->setCurrentPage(2);
        break;
    }
    default:
        break;
    }

    m_currentStatus = status;

    m_loadingIndicator->hide();
    m_player->pause();
    centralWidget()->setEnabled(true);
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
    case RestartToLiveSystem: {
        const QString &source_url = toSerialUrl(m_sourceFile);
        const QString &target_url = toSerialUrl(m_targetFile);

        qDebug() << source_url << target_url;

        break;
    }
    default:
        break;
    }
}

void MainWindow::showErrorMessage(const QString &message)
{
    DDialog dialog(message, QString(), this);

    dialog.setMaximumWidth(width() / 2);
    dialog.setIcon(QIcon::fromTheme("dialog-error"));
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

QString MainWindow::currentModeString() const
{
    switch (m_currentMode) {
    case SelectActionPage::Backup:
        return tr("Backup");
    case SelectActionPage::Clone:
        return tr("Clone");
    case SelectActionPage::Restore:
        return tr("Restore");
    default:
        break;
    }

    return QString();
}

QString MainWindow::operateObjectString() const
{
    switch (m_operateObject) {
    case SelectActionPage::Disk:
        return tr("Disk");
    case SelectActionPage::Partition:
        return tr("Partition");
    default:
        break;
    }

    return QString();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    const QMargins &content_margins = layout()->contentsMargins();

    m_contentWidget->setFixedWidth(e->size().width() - content_margins.left() - content_margins.right());
}

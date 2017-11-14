/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "clonejob.h"
#include "iconlabel.h"
#include "selectfilepage.h"
#include "helper.h"
#include "workingpage.h"
#include "endpage.h"
#include "dglobal.h"

#include "ddiskinfo.h"
#include "dpartinfo.h"
#include "ddevicepartinfo.h"

#include <DDesktopServices>
#include <ddialog.h>
#include <anchors.h>
#include <DWindowManagerHelper>
#include <DTitlebar>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QDebug>
//#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    init();
    setStatus(SelectAction);
}

MainWindow::~MainWindow()
{

}

QString parseSerialUrl(const QString &urlString, MainWindow *window = 0)
{
    if (urlString.isEmpty())
        return QString();

    const QUrl url(urlString);
    const QString serial_number = urlString.split("//").at(1).split(":").first();
    const int part_index = url.port();
    const QString &path = url.path();
    const QString &device = Helper::findDiskBySerialIndexNumber(serial_number, part_index);
    const QString &device_url = part_index > 0 ? QString("serial://%1:%2").arg(serial_number).arg(part_index) : "serial://" + serial_number;

    if (device.isEmpty()) {
        if (window) {
            if (part_index > 0)
                window->showErrorMessage(QObject::tr("Partition \"%1\" not found").arg(device_url));
            else
                window->showErrorMessage(QObject::tr("Disk \"%1\" not found").arg(device_url));
        }

        return device;
    }

    if (path.isEmpty())
        return device;

    const QString &mp = Helper::mountPoint(device);

    QDir mount_point(mp);

    if (mp.isEmpty()) {
        QString mount_name;

        if (part_index >= 0)
            mount_name = QString("%1-%2").arg(serial_number).arg(part_index);
        else
            mount_name = serial_number;

        const QString &_mount_point = Helper::temporaryMountDevice(device, mount_name);

        if (_mount_point.isEmpty()) {
            if (window)
                window->showErrorMessage(QObject::tr("Failed to mount partition \"%1\"").arg(device_url));

            return QString();
        }

        mount_point.setPath(_mount_point);
    }

    if (mount_point.absolutePath() == "/")
        return path;

    return mount_point.absolutePath() + path;
}

static QString getDeviceForFile(const QString &file, QString *rootPath = 0)
{
    if (file.isEmpty())
        return QString();

    if (Helper::isBlockSpecialFile(file))
        return file;

    QFileInfo info(file);

    while (!info.exists() && info.absoluteFilePath() != "/")
        info.setFile(info.absolutePath());

    QStorageInfo storage_info(info.absoluteFilePath());

    if (rootPath)
        *rootPath = storage_info.rootPath();

    return QString::fromUtf8(storage_info.device());
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

        if (!info)
            return QString();

        if (info.serial().isEmpty())
            return QString();

        int index = DDevicePartInfo(file).indexNumber();

        if (index == 0)
            return "serial://" + info.serial();

        return QString("serial://%1:%2").arg(info.serial()).arg(index);
    }

    QString root_path;
    QString url = toSerialUrl(getDeviceForFile(file, &root_path));

    if (root_path == "/")
        return url + QFileInfo(file).absoluteFilePath();

    return url + QFileInfo(file).absoluteFilePath().mid(root_path.length());
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

    if (s_is_block && m_targetFile.isEmpty()) {
        return;
    }

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

        if (m_targetFile.isEmpty()) {
            DDiskInfo info = DDiskInfo::getInfo(m_sourceFile);

            if (!info)
                return;

            if (info.type() == DDiskInfo::Disk)
                m_operateObject = SelectActionPage::Disk;
            else
                m_operateObject = SelectActionPage::Partition;

            SelectActionPage *page = qobject_cast<SelectActionPage*>(content());

            if (page) {
                page->setMode(m_operateObject);
                page->setAction(m_currentMode);
            }

            return setStatus(SelectFile);
        }
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
    m_fixBootCheckBox = new QCheckBox(this);
    m_fixBootCheckBox->setText(tr("Restore boot"));
    m_fixBootCheckBox->hide();
    m_cancelButton = new QPushButton(this);
    m_cancelButton->setFixedSize(310, 36);
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->hide();
    m_bottomButton = new QPushButton(this);
    m_bottomButton->setFixedSize(310, 36);
    m_bottomButton->setObjectName("NextButton");
//    m_loadingIndicator = new DVideoWidget(m_bottomButton);
//    m_loadingIndicator->resize(m_bottomButton->height() * 0.8, m_bottomButton->height() * 0.8);
//    m_loadingIndicator->hide();
//    Anchors<DVideoWidget>(m_loadingIndicator).setCenterIn(m_bottomButton);
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
    layout->addWidget(m_fixBootCheckBox, 0, Qt::AlignHCenter);
    layout->addWidget(m_subTitle, 0, Qt::AlignHCenter);
    layout->addWidget(m_cancelButton, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(m_bottomButton, 0, Qt::AlignHCenter);
    layout->addSpacing(5);
    layout->addWidget(m_pageIndicator, 0, Qt::AlignHCenter);

    connect(m_cancelButton, &QPushButton::clicked, this, [this] {
        setStatus(SelectAction);
    });
    connect(m_bottomButton, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(m_fixBootCheckBox, &QCheckBox::toggled, this, [this] {
        Global::fixBoot = m_fixBootCheckBox->isChecked();
    });
}

void MainWindow::setStatus(MainWindow::Status status)
{
    dCInfo("current state: \"%s\", new state: \"%s\"",
           QMetaEnum::fromType<Status>().valueToKey(m_currentStatus),
           QMetaEnum::fromType<Status>().valueToKey(status));
    dCInfo("action: \"%s\", mode: \"%s\"\nsource: \"%s\"\ntarget: \"%s\"",
           QMetaEnum::fromType<SelectActionPage::Action>().valueToKey(m_currentMode),
           QMetaEnum::fromType<SelectActionPage::Mode>().valueToKey(m_operateObject),
           qPrintable(m_sourceFile), qPrintable(m_targetFile));

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
            showErrorMessage("Source file not found");

            return;
        }

        if (m_targetFile.isEmpty()) {
            showErrorMessage("Target file is empty");

            return;
        }

        if (m_sourceFile == m_targetFile) {
            showErrorMessage("The source file and the destination file can not be the same file, Please re-select");

            return;
        }

        break;
    }
    default:
        break;
    }

    centralWidget()->setEnabled(false);
    playLoadingIndicator();

    m_title->setIcon(QIcon());
    m_subTitle->setText(QString());
    m_cancelButton->setVisible(status == WaitConfirm || status == ToLiveSystem);
    m_bottomButton->setVisible(true);
    m_fixBootCheckBox->hide();

    switch (status) {
    case SelectAction: {
        m_currentMode = SelectActionPage::Backup;
        m_operateObject = SelectActionPage::Disk;

        setContent(new SelectActionPage());
        m_title->setTitle(tr("Select Operation"));
        m_bottomButton->setText(tr("Next"));
        m_buttonAction = Next;
        m_pageIndicator->setCurrentPage(0);
        break;
    } case SelectFile: {
        SelectActionPage *page = qobject_cast<SelectActionPage*>(content());
        SelectFilePage *new_page = new SelectFilePage(m_operateObject, m_currentMode);
        QString sub_title;
        QString button_text;

        if (!m_sourceFile.isEmpty())
            new_page->setSource(m_sourceFile);

        if (!m_targetFile.isEmpty())
            new_page->setTarget(m_targetFile);

        setContent(new_page);

        if (page) {
            m_title->setTitle(page->selectedItemTitle());
            m_title->setIcon(page->selectedItemIcon(), m_title->sizeHint().height() * 2);
        }

        if (m_currentMode == SelectActionPage::Backup) {
            button_text = tr("Backup");
        } else if (m_currentMode == SelectActionPage::Clone) {
            button_text = tr("Clone");

            if (m_operateObject == SelectActionPage::Disk)
                sub_title = tr("Target disk will be permanently overwritten, please confirm to continue");
            else
                sub_title = tr("Target partition will be permanently overwritten, please confirm to continue");
        } else {
            button_text = tr("Restore");

            if (m_operateObject == SelectActionPage::Disk)
                sub_title = tr("Target disk will be permanently overwritten, please confirm to continue");
            else
                sub_title = tr("Target partition will be permanently overwritten, please confirm to continue");
        }

        auto on_file_changed = [new_page, sub_title, button_text, this] {
            const QString &source = new_page->source();
            const QString &target = new_page->target();

            if (!Helper::isBlockSpecialFile(source)) {
                const QString &source_device = getDeviceForFile(source);

                if (source_device == target || Helper::parentDevice(source_device) == target) {
                    if (m_operateObject == SelectActionPage::Disk)
                        m_subTitle->setText(tr("Please move image file to other location outside the disk to avoid data loss"));
                    else
                        m_subTitle->setText(tr("Please move image file to other location outside the partition to avoid data loss"));

                    m_bottomButton->setEnabled(false);

                    return;
                }
            }

            if (!Helper::isBlockSpecialFile(target)) {
                const QString target_device = getDeviceForFile(target);

                if (target_device == source || Helper::parentDevice(target_device) == source) {
                    if (m_operateObject == SelectActionPage::Disk)
                        m_subTitle->setText(tr("Storage location can not be in the disk to backup, please reselect"));
                    else
                        m_subTitle->setText(tr("Storage location can not be in the partition to backup, please reselect"));

                    m_bottomButton->setEnabled(false);

                    return;
                }
            }

            const DDiskInfo &disk_info = DDiskInfo::getInfo(source);

            if (disk_info) {
                if (Helper::isBlockSpecialFile(target)) {
                    const DDiskInfo &target_disk_info = DDiskInfo::getInfo(target);

                    if (target_disk_info) {
                        if (target_disk_info.totalSize() < disk_info.totalSize()) {
                            dCError("Not enough total capacity in target device, source size: %lld, target size: %lld", disk_info.totalSize(), target_disk_info.totalSize());

                            if (m_operateObject == SelectActionPage::Disk)
                                m_subTitle->setText(tr("Not enough total capacity in target disk, please select another one"));
                            else
                                m_subTitle->setText(tr("Not enough total capacity in target partition, please select another one"));

                            m_bottomButton->setEnabled(false);

                            return;
                        }

                        if (target_disk_info.type() == DDiskInfo::Part
                                && target_disk_info.ptType() != DDiskInfo::Unknow
                                && disk_info.childrenPartList().first().isDeepinSystemRoot()
                                && disk_info.type() == DDiskInfo::Part) {
                            m_fixBootCheckBox->show();
                            m_fixBootCheckBox->setChecked(Global::fixBoot);
                        } else {
                            m_fixBootCheckBox->hide();
                        }
                    }
                } else {
                    if (!target.isEmpty()) {
                        QFileInfo target_info(target);
                        QStorageInfo storage_info(target_info.absoluteDir());

                        if (storage_info.bytesAvailable() < disk_info.totalReadableDataSize()) {
                            dCError("Not enough total capacity, source size: %lld, target available size: %lld", disk_info.totalSize(), storage_info.bytesAvailable());

                            m_subTitle->setText(tr("Not enough total capacity, please select another disk"));
                            m_bottomButton->setEnabled(false);

                            return;
                        }
                    }
                }
            }

            m_subTitle->setText(sub_title);
            m_bottomButton->setText(button_text);
            m_buttonAction = Next;
            m_bottomButton->setEnabled(!source.isEmpty() && !target.isEmpty());
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
            return setStatus(ToLiveSystem);

        EndPage *page = new EndPage(EndPage::Warning);

        page->setTitle(tr("Proceed to clone?"));
        page->setMessage(tr("All data in target loacation will be formated during cloning or restoring disk (partition) without cancelable operation."));
        setContent(page);
        m_title->setTitle(tr("Warning"));
        m_bottomButton->setText(tr("OK"));
        m_buttonAction = Next;
        m_pageIndicator->setCurrentPage(1);
        break;
    }
    case ToLiveSystem: {
        QString busy_device;

        if (Helper::isMounted(m_sourceFile)) {
            if (Helper::umountDevice(m_sourceFile)) {
                if (!Helper::isBlockSpecialFile(m_targetFile)) {
                    QFileInfo info(m_targetFile);

                    if (!info.absoluteDir().exists()) {
                        dCError("The \"%s\" directory not exists", qPrintable(info.absolutePath()));

                        showErrorMessage(tr("The selected storage location not found"));

                        return;
                    }
                }
            } else {
                busy_device = m_sourceFile;
            }
        }

        if (busy_device.isEmpty() && Helper::isMounted(m_targetFile)) {
            if (Helper::umountDevice(m_targetFile)) {
                if (!Helper::isBlockSpecialFile(m_sourceFile)) {
                    if (!QFile::exists(m_sourceFile)) {
                        showErrorMessage(tr("%1 not exist").arg(m_sourceFile));

                        return;
                    }
                }
            } else {
                busy_device = m_targetFile;
            }
        }

        if (busy_device.isEmpty())
            return setStatus(Working);

        EndPage *page;

        if (Helper::existLiveSystem()) {
            m_bottomButton->setText(tr("Restart to Continue"));
            m_buttonAction = RestartToLiveSystem;
            page = new EndPage(EndPage::Warning);
            page->setMessage(tr("\"%1\" is used, please restart and enter \"Deepin Recovery\" to continue").arg(busy_device));
        } else {
            m_bottomButton->setText(tr("OK"));
            m_buttonAction = Cancel;
            page = new EndPage(EndPage::Fail);
            page->setMessage(tr("\"%1\" is used, please install \"Deepin Recovery\" to retry").arg(busy_device));
        }

        setContent(page);
        m_title->setTitle(QString());
        m_bottomButton->setEnabled(true);
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
            showErrorMessage(QString("Can not coutinue，The \"%1\" device is busy").arg(busy_device));
            setStatus(SelectFile);

            return;
        }

        if (!Helper::isBlockSpecialFile(m_sourceFile)) {
            if (!QFile::exists(m_sourceFile)) {
                showErrorMessage(tr("%1 not exist").arg(m_sourceFile));

                return setStatus(SelectFile);
            }
        }

        if (!Helper::isBlockSpecialFile(m_targetFile)) {
            QFileInfo info(m_targetFile);

            if (!info.absoluteDir().exists()) {
                dCError("The \"%s\" directory not exists", qPrintable(info.absolutePath()));

                showErrorMessage(tr("The selected storage location not found"));

                return setStatus(SelectFile);
            }
        }

        // disable the window close function
        DWindowManagerHelper::setMotifFunctions(windowHandle(), DWindowManagerHelper::FUNC_CLOSE, false);
        titlebar()->setDisableFlags(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);

        WorkingPage *page = new  WorkingPage(m_sourceFile, m_targetFile);

        connect(page, &WorkingPage::finished, this, &MainWindow::next);
        setContent(page);

        if (m_currentMode == SelectActionPage::Backup)
            m_title->setTitle(tr("Performing Backup"));
        else if (m_currentMode == SelectActionPage::Clone)
            m_title->setTitle(tr("Cloning"));
        else
            m_title->setTitle(tr("Restoring"));

        m_bottomButton->setText(tr("Cancel"));
        m_buttonAction = Cancel;
        m_bottomButton->setVisible(m_currentMode == SelectActionPage::Backup);
        m_pageIndicator->setCurrentPage(2);
        break;
    }
    case End: {
        // enable the window close function
        DWindowManagerHelper::setMotifFunctions(windowHandle(), DWindowManagerHelper::FUNC_CLOSE, true);
        titlebar()->setDisableFlags(Qt::WindowMaximizeButtonHint);

        bool is_error = isError();

        WorkingPage *worker = qobject_cast<WorkingPage*>(content());
        EndPage *page = new EndPage(is_error ? EndPage::Fail : EndPage::Success);

        if (is_error) {
            if (m_currentMode == SelectActionPage::Backup) {
                m_title->setTitle(tr("Backup Failed"));

                if (QFile::remove(m_targetFile)) {
                    dCDebug("Remove dim file(%s)", qPrintable(m_targetFile));
                }
            } else if (m_currentMode == SelectActionPage::Clone) {
                m_title->setTitle(tr("Clone Failed"));
            } else {
                m_title->setTitle(tr("Restore Failed"));
            }

            page->setTitle("Sorry, task failed");
            page->setMessage(worker->errorString());
            m_bottomButton->setText(tr("Retry"));
            m_buttonAction = Retry;

            dCWarning("!!!!job failed!!!!\n");

            if (Helper::processExec("lsblk -O -J") == 0) {
                dCInfo("All disk/partition device info\n");
                dCDebug("\n\"%s\"\n", qPrintable(Helper::lastProcessStandardOutput()));
            }
        } else {
            page->setTitle(tr("Task done"));

            if (m_currentMode == SelectActionPage::Backup) {
                m_title->setTitle(tr("Backup Succeeded"));
                m_bottomButton->setText(tr("View Backup File"));
                m_buttonAction = ShowBackupFile;

                if (!QFile::setPermissions(m_targetFile, QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup
                                           | QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup)) {
                    dCDebug("Set the file permissions failed");
                }
            } else if (m_currentMode == SelectActionPage::Clone) {
                m_title->setTitle(tr("Clone Succeeded"));
                m_bottomButton->setText(tr("OK"));
                m_buttonAction = Quit;
            } else {
                m_title->setTitle(tr("Restore Succeeded"));
                m_bottomButton->setText(tr("Restart"));
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

    stopLoadingIndicator();
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
        dCDebug("Show \"%s\" on file manager", qPrintable(m_targetFile));

        DDesktopServices::showFileItem(m_targetFile);
        return;
    }
    case Quit: {
        return qApp->quit();
    }
    case RestartSystem: {
        dCDebug("Try restart system");

        if (Helper::processExec("reboot") != 0) {
            dCError("Failed to restart system");

            showErrorMessage(tr("Failed to restart system"));
        }

        return qApp->quit();
    }
    case RestartToLiveSystem: {
        const QString &source_url = toSerialUrl(m_sourceFile);
        const QString &target_url = toSerialUrl(m_targetFile);

        dCDebug("Try restart to live system, source url: %s, target url: %s", qPrintable(source_url), qPrintable(target_url));

        QStringList arguments;

        arguments << source_url << target_url
                  << "-B" << QString::number(Global::bufferSize)
                  << "-C" << QString::number(Global::compressionLevel)
                  << "-d" << QString::number(Global::debugLevel)
                  << "--log-backup" << toSerialUrl("/var/log/deepin-clone-livesystem.log");

        if (Global::disableMD5CheckForDimFile) {
            arguments << "--no-check-dim";
        }

        if (!Global::disableLoopDevice) {
            arguments << "--loop-device";
        }

        if (Global::fixBoot) {
            arguments << "--auto-fix-boot";
        }

        if (!Helper::restartToLiveSystem(arguments)) {
            dCError("Restart to live system failed!");

            showErrorMessage(tr("Failed to restart \"Deepin Recovery\""));
        }

        break;
    }
    default:
        break;
    }
}

void MainWindow::showErrorMessage(const QString &message)
{
    dCError(qPrintable(message));

    DDialog dialog(message, QString(), this);

    dialog.setMaximumWidth(width() / 2);
    dialog.setIcon(QIcon::fromTheme("dialog-error"));
    dialog.addButton(tr("OK"), true);
    dialog.exec();
}

void MainWindow::playLoadingIndicator()
{
    m_buttonTextBackup = m_bottomButton->text();
    m_bottomButton->setText(tr("Loading"));
//    int scale_mon = 1;

//    if (qEnvironmentVariableIsEmpty("QT_HIGHDPI_DISABLE_2X_IMAGE_LOADING"))
//        scale_mon = qCeil(qApp->devicePixelRatio());

//    m_player = new QMediaPlayer(m_loadingIndicator);

//    connect(m_player, &QMediaPlayer::stateChanged, m_player, &QMediaPlayer::play);

//    m_loadingIndicator->setSource(m_player);
//    m_player->setMedia(QMediaContent(QUrl(QString("qrc:/icons/loading@%1x.mov").arg(scale_mon == 1 ? 1 : 2))));
//    m_loadingIndicator->show();
//    m_player->play();
}

void MainWindow::stopLoadingIndicator()
{
    if (m_bottomButton->text() == tr("Loading"))
        m_bottomButton->setText(m_buttonTextBackup);

    m_buttonTextBackup.clear();
//    m_loadingIndicator->hide();
//    disconnect(m_player, &QMediaPlayer::stateChanged, m_player, &QMediaPlayer::play);
//    m_player->pause();
//    m_player->deleteLater();
//    m_player = 0;
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

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_F1 && !e->isAutoRepeat()) {
        QProcess::startDetached("dman", QStringList() << qApp->applicationName());
    }

    return DMainWindow::keyPressEvent(e);
}

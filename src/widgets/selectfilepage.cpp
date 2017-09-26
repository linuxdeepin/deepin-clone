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

#include "selectfilepage.h"
#include "disklistitem.h"
#include "partitionlistitem.h"
#include "ddevicediskinfo.h"
#include "ddevicepartinfo.h"
#include "utilitylist.h"
#include "iconlabel.h"

#include <dlinkbutton.h>

#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMimeDatabase>
#include <QDebug>

DWIDGET_USE_NAMESPACE

class SelectFileWidget : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        GetSaveName,
        GetFile
    };

    explicit SelectFileWidget(Mode mode, QWidget *parent = 0);

    QString selectFilePath() const;

signals:
    void filePathChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    void setFilePath(const QString &path);

    Mode m_mode;
    QString m_filePath;
    QString m_defaultFileName;
    QLabel *m_dragDropLabel = 0;
    IconLabel *m_label;
    DLinkButton *m_button;

    friend class SelectFilePage;
};

SelectFileWidget::SelectFileWidget(Mode mode, QWidget *parent)
    : QWidget(parent)
    , m_mode(mode)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_label = new IconLabel(this);

    m_label->setDirection(QBoxLayout::TopToBottom);

    if (mode == GetSaveName) {
        m_label->setIcon(QIcon::fromTheme("inode-directory"), 80);
    } else {
        m_label->setIcon(QIcon::fromTheme("application-x-deepinclone-dim"), 80);
        setAcceptDrops(true);
    }

    m_button = new DLinkButton(mode == GetSaveName ? tr("Select storage location") : tr("Select image file"), this);

    layout->addStretch();
    layout->addWidget(m_label, 0, Qt::AlignHCenter);

    if (mode == GetFile) {
        m_dragDropLabel = new QLabel(tr("Drag and drop backup image file here"), this);
        m_dragDropLabel->setObjectName("DragDropLabel");

        QWidget *split_line = new QWidget(this);

        split_line->setFixedSize(220, 2);
        split_line->setObjectName("SplitLine");

        layout->addWidget(m_dragDropLabel, 0, Qt::AlignHCenter);
        layout->addSpacing(10);
        layout->addWidget(split_line, 0, Qt::AlignHCenter);
        layout->addSpacing(10);
    }

    layout->addWidget(m_button, 0, Qt::AlignHCenter);
    layout->addStretch();

    connect(m_button, &DLinkButton::clicked, this, [this] {
        QFileDialog dialog(this);

        dialog.setMimeTypeFilters(QStringList() << "application-x-deepinclone-dim");
        dialog.setNameFilters(QStringList() << tr("Deepin Image File") + "(*.dim)");
        dialog.setDefaultSuffix("dim");
        dialog.setWindowTitle(m_button->text());

        if (m_mode == GetSaveName) {
            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            dialog.selectFile(m_defaultFileName);
        } else {
            dialog.setFileMode(QFileDialog::ExistingFile);
            dialog.setAcceptMode(QFileDialog::AcceptOpen);
        }

        if (dialog.exec() == QFileDialog::Accepted) {
            setFilePath(dialog.selectedFiles().first());
        }
    });

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QString SelectFileWidget::selectFilePath() const
{
    return m_filePath;
}

void SelectFileWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();

        if (urls.count() != 1)
            return QWidget::dragEnterEvent(event);

        const QUrl &first = urls.first();

        if (!first.isLocalFile())
            return QWidget::dragEnterEvent(event);

        QMimeDatabase db;
        const QMimeType &type = db.mimeTypeForUrl(first);

        if (type.name() != "application/x-deepinclone-dim")
            return QWidget::dragEnterEvent(event);

        return event->accept();
    }

    return QWidget::dragEnterEvent(event);
}

void SelectFileWidget::dropEvent(QDropEvent *event)
{
    setFilePath(event->mimeData()->urls().first().toLocalFile());

    return event->accept();
}

void SelectFileWidget::setFilePath(const QString &path)
{
    if (m_filePath == path)
        return;

    m_filePath = path;

    if (m_dragDropLabel) {
        m_button->setText(tr("Reselect image file"));
        m_dragDropLabel->hide();
    } else {
        m_button->setText(tr("Reselect storage location"));
    }

    m_label->setTitle(m_filePath);

    emit filePathChanged();
}

static QString getFilePath(const QWidget *widget)
{
    if (const UtilityList *list = qobject_cast<const UtilityList*>(widget)) {
        QListWidgetItem *item = list->selectedItems().first();

        if (item->isHidden())
            return QString();

        const QWidget *widget = list->itemWidget(item);

        if (const DiskListItem *item = qobject_cast<const DiskListItem*>(widget)) {
            return item->info().filePath();
        } else if (const PartitionListItem *item = qobject_cast<const PartitionListItem*>(widget)) {
            return item->info().filePath();
        }
    } else if (const SelectFileWidget *file = qobject_cast<const SelectFileWidget*>(widget)) {
        return file->selectFilePath();
    }

    return QString();
}

SelectFilePage::SelectFilePage(SelectActionPage::Mode mode, SelectActionPage::Action action, QWidget *parent)
    : ContentPairPage(parent)
{
    QWidget *left_widget = 0;
    QWidget *right_widget = 0;

    switch (action) {
    case SelectActionPage::Clone: {
        UtilityList *left_list = new UtilityList(this);
        UtilityList *right_list = new UtilityList(this);

        left_widget = left_list;
        right_widget = right_list;

        if (mode == SelectActionPage::Disk) {
            setLeftTitle(tr("Select the source disk"));
            setRightTitle(tr("Select the target disk"));

            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, left_list);

                left_list->addItem(item);

                if (!info.isReadonly()) {
                    item = new DiskListItem(info, right_list);

                    right_list->addItem(item);
                }
            }
        } else {
            setLeftTitle(tr("Select the source partition"));
            setRightTitle(tr("Select the target partition"));

            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, left_list);

                left_list->addItem(item);

                if (!info.isReadonly()) {
                    item = new PartitionListItem(info, right_list);

                    right_list->addItem(item);
                }
            }
        }

        hideItemForFile(getFilePath(left_widget), right_list);

        connect(left_list, &UtilityList::itemSelectionChanged, this, &SelectFilePage::onSourceChanged);
        connect(left_list, &UtilityList::itemSelectionChanged, this, &SelectFilePage::sourceChanged, Qt::QueuedConnection);
        connect(right_list, &UtilityList::itemSelectionChanged, this, &SelectFilePage::targetChanged, Qt::QueuedConnection);
        break;
    }
    case SelectActionPage::Backup: {
        UtilityList *left_list = new UtilityList(this);

        left_widget = left_list;

        if (mode == SelectActionPage::Disk) {
            setLeftTitle(tr("Select a disk to backup"));

            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, left_list);

                left_list->addItem(item);
            }
        } else {
            setLeftTitle(tr("Select a partition to backup"));

            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, left_list);

                left_list->addItem(item);
            }
        }

        setRightTitle(tr("Select storage location"));

        SelectFileWidget *sfw = new SelectFileWidget(SelectFileWidget::GetSaveName, this);

        connect(left_list, &UtilityList::itemSelectionChanged, this, [sfw, left_list] {
            QWidget *widget = left_list->itemWidget(left_list->selectedItems().first());
            QString device_name;

            if (DiskListItem *item = qobject_cast<DiskListItem*>(widget)) {
                device_name = item->info().name();
            } else if (PartitionListItem *item = qobject_cast<PartitionListItem*>(widget)) {
                device_name = item->info().name();
            }

            if (!device_name.isEmpty())
                sfw->m_defaultFileName = QString("%1.dim").arg(device_name);
        });

        emit left_list->itemSelectionChanged();
        right_widget = sfw;

        connect(left_list, &UtilityList::itemSelectionChanged, this, &SelectFilePage::sourceChanged);
        connect(sfw, &SelectFileWidget::filePathChanged, this, &SelectFilePage::targetChanged);
        break;
    }
    case SelectActionPage::Restore: {
        setLeftTitle(tr("Select a backup image file"));

        if (mode == SelectActionPage::Disk)
            setRightTitle(tr("Select a disk to restore"));
        else
            setRightTitle(tr("Select a partition to restore"));

        SelectFileWidget *sfw = new SelectFileWidget(SelectFileWidget::GetFile, this);
        UtilityList *right_list = new UtilityList(this);

        left_widget = sfw;
        right_widget = right_list;

        if (mode == SelectActionPage::Disk) {
            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, right_list);

                right_list->addItem(item);
            }
        } else {
            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, right_list);

                right_list->addItem(item);
            }
        }

        connect(sfw, &SelectFileWidget::filePathChanged, this, &SelectFilePage::sourceChanged);
        connect(right_list, &UtilityList::itemSelectionChanged, this, &SelectFilePage::targetChanged);
        break;
    }
    default:
        break;
    }

    setLeftContent(left_widget, true);
    setRightContent(right_widget, true);
}

QString SelectFilePage::source() const
{
    return getFilePath(leftContent());
}

QString SelectFilePage::target() const
{
    return getFilePath(rightContent());
}

void SelectFilePage::onSourceChanged()
{
    const QString &left_file = source();

    if (UtilityList *list = qobject_cast<UtilityList*>(rightContent())) {
        hideItemForFile(left_file, list);
    }
}

void SelectFilePage::hideItemForFile(const QString &filePath, UtilityList *list)
{
    bool hide_tareget_is_selected = false;

    for (int i = 0; i < list->count(); ++i) {
        QListWidgetItem *item = list->item(i);
        QString file_path;

        if (const DiskListItem *widget = qobject_cast<const DiskListItem*>(list->itemWidget(item)))
            file_path = widget->info().filePath();
        else if (const PartitionListItem *widget = qobject_cast<const PartitionListItem*>(list->itemWidget(item)))
            file_path = widget->info().filePath();

        if (file_path == filePath) {
            item->setHidden(true);
            hide_tareget_is_selected = item->isSelected();
        } else {
            item->setHidden(false);
        }
    }

    if (!hide_tareget_is_selected)
        return;

    for (int i = 0; i < list->count(); ++i) {
        QListWidgetItem *item = list->item(i);

        if (!item->isHidden())
            item->setSelected(true);
    }
}

#include "selectfilepage.moc"
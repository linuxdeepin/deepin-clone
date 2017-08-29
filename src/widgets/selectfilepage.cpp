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

DWIDGET_USE_NAMESPACE

class SelectFileWidget : public QWidget
{
public:
    enum Mode {
        GetSaveName,
        GetFile
    };

    explicit SelectFileWidget(Mode mode, QWidget *parent = 0);

    QString selectFilePath() const;

private:
    Mode m_mode;
    QString m_filePath;
};

SelectFileWidget::SelectFileWidget(Mode mode, QWidget *parent)
    : QWidget(parent)
    , m_mode(mode)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    IconLabel *label = new IconLabel(this);

    label->setDirection(QBoxLayout::TopToBottom);

    if (mode == GetSaveName) {
        label->setIcon(QIcon::fromTheme("inode-directory"), 80);
    } else {
        label->setIcon(QIcon::fromTheme("application-x-deepinclone-dim"), 80);
    }

    DLinkButton *button = new DLinkButton(mode == GetSaveName ? tr("选择存储位置") : tr("选择文件"), this);

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addWidget(button, 0, Qt::AlignCenter);
    layout->addStretch();

    connect(button, &DLinkButton::clicked, this, [this, button, label] {
        QFileDialog dialog(this);

        dialog.setMimeTypeFilters(QStringList() << "application-x-deepinclone-dim");
        dialog.setNameFilters(QStringList() << "*.dim");
        dialog.setDefaultSuffix("dim");

        if (m_mode == GetSaveName) {
            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setAcceptMode(QFileDialog::AcceptSave);

            if (dialog.exec() == QFileDialog::Accepted) {
                m_filePath = dialog.selectedFiles().first();
                button->setText(tr("重新选择存储位置"));
            }
        } else {
            dialog.setFileMode(QFileDialog::ExistingFile);
            dialog.setAcceptMode(QFileDialog::AcceptOpen);

            if (dialog.exec() == QFileDialog::Accepted) {
                m_filePath = dialog.selectedFiles().first();
                button->setText(tr("重新选择文件"));
            }
        }

        label->setTitle(m_filePath);
    });

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QString SelectFileWidget::selectFilePath() const
{
    return m_filePath;
}

SelectFilePage::SelectFilePage(SelectActionPage::Mode mode, SelectActionPage::Action action, QWidget *parent)
    : ContentPairPage(parent)
{
    QWidget *left_widget;
    QWidget *right_widget;

    switch (action) {
    case SelectActionPage::Clone: {
        UtilityList *left_list = new UtilityList(this);
        UtilityList *right_list = new UtilityList(this);

        left_widget = left_list;
        right_widget = right_list;

        if (mode == SelectActionPage::Disk) {
            setLeftTitle(tr("请选择您要克隆的原始磁盘"));
            setRightTitle(tr("请选择您想克隆的目标磁盘"));

            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, left_list);

                left_list->addItem(item);

                item = new DiskListItem(info, right_list);

                right_list->addItem(item);
            }
        } else {
            setLeftTitle(tr("请选择您要克隆的原始分区"));
            setRightTitle(tr("请选择您想克隆的目标分区"));

            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, left_list);

                left_list->addItem(item);

                item = new PartitionListItem(info, right_list);

                right_list->addItem(item);
            }
        }

        break;
    }
    case SelectActionPage::Backup: {
        UtilityList *left_list = new UtilityList(this);

        left_widget = left_list;

        if (mode == SelectActionPage::Disk) {
            setLeftTitle(tr("请确认您要备份的磁盘"));

            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, left_list);

                left_list->addItem(item);
            }
        } else {
            setLeftTitle(tr("请确认您要备份的分区"));

            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, left_list);

                left_list->addItem(item);
            }
        }

        setRightTitle(tr("选择存储位置"));

        right_widget = new SelectFileWidget(SelectFileWidget::GetSaveName, this);

        break;
    }
    case SelectActionPage::Restore: {
        setLeftTitle(tr("请选择备份的镜像文件"));
        setRightTitle(tr("选择恢复位置"));

        left_widget = new SelectFileWidget(SelectFileWidget::GetFile, this);

        UtilityList *right_list = new UtilityList(this);

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

        break;
    }
    default:
        break;
    }

    setLeftContent(left_widget, true);
    setRightContent(right_widget, true);
}

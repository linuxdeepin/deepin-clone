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

    m_button = new DLinkButton(mode == GetSaveName ? tr("选择存储位置") : tr("选择文件"), this);

    layout->addStretch();
    layout->addWidget(m_label, 0, Qt::AlignHCenter);

    if (mode == GetFile) {
        m_dragDropLabel = new QLabel(tr("拖拽备份的镜像文件到此"), this);
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
    m_filePath = path;

    if (m_dragDropLabel) {
        m_button->setText(tr("重新选择文件"));
        m_dragDropLabel->hide();
    } else {
        m_button->setText(tr("重新选择存储位置"));
    }

    m_label->setTitle(m_filePath);
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
            setLeftTitle(tr("请选择您要克隆的原始磁盘"));
            setRightTitle(tr("请选择您想克隆的目标磁盘"));

            for (const DDiskInfo &info : DDeviceDiskInfo::localeDiskList()) {
                DiskListItem *item = new DiskListItem(info, left_list);

                left_list->addItem(item);

                if (!info.isReadonly()) {
                    item = new DiskListItem(info, right_list);

                    right_list->addItem(item);
                }
            }
        } else {
            setLeftTitle(tr("请选择您要克隆的原始分区"));
            setRightTitle(tr("请选择您想克隆的目标分区"));

            for (const DPartInfo &info : DDevicePartInfo::localePartList()) {
                PartitionListItem *item = new PartitionListItem(info, left_list);

                left_list->addItem(item);

                if (!info.isReadonly()) {
                    item = new PartitionListItem(info, right_list);

                    right_list->addItem(item);
                }
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

static QString getFilePath(const QWidget *widget)
{
    if (const UtilityList *list = qobject_cast<const UtilityList*>(widget)) {
        const QWidget *widget = list->itemWidget(list->selectedItems().first());

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

QString SelectFilePage::source() const
{
    return getFilePath(leftContent());
}

QString SelectFilePage::target() const
{
    return getFilePath(rightContent());
}

#include "selectfilepage.moc"

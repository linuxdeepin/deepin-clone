#include "selectactionpage.h"
#include "iconlabel.h"
#include "utilitylist.h"

#include <QListView>
#include <QHBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QTimer>

SelectActionPage::SelectActionPage(QWidget *parent)
    : ContentPairPage(parent)
{
    init();
}

void SelectActionPage::init()
{
    setLeftTitle(tr("选择针对磁盘还是分区的操作"));
    setRightTitle(tr("选择相对应的功能选项"));

    QWidget *left_content = new QWidget(this);
    QHBoxLayout *left_layout = new QHBoxLayout(left_content);

    left_content->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    IconLabel *disk_item = new IconLabel(left_content);

    disk_item->setTitle(tr("Disk"));
    disk_item->setIcon(QIcon(":/icons/disk_96.svg"), 80);
    disk_item->setDirection(QBoxLayout::TopToBottom);

    IconLabel *partition_item = new IconLabel(left_content);

    partition_item->setTitle(tr("Partition"));
    partition_item->setIcon(QIcon(":/icons/partition_96.svg"), 80);
    partition_item->setDirection(QBoxLayout::TopToBottom);

    left_layout->addWidget(disk_item);
    left_layout->addWidget(partition_item);

    setLeftContent(left_content);

    UtilityList *right_content = new UtilityList(this);

    right_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_content->addItem(QIcon(":/icons/clone_disk_to_disk.svg"), 0, QSize(120, 49));
    right_content->addItem(QIcon(":/icons/clone_disk_to_image.svg"), 0, QSize(120, 49));
    right_content->addItem(QIcon(":/icons/restore_image_to_disk.svg"), 0, QSize(120, 49));

    setRightContent(right_content, true);
}

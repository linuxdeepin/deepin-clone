#include "partitionlistitem.h"

#include <QIcon>

PartitionListItem::PartitionListItem(DPartInfo info, QWidget *parent)
    : UtilityListDeviceItem(parent)
    , m_info(info)
{
    setIcon(QIcon::fromTheme("drive-removable-media"), QSize(48, 48));
    setTitle(info.mountPoint().isEmpty() ? info.partLabel() : info.mountPoint());
    setMessage(info.name());
    setSize(info.usedSize(), info.totalSize());
}

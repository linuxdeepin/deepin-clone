#include "disklistitem.h"

#include <QIcon>

DiskListItem::DiskListItem(DDiskInfo info, QWidget *parent)
    : UtilityListDeviceItem(parent)
    , m_info(info)
{
    setIcon(QIcon::fromTheme("drive-harddisk"), QSize(48, 48));
    setTitle(info.model());
    setMessage(info.name());
    setSize(info.totalReadableDataSize(), info.totalSize());
}

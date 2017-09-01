#include "disklistitem.h"

#include <QIcon>

DiskListItem::DiskListItem(DDiskInfo info, QWidget *parent)
    : UtilityListDeviceItem(parent)
    , m_info(info)
{
    setTitle(info.model());
    setMessage(info.name());
    setSize(info.usedSize(), info.totalSize());

    QString icon_name;

    if (info.isRemoveable()) {
        if (info.transport().isEmpty())
            icon_name = "drive-removable-media";
        else
            icon_name = "drive-removable-media-" + info.transport();
    } else {
        if (info.transport().isEmpty())
            icon_name = "drive-harddisk";
        else
            icon_name = "drive-harddisk-" + info.transport();
    }

    setIcon(QIcon::fromTheme(icon_name), QSize(48, 48));
}

DDiskInfo DiskListItem::info() const
{
    return m_info;
}

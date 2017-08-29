#ifndef DISKLISTITEM_H
#define DISKLISTITEM_H

#include "utilitylistdeviceitem.h"
#include "ddiskinfo.h"

class DiskListItem : public UtilityListDeviceItem
{
    Q_OBJECT
public:
    explicit DiskListItem(DDiskInfo info, QWidget *parent = 0);

private:
    DDiskInfo m_info;
};

#endif // DISKLISTITEM_H

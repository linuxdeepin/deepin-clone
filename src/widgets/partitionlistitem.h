#ifndef PARTITIONLISTITEM_H
#define PARTITIONLISTITEM_H

#include "utilitylistdeviceitem.h"
#include "dpartinfo.h"

class PartitionListItem : public UtilityListDeviceItem
{
    Q_OBJECT

public:
    explicit PartitionListItem(DPartInfo info, QWidget *parent = 0);

    DPartInfo info() const;

private:
    DPartInfo m_info;
};

#endif // PARTITIONLISTITEM_H

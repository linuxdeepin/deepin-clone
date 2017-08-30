#ifndef DPARTINFO_P_H
#define DPARTINFO_P_H

#include <QSharedData>

#include "dpartinfo.h"

class DPartInfoPrivate : public QSharedData
{
public:
    DPartInfoPrivate(DPartInfo *qq);

    virtual void refresh();

    DPartInfo::Type toType(const QString &name);

    DPartInfo *q;

    QString filePath;
    QString name;
    QString kname;
    int blockSize = 4096;
    // Unit: bytes
    qint64 sizeStart = 0;
    qint64 sizeEnd = 0;
    qint64 size = 0;
    qint64 usedSize = 0;
    qint64 freeSize = 0;
    QString typeName;
    DPartInfo::Type type;
    QString mountPoint;
    QString label;
    QString partLabel;
    QString partType;
    DPartInfo::GUIDType guidType;

    bool readonly = false;
    bool removeable = false;
};

#endif // DPARTINFO_P_H

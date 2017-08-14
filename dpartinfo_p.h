#ifndef DPARTINFO_P_H
#define DPARTINFO_P_H

#include <QSharedData>

#include "dpartinfo.h"

class DPartInfoPrivate : public QSharedData
{
public:
    DPartInfoPrivate(DPartInfo *qq);

    virtual QString device() const {return QString();}
    virtual void refresh() = 0;

    DPartInfo::Type toType(const QString &name);

    DPartInfo *q;

    QString name;
    QString kname;
    // Unit: bytes
    quint64 sizeStart = 0;
    quint64 sizeEnd = 0;
    quint64 size = 0;
    QString sizeDisplay;
    QString typeName;
    DPartInfo::Type type;
    QString mountPoint;
    QString label;
    QString partLabel;
    QString partType;
    DPartInfo::GUIDType guidType;
};

#endif // DPARTINFO_P_H

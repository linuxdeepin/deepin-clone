#ifndef DDISKINFO_P_H
#define DDISKINFO_P_H

#include <QSharedData>

#include "ddiskinfo.h"

class DDiskInfoPrivate : public QSharedData
{
public:
    DDiskInfoPrivate(DDiskInfo *qq);

    virtual QString device() const {return QString();}
    virtual void refresh() = 0;

    DDiskInfo *q;

    QString name;
    QString kname;
    quint64 size;
    QString sizeDisplay;
    QString typeName;
    DDiskInfo::Type type;
    QList<DPartInfo> children;
    QString ptTypeName;
    DDiskInfo::PTType ptType;
};

#endif // DDISKINFO_P_H

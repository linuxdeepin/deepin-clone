#ifndef DDEVICEPARTINFO_H
#define DDEVICEPARTINFO_H

#include "dpartinfo.h"

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class DDevicePartInfoPrivate;
class DDevicePartInfo : public DPartInfo
{
public:
    explicit DDevicePartInfo();
    explicit DDevicePartInfo(const QString &name);

    static QList<DDevicePartInfo> localePartList();

private:
    void init(const QJsonObject &obj);

    DDevicePartInfoPrivate *d_func() const;

    friend class DDeviceDiskInfoPrivate;
};

#endif // DDEVICEPARTINFO_H

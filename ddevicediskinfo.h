#ifndef DDEVICEDISKINFO_H
#define DDEVICEDISKINFO_H

#include "ddiskinfo.h"

class DDeviceDiskInfoPrivate;
class DDeviceDiskInfo : public DDiskInfo
{
public:
    explicit DDeviceDiskInfo();
    explicit DDeviceDiskInfo(const QString &name);

    static QList<DDeviceDiskInfo> localeDiskList();

private:
    DDeviceDiskInfoPrivate *d_func() const;
};

#endif // DDEVICEDISKINFO_H

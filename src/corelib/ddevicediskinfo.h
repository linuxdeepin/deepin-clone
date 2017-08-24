#ifndef DDEVICEDISKINFO_H
#define DDEVICEDISKINFO_H

#include "dglobal.h"
#include "ddiskinfo.h"

class DDeviceDiskInfoPrivate;
class DDeviceDiskInfo : public DDiskInfo
{
public:
    explicit DDeviceDiskInfo();
    explicit DDeviceDiskInfo(const QString &name);

    static QList<DDeviceDiskInfo> localeDiskList();

private:
    DG_DFUNC(DDeviceDiskInfo)
};

#endif // DDEVICEDISKINFO_H

#ifndef DFILEDISKINFO_H
#define DFILEDISKINFO_H

#include "dglobal.h"
#include "ddiskinfo.h"

class DFileDiskInfoPrivate;
class DFileDiskInfo : public DDiskInfo
{
public:
    explicit DFileDiskInfo();
    explicit DFileDiskInfo(const QString &fileName);

private:
    DG_DFUNC(DFileDiskInfo)
};

#endif // DFILEDISKINFO_H

#ifndef DFILEPARTINFO_H
#define DFILEPARTINFO_H

#include "dpartinfo.h"
#include "dglobal.h"

class DFilePartInfoPrivate;
class DVirtualImageFileIO;
class DFilePartInfo : public DPartInfo
{
public:
    explicit DFilePartInfo();
    explicit DFilePartInfo(const QString &fileName);

private:
    void init(const QString &filePath, DVirtualImageFileIO *io);

    DG_DFUNC(DFilePartInfo)
};

#endif // DFILEPARTINFO_H

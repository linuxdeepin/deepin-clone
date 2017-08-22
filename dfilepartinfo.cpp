#include "dfilepartinfo.h"
#include "dpartinfo_p.h"
#include "dvirtualimagefileio.h"

#include <QFileInfo>

class DFilePartInfoPrivate : public DPartInfoPrivate
{
public:
    DFilePartInfoPrivate(DFilePartInfo *qq);

    void init(const QString &filePath, DVirtualImageFileIO *io);

    void refresh() Q_DECL_OVERRIDE;
};

DFilePartInfoPrivate::DFilePartInfoPrivate(DFilePartInfo *qq)
    : DPartInfoPrivate(qq)
{

}

void DFilePartInfoPrivate::init(const QString &filePath, DVirtualImageFileIO *io)
{

}

void DFilePartInfoPrivate::refresh()
{

}

DFilePartInfo::DFilePartInfo()
{

}

DFilePartInfo::DFilePartInfo(const QString &fileName)
{
    DVirtualImageFileIO io(QFileInfo(fileName).absolutePath());

    if (io.isValid()) {
        init(fileName, &io);
    }
}

void DFilePartInfo::init(const QString &filePath, DVirtualImageFileIO *io)
{
    if (!d)
        d = new DFilePartInfoPrivate(this);

    d_func()->init(filePath, io);
}

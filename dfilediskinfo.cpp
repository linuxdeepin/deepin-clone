#include "dfilediskinfo.h"
#include "ddiskinfo_p.h"

#include <QString>

class DFileDiskInfoPrivate : public DDiskInfoPrivate
{
public:
    DFileDiskInfoPrivate(DFileDiskInfo *qq);

    QString filePath() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    QString m_filePath;
};

DFileDiskInfoPrivate::DFileDiskInfoPrivate(DFileDiskInfo *qq)
    : DDiskInfoPrivate(qq)
{

}

QString DFileDiskInfoPrivate::filePath() const
{
    return m_filePath;
}

void DFileDiskInfoPrivate::refresh()
{

}

DFileDiskInfo::DFileDiskInfo()
    : DDiskInfo(new DFileDiskInfoPrivate(this))
{

}

DFileDiskInfo::DFileDiskInfo(const QString &fileName)
    : DFileDiskInfo()
{
    d_func()->m_filePath = fileName;
}

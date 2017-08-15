#include "dfilediskinfo.h"
#include "ddiskinfo_p.h"

#include <QString>

class DFileDiskInfoPrivate : public DDiskInfoPrivate
{
public:
    DFileDiskInfoPrivate(DFileDiskInfo *qq);

    QString filePath() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    bool hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const Q_DECL_OVERRIDE;
    bool openDataStream(int index) Q_DECL_OVERRIDE;
    void closeDataStream() Q_DECL_OVERRIDE;

    qint64 read(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 maxSize) Q_DECL_OVERRIDE;

    bool atEnd() const Q_DECL_OVERRIDE;

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

bool DFileDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const
{

}

bool DFileDiskInfoPrivate::openDataStream(int index)
{

}

void DFileDiskInfoPrivate::closeDataStream()
{

}

qint64 DFileDiskInfoPrivate::read(char *data, qint64 maxSize)
{

}

qint64 DFileDiskInfoPrivate::write(const char *data, qint64 maxSize)
{

}

bool DFileDiskInfoPrivate::atEnd() const
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

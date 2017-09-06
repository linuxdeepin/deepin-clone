#include "dfilediskinfo.h"
#include "ddiskinfo_p.h"
#include "dpartinfo_p.h"
#include "dvirtualimagefileio.h"
#include "dzlibfile.h"

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class DFileDiskInfoPrivate : public DDiskInfoPrivate
{
public:
    DFileDiskInfoPrivate(DFileDiskInfo *qq);

    void init(const QString &filePath, DVirtualImageFileIO *io);

    QString filePath() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    bool hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const Q_DECL_OVERRIDE;
    bool openDataStream(int index) Q_DECL_OVERRIDE;
    void closeDataStream() Q_DECL_OVERRIDE;

    // Unfulfilled
    qint64 readableDataSize(DDiskInfo::DataScope scope) const Q_DECL_OVERRIDE;

    qint64 totalReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 maxReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 totalWritableDataSize() const Q_DECL_OVERRIDE;
    bool setTotalWritableDataSize(qint64 size) Q_DECL_OVERRIDE;

    qint64 read(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 maxSize) Q_DECL_OVERRIDE;

    bool atEnd() const Q_DECL_OVERRIDE;

    QString errorString() const Q_DECL_OVERRIDE;

    QString m_filePath;
    DZlibFile m_file;
};

DFileDiskInfoPrivate::DFileDiskInfoPrivate(DFileDiskInfo *qq)
    : DDiskInfoPrivate(qq)
{

}

static QString getDIMFilePath(const QString &base, const QString &file)
{
    return QString("dim://%1/%2").arg(base).arg(file);
}

void DFileDiskInfoPrivate::init(const QString &filePath, DVirtualImageFileIO *io)
{
    Q_UNUSED(io)

    m_filePath.clear();

    DZlibFile file_pt(getDIMFilePath(filePath, "info.json"));

    if (file_pt.open(QIODevice::ReadOnly)) {
        const QByteArray &data = file_pt.readAll();

        initFromJson(data);
    } else {
        name = filePath;
        kname = filePath;
        size = INT64_MAX;
    }

    m_filePath = filePath;
    readonly = true;

    int index = 1;

    for (const DPartInfo &part : children) {
        part.d->filePath = getDIMFilePath(m_filePath, QString::number(index));
        part.d->parentDiskFilePath = m_filePath;
        part.d->readonly = true;
        ++index;
    }

    typeName = "dim";
}

QString DFileDiskInfoPrivate::filePath() const
{
    return m_filePath;
}

void DFileDiskInfoPrivate::refresh()
{
    children.clear();

    DVirtualImageFileIO io(m_filePath);

    if (io.isValid()) {
        init(m_filePath, &io);
    }
}

bool DFileDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const
{
    if (mode == DDiskInfo::Read) {
        if (scope == DDiskInfo::Headgear || scope == DDiskInfo::PartitionTable)
            return havePartitionTable;
        else if (scope == DDiskInfo::Partition)
            return !children.isEmpty();
        else if (scope == DDiskInfo::JsonInfo)
            return QFile::exists(getDIMFilePath(m_filePath, "info.json"));
    } else {
        return true;
    }

    return false;
}

bool DFileDiskInfoPrivate::openDataStream(int index)
{
    switch (currentScope) {
    case DDiskInfo::Headgear: {
        m_file.setFileName(getDIMFilePath(m_filePath, "headgear"));
        break;
    }
    case DDiskInfo::PartitionTable: {
        m_file.setFileName(getDIMFilePath(m_filePath, "pt.json"));
        break;
    }
    case DDiskInfo::Partition: {
        m_file.setFileName(getDIMFilePath(m_filePath, QString::number(index)));
        break;
    }
    case DDiskInfo::JsonInfo: {
        m_file.setFileName(getDIMFilePath(m_filePath, "info.json"));
        break;
    }
    default:
        break;
    }

    if (currentMode == DDiskInfo::Read)
        return m_file.open(QIODevice::ReadOnly);

    bool ok = m_file.open(QIODevice::WriteOnly);

    if (!ok) {
        setErrorString(QObject::tr("Device open failed, %1").arg(m_file.errorString()));
    }

    return ok;
}

void DFileDiskInfoPrivate::closeDataStream()
{
    m_file.close();

    if (currentMode == DDiskInfo::Write && currentScope == DDiskInfo::JsonInfo) {
        DVirtualImageFileIO io(m_filePath);

        if (io.isValid()) {
            QFile::resize(m_filePath, io.metaDataSize() + io.fileDataSize());
        }

        refresh();
    }
}

qint64 DFileDiskInfoPrivate::readableDataSize(DDiskInfo::DataScope scope) const
{
    Q_UNUSED(scope)

    return -1;
}

qint64 DFileDiskInfoPrivate::totalReadableDataSize() const
{
    DVirtualImageFileIO io(m_filePath);

    if (!io.isValid())
        return 0;

    qint64 size = 0;

    for (const QString &file : io.fileList())
        size += DZlibFile(getDIMFilePath(m_filePath, file)).size();

    return size;
}

qint64 DFileDiskInfoPrivate::maxReadableDataSize() const
{
    qint64 size = 0;

    if (children.isEmpty()) {
        size = DZlibFile(getDIMFilePath(m_filePath, "headgear")).size();

        return size < 0 ? 0 : size;
    }

    DVirtualImageFileIO io(m_filePath);

    for (int i = children.count() - 1; i >= 0; --i) {
        const QString &part_file_name = QString::number(i);

        if (io.existes(part_file_name))
            size += io.size(part_file_name);
    }

    return size;
}

qint64 DFileDiskInfoPrivate::totalWritableDataSize() const
{
    DVirtualImageFileIO io(m_filePath);

    if (!io.isValid())
        return 0;

    return io.writableDataSize();
}

bool DFileDiskInfoPrivate::setTotalWritableDataSize(qint64 size)
{
    DVirtualImageFileIO io(m_filePath);

    if (!io.isValid())
        return false;

    return io.setSize(size + DVirtualImageFileIO::metaDataSize());
}

qint64 DFileDiskInfoPrivate::read(char *data, qint64 maxSize)
{
    return m_file.read(data, maxSize);
}

qint64 DFileDiskInfoPrivate::write(const char *data, qint64 maxSize)
{
    return m_file.write(data, maxSize);
}

bool DFileDiskInfoPrivate::atEnd() const
{
    return m_file.atEnd();
}

QString DFileDiskInfoPrivate::errorString() const
{
    if (!error.isEmpty())
        return error;

    return m_file.errorString();
}

DFileDiskInfo::DFileDiskInfo()
{

}

DFileDiskInfo::DFileDiskInfo(const QString &fileName)
{
    DVirtualImageFileIO io(fileName);

    if (io.isValid()) {
        d = new DFileDiskInfoPrivate(this);
        d_func()->init(fileName, &io);
    }
}

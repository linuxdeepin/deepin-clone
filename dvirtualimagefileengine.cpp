#include "dvirtualimagefileengine.h"
#include "dvirtualimagefileio.h"

#include <QDebug>

QAbstractFileEngine *DVirtualImageFileEngineHandler::create(const QString &fileName) const
{
    if (!fileName.startsWith("dim://"))
        return 0;

    QFileInfo file_info(fileName.mid(6));

    DVirtualImageFileIO *info = new DVirtualImageFileIO(file_info.absolutePath());

    if (!info->isValid()) {
        delete info;

        return 0;
    }

    return new DVirtualImageFileEngine(info, file_info.fileName());
}

static DVirtualImageFileEngineHandler _global_dim_helper;

DVirtualImageFileEngine::DVirtualImageFileEngine(DVirtualImageFileIO *info, const QString &fileName)
    : m_info(info)
    , m_name(fileName)
{

}

DVirtualImageFileEngine::~DVirtualImageFileEngine()
{
    close();
}

bool DVirtualImageFileEngine::open(QIODevice::OpenMode openMode)
{
    return m_info->open(m_name, openMode);
}

bool DVirtualImageFileEngine::close()
{
    return m_info->close();
}

bool DVirtualImageFileEngine::flush()
{
    return m_info->m_file.flush();
}

qint64 DVirtualImageFileEngine::size() const
{
    return m_info->size(m_name);
}

qint64 DVirtualImageFileEngine::pos() const
{
    return m_info->pos();
}

bool DVirtualImageFileEngine::seek(qint64 pos)
{
    return m_info->seek(pos);
}

bool DVirtualImageFileEngine::isSequential() const
{
    return m_info->m_file.isSequential();
}

bool DVirtualImageFileEngine::setSize(qint64 size)
{
    return m_info->setSize(m_name, size);
}

QAbstractFileEngine::FileFlags DVirtualImageFileEngine::fileFlags(QAbstractFileEngine::FileFlags type) const
{
    FileFlags flags = 0;

    if (type & TypesMask) {
        flags |= FileType;
    }

    if ((type & FlagsMask) && m_info->existes(m_name)) {
        flags |= ExistsFlag;
    }

    if ((type & PermsMask) && m_info->existes(m_name)) {
        flags |= (FileFlags)(int)m_info->m_file.permissions();

        if (!m_info->isWritable(m_name)) {
            flags &= ~(WriteGroupPerm | WriteOtherPerm | WriteOwnerPerm | WriteUserPerm);
        }
    }

    return flags;
}

QString DVirtualImageFileEngine::fileName(QAbstractFileEngine::FileName file) const
{
    Q_UNUSED(file)

    return m_name;
}

void DVirtualImageFileEngine::setFileName(const QString &file)
{
    m_info->rename(m_name, file);
}

qint64 DVirtualImageFileEngine::read(char *data, qint64 maxlen)
{
    if (!m_info->isOpen(m_name))
        return -1;

    maxlen = qMin(maxlen, m_info->size(m_name));

    return m_info->m_file.read(data, maxlen);
}

qint64 DVirtualImageFileEngine::readLine(char *data, qint64 maxlen)
{
    if (!m_info->isOpen(m_name))
        return -1;

    maxlen = qMin(maxlen, m_info->size(m_name));

    return m_info->m_file.readLine(data, maxlen);
}

qint64 DVirtualImageFileEngine::write(const char *data, qint64 len)
{
    if (!m_info->isOpen(m_name))
        return -1;

    return m_info->m_file.write(data, len);
}

bool DVirtualImageFileEngine::extension(QAbstractFileEngine::Extension extension, const QAbstractFileEngine::ExtensionOption *option, QAbstractFileEngine::ExtensionReturn *output)
{
    Q_UNUSED(option)
    Q_UNUSED(output)

    return extension == AtEndExtension && (m_info->pos() < 0 || m_info->pos() == m_info->size(m_name));
}

bool DVirtualImageFileEngine::supportsExtension(QAbstractFileEngine::Extension extension) const
{
    return extension == AtEndExtension;
}

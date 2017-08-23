#include "dvirtualimagefileio.h"
#include "helper.h"
#include "dglobal.h"

#include <QDataStream>

DVirtualImageFileIO::DVirtualImageFileIO(const QString &fileName)
{
    setFile(fileName);
}

DVirtualImageFileIO::~DVirtualImageFileIO()
{
    close();
}

template<typename T>
T getData(QDataStream &stream)
{
    T t;

    stream >> t;

    return t;
}

bool DVirtualImageFileIO::setFile(const QString &fileName)
{
    m_isValid = false;
    m_file.close();

    if (!fileName.endsWith(".dim"))
        return false;

    m_file.setFileName(fileName);

    if (!m_file.exists())
        return false;

    if (m_file.size() > 0) {
        if (m_file.size() < metaDataSize()) {
            dCDebug("Not a valid dim file");

            return false;
        }

        if (!m_file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QDataStream stream(&m_file);

        stream.setVersion(QDataStream::Qt_5_6);

        if (getData<quint8>(stream) != 0xdd) {
            dCDebug("The 1dth character should be 0xdd");

            m_file.close();

            return false;
        }

        stream >> m_version;

        if (m_version != 1) {
            dCDebug("Unsupported version: %d", (int)m_version);

            m_file.close();

            return false;
        }

        quint8 file_count;

        stream >> file_count;

        if (m_file.size() < 3 + file_count * 80) {
            dCDebug("Not a valid dim file");

            m_file.close();

            return false;
        }

        for (quint8 i = 0; i < file_count; ++i) {
            if (getData<quint8>(stream) != 0xdd) {
                dCDebug("The %lldth character should be 0xdd", m_file.pos());

                m_file.close();

                return false;
            }

            FileInfo info;

            info.name = QString::fromUtf8(m_file.read(63));
            info.index = i;

            stream >> info.start;
            stream >> info.end;

            m_fileMap[info.name] = info;
        }
    } else if (m_file.open(QIODevice::WriteOnly)) {
        m_file.resize(metaDataSize());
        m_file.putChar(0xdd);
        m_file.putChar(1);
        m_file.putChar(0);
    } else {
        return false;
    }

    m_file.close();
    m_isValid = true;

    return true;
}

bool DVirtualImageFileIO::setSize(qint64 size)
{
    return m_file.resize(size);
}

bool DVirtualImageFileIO::isValid() const
{
    return m_isValid;
}

bool DVirtualImageFileIO::existes(const QString &fileName) const
{
    return m_fileMap.contains(fileName);
}

bool DVirtualImageFileIO::isOpen(const QString &fileName) const
{
    return m_openedFile == fileName;
}

bool DVirtualImageFileIO::open(const QString &fileName, QIODevice::OpenMode openMode)
{
    if (m_file.isOpen() || !isValid())
        return false;

    if (openMode.testFlag(QIODevice::NotOpen))
        return false;

    if (openMode & (QIODevice::WriteOnly | QIODevice::Append)) {
        if (!isWritable(fileName)) {
            return false;
        }
    } else if (!existes(fileName)) {
        return false;
    }

    if (!existes(fileName)) {
        addFile(fileName);
    }

    if (!m_file.open(openMode | QIODevice::ReadOnly))
        return false;

    const FileInfo &info = m_fileMap.value(fileName);

    m_file.seek(info.start);
    m_openedFile = fileName;

    return true;
}

bool DVirtualImageFileIO::close()
{
    if (!m_file.isOpen())
        return false;

    const QFile::OpenMode open_mode = m_file.openMode();

    m_file.close();

    if (!m_openedFile.isEmpty() && open_mode.testFlag(QFile::WriteOnly)) {
        const FileInfo &info = m_fileMap.value(m_openedFile);

        setSize(m_openedFile, info.end - info.start);
    }

    m_openedFile.clear();

    return m_file.error() == QFile::NoError;
}

qint64 DVirtualImageFileIO::pos() const
{
    if (m_openedFile.isEmpty())
        return -1;

    const FileInfo &info = m_fileMap.value(m_openedFile);

    qint64 pos = m_file.pos();

    if (pos < info.start || pos > info.end)
        return -1;

    return m_file.pos() - info.start;
}

bool DVirtualImageFileIO::seek(qint64 pos)
{
    if (pos < 0)
        return false;

    if (m_openedFile.isEmpty())
        return -1;

    const FileInfo &info = m_fileMap.value(m_openedFile);

    return m_file.seek(info.start + pos);
}

qint64 DVirtualImageFileIO::read(char *data, qint64 maxlen)
{
    maxlen = qMin(maxlen, m_fileMap.value(m_openedFile).end - m_file.pos());

    return m_file.read(data, maxlen);
}

qint64 DVirtualImageFileIO::write(const char *data, qint64 len)
{
    len = m_file.write(data, len);

    FileInfo &info = m_fileMap[m_openedFile];
    info.end = qMax(info.end, m_file.pos());

    return len;
}

qint64 DVirtualImageFileIO::size(const QString &fileName) const
{
    if (!m_fileMap.contains(fileName))
        return -1;

    const FileInfo &info = m_fileMap.value(fileName);

    return info.end - info.start;
}

qint64 DVirtualImageFileIO::start(const QString &fileName) const
{
    if (!m_fileMap.contains(fileName))
        return -1;

    const FileInfo &info = m_fileMap.value(fileName);

    return info.start;
}

qint64 DVirtualImageFileIO::end(const QString &fileName) const
{
    if (!m_fileMap.contains(fileName))
        return -1;

    const FileInfo &info = m_fileMap.value(fileName);

    return info.end;
}

bool DVirtualImageFileIO::setSize(const QString &fileName, qint64 size)
{
    if (!isWritable(fileName))
        return false;

    if (size < 0)
        return false;

    if (!m_file.open(QIODevice::ReadWrite))
        return false;

    m_fileMap[fileName].end = m_fileMap.value(fileName).start + size;
    m_file.seek(3 + m_fileMap.count() * 80 - 8);

    QDataStream stream(&m_file);

    stream.setVersion(QDataStream::Qt_5_6);
    stream << m_fileMap.value(fileName).end;

    m_file.close();

    return m_file.error() == QFile::NoError;
}

bool DVirtualImageFileIO::rename(const QString &from, const QString &to)
{
    if (!existes(from))
        return false;

    FileInfo info = m_fileMap.take(from);

    info.name = to;

    qint64 pos = m_file.pos();

    if (!m_file.seek(3 + info.index * 80 + 1))
        return false;

    const QByteArray &file_name = to.toUtf8();

    if (file_name.size() > 63) {
        dCDebug("File name length exceeds limit");

        return false;
    }

    m_file.write(file_name);

    if (file_name.size() < 63) {
        char empty_ch[63 - file_name.size()] = {0};

        m_file.write(empty_ch, 63 - file_name.size());
    }

    m_file.seek(pos);

    return true;
}

bool DVirtualImageFileIO::isWritable(const QString &fileName)
{
    if (!existes(fileName))
        return true;

    const FileInfo &info = m_fileMap.value(fileName);

    return info.index == m_fileMap.count() - 1;
}

int DVirtualImageFileIO::maxFileCount()
{
    return UINT8_MAX;
}

qint64 DVirtualImageFileIO::metaDataSize()
{
    return 24 * 1024;
}

qint64 DVirtualImageFileIO::validMetaDataSize() const
{
    return 3 + m_fileMap.count() * 80;
}

qint64 DVirtualImageFileIO::fileDataSize() const
{
    if (m_fileMap.isEmpty())
        return 0;

    qint64 max_end = 0;

    for (const FileInfo &info : m_fileMap) {
        max_end = qMax(max_end, info.end);
    }

    return max_end - metaDataSize();
}

qint64 DVirtualImageFileIO::writableDataSize() const
{
    return m_file.size() - fileDataSize() - metaDataSize();
}

QStringList DVirtualImageFileIO::fileList() const
{
    return m_fileMap.keys();
}

bool DVirtualImageFileIO::addFile(const QString &name)
{
    if (!m_file.open(QIODevice::ReadWrite)) {
        return false;
    }

    qint64 start = validMetaDataSize();

    m_file.seek(start);

    if (!m_file.putChar(0xdd)) {
        m_file.close();

        return false;
    }

    const QByteArray &file_name = name.toUtf8();

    if (file_name.size() > 63) {
        dCDebug("File name length exceeds limit");

        m_file.close();

        return false;
    }

    m_file.write(file_name);

    if (file_name.size() < 63) {
        char empty_ch[63 - file_name.size()] = {0};

        m_file.write(empty_ch, 63 - file_name.size());
    }

    FileInfo info;

    info.name = name;
    info.start = metaDataSize() + fileDataSize();
    info.end = info.start;
    info.index = m_fileMap.count();

    m_fileMap[name] = info;

    QDataStream stream(&m_file);

    stream.setVersion(QDataStream::Qt_5_6);
    stream << info.start;
    stream << info.end;

    m_file.seek(2);
    m_file.putChar(m_fileMap.count());
    m_file.close();

    return true;
}

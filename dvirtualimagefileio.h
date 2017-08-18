#ifndef DVIRTUALIMAGEFILEIO_H
#define DVIRTUALIMAGEFILEIO_H

#include <QFile>
#include <QMap>

class DVirtualImageFileIO
{
public:
    explicit DVirtualImageFileIO(const QString &fileName);
    ~DVirtualImageFileIO();

    bool setFile(const QString &fileName);
    bool isValid() const;

    bool existes(const QString &fileName) const;

    bool isOpen(const QString &fileName) const;
    bool open(const QString &fileName, QIODevice::OpenMode openMode = QIODevice::ReadOnly);
    bool close();
    qint64 pos() const;
    bool seek(qint64 pos);

    qint64 size(const QString &fileName) const;
    bool rename(const QString &from, const QString &to);
    bool isWritable(const QString &fileName);

    qint64 metaDataSize() const;

private:
    bool addFile(const QString &name);

    bool m_isValid = false;

    QFile m_file;

    quint8 m_version;

    struct FileInfo {
        quint8 index;
        QString name;
        quint64 start;
        quint64 end;
    };

    QMap<QString, FileInfo> m_fileMap;
    QString m_openedFile;

    friend class DVirtualImageFileEngine;
};

#endif // DVIRTUALIMAGEFILEIO_H

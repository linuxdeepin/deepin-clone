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
    bool setSize(qint64 size);
    bool isValid() const;

    bool existes(const QString &fileName) const;

    bool isOpen(const QString &fileName) const;
    bool open(const QString &fileName, QIODevice::OpenMode openMode = QIODevice::ReadOnly);
    bool close();
    qint64 pos() const;
    bool seek(qint64 pos);

    qint64 size(const QString &fileName) const;
    bool resize(const QString &fileName);
    bool setSize(const QString &fileName, qint64 size);
    bool rename(const QString &from, const QString &to);
    bool isWritable(const QString &fileName);

    static int maxFileCount();
    static qint64 metaDataSize();
    qint64 validMetaDataSize() const;
    qint64 fileDataSize() const;
    qint64 writableDataSize() const;
    QStringList fileList() const;

private:
    bool addFile(const QString &name);

    bool m_isValid = false;

    QFile m_file;

    quint8 m_version;

    struct FileInfo {
        quint8 index;
        QString name;
        qint64 start;
        qint64 end;
    };

    QMap<QString, FileInfo> m_fileMap;
    QString m_openedFile;

    friend class DVirtualImageFileEngine;
};

#endif // DVIRTUALIMAGEFILEIO_H

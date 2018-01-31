/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DVIRTUALIMAGEFILEIO_H
#define DVIRTUALIMAGEFILEIO_H

#include <QHash>
#include <QExplicitlySharedDataPointer>
#include <QFile>

class DVirtualImageFileIOPrivate;
class DVirtualImageFileIO
{
public:
    explicit DVirtualImageFileIO(const QString &fileName);
    ~DVirtualImageFileIO();

    QString fileName();
    bool setFile(const QString &fileName);
    bool setSize(qint64 size);
    bool isValid() const;

    bool existes(const QString &fileName) const;

    bool isOpen(const QString &fileName) const;
    bool open(const QString &fileName, QIODevice::OpenMode openMode = QIODevice::ReadOnly);
    bool close();
    qint64 pos() const;
    bool seek(qint64 pos);

    bool flush();
    bool isSequential() const;
    QFileDevice::Permissions permissions() const;

    qint64 read(char *data, qint64 maxlen);
    qint64 write(const char *data, qint64 len);

    qint64 size(const QString &fileName) const;
    qint64 start(const QString &fileName) const;
    qint64 end(const QString &fileName) const;
    bool setSize(const QString &fileName, qint64 size);
    bool rename(const QString &from, const QString &to);
    bool isWritable(const QString &fileName);

    static int maxFileCount();
    static qint64 metaDataSize();
    qint64 validMetaDataSize() const;
    qint64 fileDataSize() const;
    qint64 writableDataSize() const;
    QStringList fileList() const;

    static bool updateMD5sum(const QString &fileName);

private:
    bool addFile(const QString &name);
    QByteArray md5sum(bool readCache = true);
    bool updateMD5sum();

    QExplicitlySharedDataPointer<DVirtualImageFileIOPrivate> d;

    friend class DVirtualImageFileEngine;
};

#endif // DVIRTUALIMAGEFILEIO_H

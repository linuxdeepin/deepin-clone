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

#ifndef DVIRTUALIMAGEFILEENGINE_H
#define DVIRTUALIMAGEFILEENGINE_H

#include <private/qabstractfileengine_p.h>

class DVirtualImageFileIO;

class DVirtualImageFileEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString &fileName) const Q_DECL_OVERRIDE;
};

class DVirtualImageFileIterator : public QAbstractFileEngineIterator
{
public:
    DVirtualImageFileIterator(QDir::Filters filters, const QStringList &nameFilters);

    QString next() override;
    bool hasNext() const override;

    QString currentFileName() const override;

private:
    int index = 0;
    QStringList list;

    friend class DVirtualImageFileEngine;
};

class DVirtualImageFileEngine : public QAbstractFileEngine
{
public:
    explicit DVirtualImageFileEngine(DVirtualImageFileIO *info, const QString &fileName);
    ~DVirtualImageFileEngine();

    bool open(QIODevice::OpenMode openMode) Q_DECL_OVERRIDE;
    bool close() Q_DECL_OVERRIDE;
    bool flush() Q_DECL_OVERRIDE;

    qint64 size() const Q_DECL_OVERRIDE;
    qint64 pos() const Q_DECL_OVERRIDE;
    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    bool isSequential() const Q_DECL_OVERRIDE;

    bool setSize(qint64 size) Q_DECL_OVERRIDE;

    FileFlags fileFlags(FileFlags type=FileInfoAll) const Q_DECL_OVERRIDE;
    QString fileName(FileName file=DefaultName) const Q_DECL_OVERRIDE;

    void setFileName(const QString &file) Q_DECL_OVERRIDE;

    typedef DVirtualImageFileIterator Iterator;
    Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames) override;
    Iterator *endEntryList() override;

    qint64 read(char *data, qint64 maxlen) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 len) Q_DECL_OVERRIDE;

    bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0) Q_DECL_OVERRIDE;
    bool supportsExtension(Extension extension) const Q_DECL_OVERRIDE;

private:
    DVirtualImageFileIO *m_info;
    QString m_name;
    QStringList m_fileList;
};

#endif // DVIRTUALIMAGEFILEENGINE_H

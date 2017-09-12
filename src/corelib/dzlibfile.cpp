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

#include "dzlibfile.h"

#include <QDebug>

DZlibFile::DZlibFile(const QString &name)
    : m_file(new QFile(this))
{
    setFileName(name);
    setDevice(m_file);
}

DZlibFile::DZlibFile(QObject *parent)
    : DZlibIODevice(parent)
    , m_file(new QFile(this))
{
    setDevice(m_file);
}

DZlibFile::DZlibFile(const QString &name, QObject *parent)
    : DZlibIODevice(parent)
    , m_file(new QFile(this))
{
    setFileName(name);
    setDevice(m_file);
}

void DZlibFile::setFileName(const QString &name)
{
    if (isOpen()) {
        qWarning("DZlibFile::setFileName: File (%s) is already opened",
                 qPrintable(m_file->fileName()));
        close();
    }

    m_file->setFileName(name);
    setDevice(m_file);
}

QString DZlibFile::fileName() const
{
    return m_file->fileName();
}

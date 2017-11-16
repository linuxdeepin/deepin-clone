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

#ifndef DZLIBFILE_H
#define DZLIBFILE_H

#include "dzlibiodevice.h"

#include <QFile>

class DZlibFile : public DZlibIODevice
{
    Q_OBJECT
public:
    DZlibFile(const QString &name);
    explicit DZlibFile(QObject *parent = 0);
    DZlibFile(const QString &name, QObject *parent);

    void setFileName(const QString &name);
    QString fileName() const;

private:
    QFile *m_file;
};

#endif // DZLIBFILE_H

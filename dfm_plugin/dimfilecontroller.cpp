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
#include "dimfilecontroller.h"
#include "dimfileinfo.h"

#include <dfmevent.h>
#include <dfileinfo.h>

#include <QFileInfo>
#include <QDir>
#include <QDebug>

const QList<DAbstractFileInfoPointer> DIMFileController::getChildren(const QSharedPointer<DFMGetChildrensEvent> &event) const
{
    QList<DAbstractFileInfoPointer> list;
    const QString &base_dim = event->url().toString();

    for (const QString &name : QDir(base_dim).entryList(event->nameFilters(), event->filters())) {
        list << DAbstractFileInfoPointer(new DIMFileInfo(event->url().path() + QDir::separator() + name));
    }

    return list;
}

const DAbstractFileInfoPointer DIMFileController::createFileInfo(const QSharedPointer<DFMCreateFileInfoEvent> &event) const
{
    return DAbstractFileInfoPointer(new DIMFileInfo(event->url().path()));
}

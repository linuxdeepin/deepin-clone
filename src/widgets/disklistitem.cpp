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

#include "disklistitem.h"

#include <QIcon>

DiskListItem::DiskListItem(DDiskInfo info, QWidget *parent)
    : UtilityListDeviceItem(parent)
    , m_info(info)
{
    setTitle(info.model());
    setMessage(info.name());
    setSizeInfo(info.usedSize(), info.totalSize());

    QString icon_name;

    if (info.isRemoveable()) {
        if (info.transport().isEmpty())
            icon_name = "drive-removable-media";
        else
            icon_name = "drive-removable-media-" + info.transport();
    } else {
        if (info.transport().isEmpty())
            icon_name = "drive-harddisk";
        else
            icon_name = "drive-harddisk-" + info.transport();
    }

    setIcon(QIcon::fromTheme(icon_name), QSize(48, 48));
}

DDiskInfo DiskListItem::info() const
{
    return m_info;
}

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

#ifndef DDEVICEPARTINFO_H
#define DDEVICEPARTINFO_H

#include "../dglobal.h"
#include "dpartinfo.h"

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class DDevicePartInfoPrivate;
class DDevicePartInfo : public DPartInfo
{
public:
    explicit DDevicePartInfo();
    explicit DDevicePartInfo(const QString &filePath);

    static QList<DDevicePartInfo> localePartList();

private:
    void init(const QJsonObject &obj);

    DG_DFUNC(DDevicePartInfo)

    friend class DDeviceDiskInfoPrivate;
};

#endif // DDEVICEPARTINFO_H

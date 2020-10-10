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

#ifndef DDEVICEDISKINFO_H
#define DDEVICEDISKINFO_H

#include "../dglobal.h"
#include "ddiskinfo.h"

class DDeviceDiskInfoPrivate;
class DDeviceDiskInfo : public DDiskInfo
{
public:
    explicit DDeviceDiskInfo();
    explicit DDeviceDiskInfo(const QString &filePath);

    static QList<DDeviceDiskInfo> localeDiskList();

private:
    DG_DFUNC(DDeviceDiskInfo)
};

#endif // DDEVICEDISKINFO_H

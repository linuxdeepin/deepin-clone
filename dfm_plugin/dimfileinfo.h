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
#ifndef DIMFILEINFO_H
#define DIMFILEINFO_H

#include <dabstractfileinfo.h>
#include <dfileinfo.h>

#include "../app/src/corelib/dpartinfo.h"

class DIMFileInfo : public DFileInfo
{
public:
    explicit DIMFileInfo(const QString &path);

    bool canShare() const override;

    QString fileDisplayName() const override;
    FileType fileType() const override;
    QString mimeTypeDisplayName() const override;

    QVector<MenuAction> menuActionList(MenuType type = SingleFile) const override;

    QList<int> userColumnRoles() const override;
    QVariant userColumnDisplayName(int userColumnRole) const override;
    /// get custom column data
    QVariant userColumnData(int userColumnRole) const override;
    /// get custom column width
    int userColumnWidth(int userColumnRole, const QFontMetrics &fontMetrics) const override;
    /// user column default visible for role
    bool columnDefaultVisibleForRole(int role) const override;

    quint8 supportViewMode() const override;

    QIODevice *createIODevice() const override;

private:
    void ensureInfo() const;

    DPartInfo partInfo;
};

#endif // DIMFILEINFO_H

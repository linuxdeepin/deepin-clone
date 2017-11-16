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

#ifndef UTILITYLIST_H
#define UTILITYLIST_H

#include <QListWidget>

class UtilityListItem;
class UtilityList : public QListWidget
{
    Q_OBJECT
public:
    explicit UtilityList(QWidget *parent = 0);

    void addItem(UtilityListItem *item);
    void addItem(const QIcon &icon, const QString &title, const QString &message, const QSize &iconSize = QSize());

protected:
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
};

#endif // UTILITYLIST_H

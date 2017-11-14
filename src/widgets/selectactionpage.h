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

#ifndef SELECTACTIONPAGE_H
#define SELECTACTIONPAGE_H

#include "contentpairpage.h"

class IconLabel;
class UtilityList;
class SelectActionPage : public ContentPairPage
{
    Q_OBJECT

public:
    enum Mode {
        Disk,
        Partition
    };
    Q_ENUM(Mode)

    enum Action {
        Clone,
        Backup,
        Restore
    };
    Q_ENUM(Action)

    explicit SelectActionPage(QWidget *parent = 0);

    Mode mode() const;
    Action action() const;

    void setMode(Mode mode);
    void setAction(Action action);

    QString selectedItemTitle() const;
    QIcon selectedItemIcon() const;

private:
    void init();

    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    Mode m_mode = Disk;
    Action m_action = Clone;
    IconLabel *m_diskItem = NULL;
    IconLabel *m_partItem = NULL;
    UtilityList *m_rightContent = NULL;
};

#endif // SELECTACTIONPAGE_H

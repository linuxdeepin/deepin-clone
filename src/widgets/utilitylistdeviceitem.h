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

#ifndef UTILITYLISTDEVICEITEM_H
#define UTILITYLISTDEVICEITEM_H

#include "utilitylistitem.h"

QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

class UtilityListDeviceItem : public UtilityListItem
{
    Q_OBJECT
public:
    explicit UtilityListDeviceItem(QWidget *parent = 0);

    void setSizeInfo(qint64 used, qint64 total);

private:
    QProgressBar *m_progressBar;
    QLabel *m_sizeLabel;
};

#endif // UTILITYLISTDEVICEITEM_H

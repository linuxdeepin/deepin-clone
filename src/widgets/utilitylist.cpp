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

#include "utilitylist.h"
#include "utilitylistitem.h"

UtilityList::UtilityList(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionRectVisible(false);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void UtilityList::addItem(UtilityListItem *item)
{
    QListWidgetItem *lwi = new QListWidgetItem(this);
    QListWidget::addItem(lwi);

    QSize size = item->sizeHint();

    size.setWidth(qMin(size.width(), width()));

    lwi->setSizeHint(size);
    setItemWidget(lwi, item);

    if (count() == 1)
        lwi->setSelected(true);
}

void UtilityList::addItem(const QIcon &icon, const QString &title, const QString &message, const QSize &iconSize)
{
    UtilityListItem *item = new UtilityListItem(this);

    item->setIcon(icon, iconSize);
    item->setTitle(title);
    item->setMessage(message);

    addItem(item);
}

void UtilityList::showEvent(QShowEvent *e)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        const QWidget *widget = itemWidget(item);

        if (widget) {
            item->setSizeHint(QSize(widget->width(), widget->heightForWidth(width())));
        }
    }

    return QListWidget::showEvent(e);
}

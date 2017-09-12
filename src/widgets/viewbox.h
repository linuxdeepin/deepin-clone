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

#ifndef VIEWBOX_H
#define VIEWBOX_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QFrame;
class QBoxLayout;
QT_END_NAMESPACE

class IconLabel;
class ViewBox : public QWidget
{
    Q_OBJECT

public:
    explicit ViewBox(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    void setContent(QWidget *widget, bool full = false);

    QWidget *content() const;

private:
    void init();
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    IconLabel *m_title;
    QFrame *m_borderFrame;
    QBoxLayout *m_borderFrameLayout;
    QWidget *m_contentWidget = NULL;
    bool m_fullContent = false;
};

#endif // VIEWBOX_H

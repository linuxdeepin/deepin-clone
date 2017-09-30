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

#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QWidget>
#include <QBoxLayout>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class IconLabel : public QWidget
{
public:
    explicit IconLabel(QWidget *parent = 0, bool wordWrap = false);

    QString title() const;
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon, int extent = 0);
    void setDirection(QBoxLayout::Direction direction);

    void setChecked(bool checked);

    void setHoverBackground(const QColor &color);

private:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

    QLabel *m_icon;
    QLabel *m_label;

    bool m_isChecked = false;
    QColor m_background;
};

#endif // ICONLABEL_H

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

#include "iconlabel.h"

#include <QLabel>
#include <QIcon>
#include <QPainter>

IconLabel::IconLabel(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);
    m_icon->hide();
    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    m_label->hide();

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addWidget(m_label, 0, Qt::AlignHCenter);
    layout->addStretch();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QString IconLabel::title() const
{
    return m_label->text();
}

void IconLabel::setTitle(const QString &title)
{
    m_label->setText(title);
    m_label->setVisible(!title.isEmpty());
}

void IconLabel::setIcon(const QIcon &icon, int extent)
{
    if (extent <= 0) {
        QBoxLayout::Direction d = static_cast<QBoxLayout*>(layout())->direction();

        if (d == QBoxLayout::LeftToRight || d == QBoxLayout::RightToLeft)
            extent = m_label->sizeHint().height();
        else
            extent = 50;
    }

    m_icon->setPixmap(icon.pixmap(extent));
    m_icon->setVisible(!icon.isNull());
}

void IconLabel::setDirection(QBoxLayout::Direction direction)
{
    static_cast<QBoxLayout*>(layout())->setDirection(direction);
}

void IconLabel::setChecked(bool checked)
{
    if (m_isChecked == checked)
        return;

    m_isChecked = checked;

    update();
}

void IconLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!m_isChecked)
        return;

    QPainter pa(this);
    QPen pen = pa.pen();

    pen.setColor(QColor("#2ca7f8"));
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    pa.setPen(pen);
    pa.setRenderHint(QPainter::Antialiasing);
    pa.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 8, 8);
}

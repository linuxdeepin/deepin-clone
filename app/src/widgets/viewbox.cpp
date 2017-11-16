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

#include "viewbox.h"
#include "iconlabel.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

ViewBox::ViewBox(QWidget *parent)
    : QWidget(parent)
{
    init();
}

void ViewBox::setTitle(const QString &title)
{
    m_title->setTitle(title);
}

void ViewBox::setIcon(const QIcon &icon)
{
    m_title->setIcon(icon);
}

void ViewBox::setContent(QWidget *widget, bool full)
{
    if (m_contentWidget) {
        m_borderFrameLayout->removeWidget(m_contentWidget);
        m_contentWidget->deleteLater();
    }

    m_contentWidget = widget;
    m_fullContent = full;
    m_borderFrameLayout->addWidget(widget, 0, Qt::AlignLeft | Qt::AlignTop);

    if (full) {
        m_contentWidget->setFixedSize((m_borderFrame->rect() - m_borderFrameLayout->contentsMargins()).size());
    }
}

QWidget *ViewBox::content() const
{
    return m_contentWidget;
}

void ViewBox::init()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_title = new IconLabel(this);
    m_borderFrame = new QFrame(this);
    m_borderFrame->setObjectName("BorderFrame");

    layout->addWidget(m_title);
    layout->addWidget(m_borderFrame);

    m_borderFrameLayout = new QVBoxLayout(m_borderFrame);
    m_borderFrameLayout->setContentsMargins(0, 0, 0, 0);
    m_borderFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ViewBox::resizeEvent(QResizeEvent *e)
{
    if (m_contentWidget && m_fullContent)
        m_contentWidget->setFixedSize((m_borderFrame->rect() - m_borderFrameLayout->contentsMargins()).size());

    return QWidget::resizeEvent(e);
}

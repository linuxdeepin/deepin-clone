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

#include "contentpairpage.h"
#include "viewbox.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QDebug>

ContentPairPage::ContentPairPage(QWidget *parent)
    : QWidget(parent)
{
    init();
}

void ContentPairPage::setLeftTitle(const QString &title)
{
    m_leftBox->setTitle(title);
}

void ContentPairPage::setRightTitle(const QString &title)
{
    m_rightBox->setTitle(title);
}

void ContentPairPage::setLeftContent(QWidget *widget, bool full)
{
    m_leftBox->setContent(widget, full);
}

void ContentPairPage::setRightContent(QWidget *widget, bool full)
{
    m_rightBox->setContent(widget, full);
}

QWidget *ContentPairPage::leftContent() const
{
    return m_leftBox->content();
}

QWidget *ContentPairPage::rightContent() const
{
    return m_rightBox->content();
}

void ContentPairPage::init()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_leftBox = new ViewBox(this);
    m_rightBox = new ViewBox(this);

    layout->addSpacing(20);
    layout->addWidget(m_leftBox);
    layout->addSpacing(20);
    layout->addWidget(m_rightBox);
    layout->addSpacing(20);

    m_leftBox->setIcon(QIcon(":/icons/1.svg"));
    m_leftBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_rightBox->setIcon(QIcon(":/icons/2.svg"));
    m_rightBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ContentPairPage::resizeEvent(QResizeEvent *event)
{
    const QMargins &content_margins = static_cast<QBoxLayout*>(layout())->contentsMargins();
    int valid_width = width() - 60 - content_margins.left() - content_margins.right();

    m_leftBox->setFixedWidth(valid_width / 2);
    m_rightBox->setFixedWidth(valid_width / 2);

    return QWidget::resizeEvent(event);
}

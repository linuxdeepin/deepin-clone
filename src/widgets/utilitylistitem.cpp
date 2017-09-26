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

#include "utilitylistitem.h"

#include <dseparatorhorizontal.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QDebug>

DWIDGET_USE_NAMESPACE

class RightContentLabel : public QWidget
{
public:
    explicit RightContentLabel(const QString &title, const QString &message, QWidget *parent = 0);
    explicit RightContentLabel(QWidget *parent = 0);

private:
    QLabel *titleLabel;
    QLabel *messageLabel;

    friend class UtilityListItem;
};

RightContentLabel::RightContentLabel(const QString &title, const QString &message, QWidget *parent)
    : QWidget(parent)
{
    titleLabel->setText(title);
    messageLabel->setText(message);
}

RightContentLabel::RightContentLabel(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    titleLabel = new QLabel(this);
    titleLabel->setObjectName("RightContentLabel_Title");

    messageLabel = new QLabel(this);
    messageLabel->setObjectName("RightContentLabel_Message");

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
}

UtilityListItem::UtilityListItem(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    QVBoxLayout *content_layout = new QVBoxLayout();

    m_layout = new QHBoxLayout();
    m_label = new RightContentLabel(this);
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_layout->addWidget(m_label);

    m_bottomSeparator = new DSeparatorHorizontal(this);

    main_layout->addWidget(m_icon);
    main_layout->addLayout(content_layout);

    content_layout->addLayout(m_layout);
    content_layout->addStretch();
    content_layout->addWidget(m_bottomSeparator);
}

void UtilityListItem::setTitle(const QString &title)
{
    m_label->titleLabel->setText(title);
}

void UtilityListItem::setMessage(const QString &message)
{
    m_label->messageLabel->setText(message);
}

QString UtilityListItem::title() const
{
    return m_label->titleLabel->text();
}

void UtilityListItem::setIcon(const QIcon &icon, const QSize &size)
{
    m_iconFile = icon;

    if (size.isValid())
        m_icon->setPixmap(icon.pixmap(size));
    else
        m_icon->setPixmap(icon.pixmap(height()));
}

QIcon UtilityListItem::icon() const
{
    return m_iconFile;
}

void UtilityListItem::addWidget(QWidget *w, int stretch, Qt::Alignment alignment)
{
    m_layout->addWidget(w, stretch, alignment);
}

void UtilityListItem::enterEvent(QEvent *event)
{
    m_bottomSeparator->hide();

    return QWidget::enterEvent(event);
}

void UtilityListItem::leaveEvent(QEvent *event)
{
    m_bottomSeparator->show();

    return QWidget::leaveEvent(event);
}

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

#include "endpage.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QDebug>

EndPage::EndPage(Mode mode, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *icon = new QLabel(this);

    if (mode == Success)
        icon->setPixmap(QIcon(":/icons/success.svg").pixmap(128));
    else if (mode == Fail)
        icon->setPixmap(QIcon(":/icons/fail.svg").pixmap(128));
    else
        icon->setPixmap(QIcon(":/icons/warning.svg").pixmap(128));

    m_title = new QLabel(this);
    m_message = new QLabel(this);
    m_message->setObjectName("MessageLabel");
    m_message->setWordWrap(true);
    m_message->setAlignment(Qt::AlignHCenter);

    layout->addStretch();
    layout->addWidget(icon, 0, Qt::AlignCenter);
    layout->addWidget(m_title, 0, Qt::AlignCenter);
    layout->addWidget(m_message, 0, Qt::AlignCenter);
    layout->addStretch();
}

void EndPage::setTitle(const QString &title)
{
    m_title->setText(title);
}

void EndPage::setMessage(const QString &message)
{
    m_message->setText(message);
}

void EndPage::resizeEvent(QResizeEvent *e)
{
    int width = this->width() * 0.606;

    m_message->setFixedSize(width, m_message->heightForWidth(width));

    return QWidget::resizeEvent(e);
}

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

#ifndef UTILITYLISTITEM_H
#define UTILITYLISTITEM_H

#include <QWidget>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

class RightContentLabel;
class UtilityListItem : public QWidget
{
    Q_OBJECT
public:
    explicit UtilityListItem(QWidget *parent);

    void setTitle(const QString &title);
    void setMessage(const QString &message);

    QString title() const;

    void setIcon(const QIcon &icon, const QSize &size = QSize());
    QIcon icon() const;

    void addWidget(QWidget *w, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());

private:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

    QIcon m_iconFile;
    QLabel *m_icon;
    RightContentLabel *m_label;
    QHBoxLayout *m_layout;
    QWidget *m_spaceSeparator;
    QWidget *m_bottomSeparator;
};

#endif // UTILITYLISTITEM_H

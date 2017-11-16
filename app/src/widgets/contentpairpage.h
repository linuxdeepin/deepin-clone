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

#ifndef CONTENTPAIRPAGE_H
#define CONTENTPAIRPAGE_H

#include <QWidget>

class ViewBox;
class ContentPairPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContentPairPage(QWidget *parent = 0);

    void setLeftTitle(const QString &title);
    void setRightTitle(const QString &title);

    void setLeftContent(QWidget *widget, bool full = false);
    void setRightContent(QWidget *widget, bool full = false);

    QWidget *leftContent() const;
    QWidget *rightContent() const;

private:
    void init();
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    ViewBox *m_leftBox;
    ViewBox *m_rightBox;
};

#endif // CONTENTPAIRPAGE_H

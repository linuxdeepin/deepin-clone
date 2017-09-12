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

#ifndef ENDPAGE_H
#define ENDPAGE_H

#include <QWidget>
#include <QLabel>

class EndPage : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        Success,
        Warning,
        Fail
    };

    explicit EndPage(Mode mode, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setMessage(const QString &message);

private:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    QLabel *m_title;
    QLabel *m_message;
};

#endif // ENDPAGE_H

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

#ifndef SELECTFILEPAGE_H
#define SELECTFILEPAGE_H

#include "contentpairpage.h"
#include "selectactionpage.h"

class SelectFilePage : public ContentPairPage
{
    Q_OBJECT
public:
    explicit SelectFilePage(SelectActionPage::Mode mode, SelectActionPage::Action action, QWidget *parent = 0);

    QString source() const;
    QString target() const;

    void setSource(const QString &source);
    void setTarget(const QString &target);

signals:
    void sourceChanged();
    void targetChanged();

private:
    void onSourceChanged();
    void hideItemForFile(const QString &filePath, UtilityList *list);
    void selectItemForFile(const QString &filePath, UtilityList *list);
};

#endif // SELECTFILEPAGE_H

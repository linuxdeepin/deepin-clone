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

#ifndef WORKINGPAGE_H
#define WORKINGPAGE_H

#include <QWidget>

#include <DWaterProgress>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class CloneJob;
class WorkingPage : public QWidget
{
    Q_OBJECT
public:
    explicit WorkingPage(const QString &from, const QString &to, QWidget *parent = 0);

    bool isError() const;
    QString errorString() const;

    void cancel() const;

signals:
    void finished();

private:
    CloneJob *m_job;

    DWaterProgress *m_progress;
    QLabel *m_writtenSizeLabel;
    QLabel *m_timeRemainingLabel;
};

#endif // WORKINGPAGE_H

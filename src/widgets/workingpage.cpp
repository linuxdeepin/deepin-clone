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

#include "workingpage.h"
#include "ddiskinfo.h"
#include "helper.h"
#include "clonejob.h"

#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

static QString secondsToString(int seconds)
{
    int days = seconds / 86400;

    seconds = seconds % 86400;

    int hours = seconds / 3600;

    seconds = seconds % 3600;

    int minutes = seconds / 60;

    seconds = seconds % 60;

    if (days > 0)
        return QObject::tr("%1 days %2 hours %3 mimutes").arg(days).arg(hours).arg(minutes + 1);

    if (hours > 0)
        return QObject::tr("%1 hours %2 mimutes").arg(hours).arg(minutes + 1);

    if (minutes > 0)
        return QObject::tr("%1 mimutes").arg(minutes + 1);

    return QObject::tr("%1 seconds").arg(seconds);
}

WorkingPage::WorkingPage(const QString &from, const QString &to, QWidget *parent)
    : QWidget(parent)
{
    DDiskInfo from_info = DDiskInfo::getInfo(from);
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_progress = new DWaterProgress(this);

    qint64 total_readable_data_size = 0;

    if (from_info)
        total_readable_data_size = from_info.totalReadableDataSize();

    QLabel *tip_label = new QLabel(tr("任务正在进行，请稍后..."), this);
    QLabel *totalSize_label = new QLabel(tr("预计数据总大小：%1").arg(Helper::sizeDisplay(total_readable_data_size)), this);

    m_writtenSizeLabel = new QLabel(this);
    m_timeRemainingLabel = new QLabel(this);

    layout->addStretch();
    layout->addWidget(m_progress, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(totalSize_label, 0, Qt::AlignHCenter);
    layout->addWidget(m_writtenSizeLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_timeRemainingLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    m_job = new CloneJob(this);

    connect(m_job, &CloneJob::progressChanged, this, [this, total_readable_data_size] (qreal progress) {
        m_progress->setValue(progress * 100);
        m_writtenSizeLabel->setText(tr("已写入大小：%1").arg(Helper::sizeDisplay(total_readable_data_size * progress)));
        m_timeRemainingLabel->setText(tr("预计剩余时间： %1").arg(secondsToString(m_job->estimateTime())));
    });
    connect(m_job, &CloneJob::finished, this, &WorkingPage::finished);

    m_job->start(from, to);
    m_progress->start();
}

bool WorkingPage::isError() const
{
    return !m_job->errorString().isEmpty();
}

QString WorkingPage::errorString() const
{
    return m_job->errorString();
}

void WorkingPage::cancel() const
{
    m_job->abort();
}
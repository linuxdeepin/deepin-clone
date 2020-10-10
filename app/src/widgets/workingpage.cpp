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
#include "../corelib/ddiskinfo.h"
#include "../corelib/helper.h"
#include "../corelib/clonejob.h"

#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

WorkingPage::WorkingPage(const QString &from, const QString &to, QWidget *parent)
    : QWidget(parent)
{
    DDiskInfo from_info = DDiskInfo::getInfo(from);
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_progress = new DWaterProgress(this);

    qint64 total_readable_data_size = 0;

    if (from_info)
        total_readable_data_size = from_info.totalReadableDataSize();

    QLabel *tip_label = new QLabel(tr("Task is ongoing, please wait......"), this);

    m_writtenSizeLabel = new QLabel(this);
    m_timeRemainingLabel = new QLabel(this);

    layout->addStretch();
    layout->addWidget(m_progress, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(m_writtenSizeLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_timeRemainingLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    m_job = new CloneJob(this);

    connect(m_job, &CloneJob::progressChanged, this, [this, total_readable_data_size] (qreal progress) {
        m_progress->setValue(progress * 100);
        m_writtenSizeLabel->setText(tr("Progress: %1/%2").arg(Helper::sizeDisplay(total_readable_data_size * progress)).arg(Helper::sizeDisplay(total_readable_data_size)));
        m_timeRemainingLabel->setText(tr("Time remaining: %1").arg(Helper::secondsToString(m_job->estimateTime())));
    });
    connect(m_job, &CloneJob::finished, this, &WorkingPage::finished);
    connect(m_job, &CloneJob::statusChanged, this, [this, tip_label] (CloneJob::Status s) {
        if (s == CloneJob::Fix_Boot) {
            tip_label->setText(tr("Repairing system boot, please wait......"));
            m_writtenSizeLabel->clear();
            m_timeRemainingLabel->clear();
        }
    });

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

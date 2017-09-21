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

#include "clonejob.h"
#include "ddiskinfo.h"
#include "ddevicediskinfo.h"
#include "dfilediskinfo.h"
#include "helper.h"

#include <QDir>
#include <QElapsedTimer>

#include <functional>

CloneJob::CloneJob(QObject *parent)
    : QThread(parent)
    , m_status(Stoped)
{
    connect(this, &QThread::finished, this, [this] {
        setStatus(Stoped);

        if (!m_abort)
            emit finished();
    });
}

CloneJob::~CloneJob()
{
    if (isRunning()) {
        m_abort = true;
        quit();
        wait();
    }
}

bool CloneJob::start(const QString &from, const QString &to)
{
    if (isRunning())
        return false;

    m_abort = false;
    m_from = from;
    m_to = to;
    m_errorString.clear();
    m_progress = 0;
    m_estimateTime = 0;

    QThread::start();

    return true;
}

void CloneJob::abort()
{
    m_abort = true;
}

CloneJob::Status CloneJob::status() const
{
    return m_status;
}

qreal CloneJob::progress() const
{
    return m_progress;
}

int CloneJob::estimateTime() const
{
    return m_estimateTime;
}

QString CloneJob::errorString() const
{
    return m_errorString;
}

typedef std::function<bool(qint64 accomplishBytes, int)> PipeNotifyFunction;

static bool diskInfoPipe(DDiskInfo &from, DDiskInfo &to, DDiskInfo::DataScope scope,
                         int fromIndex = 0, int toIndex = 0, QString *error = 0, PipeNotifyFunction *notify = 0)
{
    bool ok = false;
    char block[Global::bufferSize];
    QElapsedTimer elapsedTimer;
    int speed = 10000000;
    qint64 total_size = 0;

    if (!from.beginScope(scope, DDiskInfo::Read, fromIndex)) {
        if (error)
            *error = from.errorString();

        dCDebug("BeginScope failed, scope: %d, index: %d, mode: Read", scope, fromIndex);

        goto exit;
    }

    if (!to.beginScope(scope, DDiskInfo::Write, toIndex)) {
        if (error)
            *error = to.errorString();

        dCDebug("BeginScope failed, scope: %d, index: %d, mode: Write", scope, toIndex);

        goto exit;
    }

    elapsedTimer.start();

    while (!from.atEnd()) {
        qint64 read_size = from.read(block, Global::bufferSize);

        if (read_size <= 0) {
            if (error)
                *error = QObject::tr("Read data from device: %1 is failed, %2").arg(from.filePath()).arg(from.errorString());

            goto exit;
        }

        qint64 write_size = to.write(block, read_size);

        if (write_size < read_size) {
            if (error)
                *error = QObject::tr("Write data to device: %1 is failed, hope size: %2, actual size: %3, ").arg(to.filePath()).arg(read_size).arg(write_size).arg(to.errorString());

            goto exit;
        }

        if (notify)
            if (!(*notify)(write_size, speed))
                return false;

        total_size += write_size;

        if (elapsedTimer.elapsed() > 0)
            speed = total_size / (qreal)elapsedTimer.elapsed() * 1000;
    }

    ok = true;

exit:

    if (!from.endScope()) {
        if (error)
            *error = from.errorString();

        ok = false;
    }

    if (!to.endScope()) {
        if (error)
            *error = to.errorString();

        ok = false;
    }

    return ok;
}

void CloneJob::run()
{
    setStatus(Started);

    dCDebug("Clone job start, source: %s, target: %s", qPrintable(m_from), qPrintable(m_to));

    if (!QFile::exists(m_from)) {
        setErrorString(QObject::tr("%1 not found").arg(m_from));

        return;
    }

    if (Helper::isBlockSpecialFile(m_from)) {
        dCDebug("Refresh device: %s", qPrintable(m_from));

        Helper::refreshSystemPartList(m_from);
    }

    DDiskInfo from_info = DDiskInfo::getInfo(m_from);

    if (!from_info) {
        setErrorString(QObject::tr("%1 is invalid file").arg(m_from));

        return;
    }

    if (Helper::isBlockSpecialFile(m_to)) {
        dCDebug("Refresh device: %s", qPrintable(m_to));

        Helper::refreshSystemPartList(m_to);

        if (Helper::isDiskDevice(m_from) != Helper::isDiskDevice(m_to)) {
            if (Helper::isDiskDevice(m_from) && from_info.hasScope(DDiskInfo::PartitionTable)) {
                setErrorString(tr("Hard disk devices can only be cloned to the hard disk"));

                return;
            }
        }
    } else if (Global::isOverride) {
        QFile file(m_to);

        if (file.open(QIODevice::WriteOnly))
            file.close();
    }

    DDiskInfo to_info = DDiskInfo::getInfo(m_to);

    if (!to_info) {
        setErrorString(QObject::tr("%1 is invalid file").arg(m_to));

        return;
    }

    if (to_info.totalSize() < from_info.maxReadableDataSize()) {
        setErrorString(QObject::tr("Device %1 must be larger than %2 of device %3").arg(m_to).arg(Helper::sizeDisplay(from_info.maxReadableDataSize())).arg(m_from));

        return;
    }

    qint64 from_info_total_data_size = from_info.totalReadableDataSize();
    qint64 have_been_written = 0;

    dCDebug("The total amount of data to be backed up: %s", qPrintable(Helper::sizeDisplay(from_info_total_data_size)));

    if (to_info.totalWritableDataSize() < from_info_total_data_size) {
        if (!to_info.setTotalWritableDataSize(from_info_total_data_size)) {
            setErrorString(QObject::tr("The writeable size of device %1 must be greater than the readable size of device %1").arg(m_to).arg(m_from));

            return;
        }
    }

    qint8 progress = 0;

    PipeNotifyFunction print_fun = [from_info_total_data_size, &have_been_written, &progress, this] (qint64 accomplishBytes, int speed) {
        if (m_abort)
            return false;

        have_been_written += accomplishBytes;

        if (qFuzzyCompare(m_progress, 0.99))
            return true;

        m_progress = ((have_been_written / 1000000.0) / (from_info_total_data_size  / 1000000.0));
        m_progress = qMin(m_progress, 0.99);
        m_estimateTime = from_info_total_data_size / (qreal)speed * (1 - m_progress);

        if (Global::isTUIMode) {
            printf("\033[A");
            fflush(stdout);
            printf("----%lld bytes of data have been written, total progress: %f----", have_been_written, m_progress * 100);
        } else if (progress != (int)(m_progress * 100)) {
            progress = m_progress * 100;
            dCDebug("----%lld bytes of data have been written, total progress: %d----", have_been_written, progress);
        }

        emit progressChanged(m_progress);

        return true;
    };

    auto call_disk_pipe = [&print_fun, this, &from_info, &to_info] (DDiskInfo::DataScope scope, int fromIndex = 0, int toIndex = 0) {
        QString error;

        printf("\n");

        if (!diskInfoPipe(from_info, to_info, scope, fromIndex, toIndex, &error, &print_fun)) {
            setErrorString(error);
            printf("\n");

            return false;
        }

        printf("\n");

        return true;
    };

    if (from_info.hasScope(DDiskInfo::Headgear)) {
        setStatus(Clone_Headgear);

        dCDebug("begin clone headgear......................\n");

        if (!call_disk_pipe(DDiskInfo::Headgear)) {
            dCDebug("failed!!!");

            return;
        }
    }

    if (from_info.hasScope(DDiskInfo::PartitionTable)) {
        setStatus(Clone_PartitionTable);

        dCDebug("begin clone partition table......................\n");

        if (!call_disk_pipe(DDiskInfo::PartitionTable)) {
            dCDebug("failed!!!");

            return;
        }
    }

    if (from_info.hasScope(DDiskInfo::Partition)) {
        setStatus(Clone_Partition);

        if (!from_info.hasScope(DDiskInfo::PartitionTable)) {
            dCDebug("begin clone device\n");

            if (!call_disk_pipe(DDiskInfo::Partition, 0, -1)) {
                dCDebug("failed!!!");

                return;
            }
        } else {
            int partition_count = from_info.childrenPartList().count();

            for (int i = 0; i < partition_count; ++i) {
                dCDebug("begin clone partition, index: %d......................\n", i);

                if (!call_disk_pipe(DDiskInfo::Partition, i, i)) {
                    dCDebug("failed!!!");

                    return;
                }
            }
        }
    }

    if (from_info.hasScope(DDiskInfo::JsonInfo) && to_info.hasScope(DDiskInfo::JsonInfo, DDiskInfo::Write)) {
        setStatus(Save_Info);

        dCDebug("begin clone json info\n");

        if (!call_disk_pipe(DDiskInfo::JsonInfo)) {
            dCDebug("failed!!!");

            return;
        }
    }

    m_estimateTime = 0;

    if (!m_abort) {
        emit progressChanged(1.0);
        dCDebug("clone finished!");
    }
}

void CloneJob::setStatus(CloneJob::Status s)
{
    if (s == m_status)
        return;

    m_status = s;

    emit statusChanged(s);
}

void CloneJob::setErrorString(const QString &error)
{
    m_errorString = error;

    dCError(error);

    emit failed(error);
}

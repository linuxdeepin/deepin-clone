#include "clonejob.h"
#include "ddiskinfo.h"
#include "ddevicediskinfo.h"
#include "dfilediskinfo.h"
#include "helper.h"

#include <QDir>

#include <functional>

CloneJob::CloneJob(QObject *parent)
    : QThread(parent)
    , m_status(Stoped)
{
    connect(this, &CloneJob::finished, this, [this] {
        setStatus(Stoped);
    });
}

bool CloneJob::start(const QString &from, const QString &to)
{
    if (isRunning())
        return false;

    m_from = from;
    m_to = to;
    m_errorString.clear();

    QThread::start();

    return true;
}

CloneJob::Status CloneJob::status() const
{
    return m_status;
}

QString CloneJob::errorString() const
{
    return m_errorString;
}

typedef std::function<void(qint64 accomplishBytes)> PipeNotifyFunction;

static bool diskInfoPipe(DDiskInfo &from, DDiskInfo &to, DDiskInfo::DataScope scope, int index = 0, QString *error = 0, PipeNotifyFunction *notify = 0)
{
    bool ok = false;

    char block[Global::bufferSize];

    if (!from.beginScope(scope, DDiskInfo::Read, index)) {
        if (error)
            *error = from.errorString();

        goto exit;
    }

    if (!to.beginScope(scope, DDiskInfo::Write, index)) {
        if (error)
            *error = to.errorString();

        goto exit;
    }

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
            (*notify)(write_size);
    }

    ok = true;

exit:

    from.endScope();
    to.endScope();

    return ok;
}

void CloneJob::run()
{
    setStatus(Started);

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
    }

    DDiskInfo to_info = DDiskInfo::getInfo(m_to);

    if (!to_info) {
        setErrorString(QObject::tr("%1 is invalid file").arg(m_to));

        return;
    }

    if (to_info.totalSize() < from_info.maxReadableDataSize()) {
        setErrorString(QObject::tr("Device %1 must be larger than %2 of device %3").arg(m_to).arg(from_info.maxReadableDataSize()).arg(m_from));

        return;
    }

    qint64 from_info_total_data_size = from_info.totalReadableDataSize();
    qint64 have_been_written = 0;

    dCDebug("The total amount of data to be backed up: %lld", from_info_total_data_size);

    if (to_info.totalWritableDataSize() < from_info_total_data_size) {
        if (!to_info.setTotalWritableDataSize(from_info_total_data_size)) {
            setErrorString(QObject::tr("The writeable size of device %s must be greater than the readable size of device %s").arg(m_to).arg(m_from));

            return;
        }
    }

    PipeNotifyFunction print_fun = [from_info_total_data_size, &have_been_written] (qint64 accomplishBytes) {
        printf("\033[A");
        fflush(stdout);

        have_been_written += accomplishBytes;

        qreal progress = ((qreal)have_been_written / from_info_total_data_size) * 100;

        progress = qMin(progress, 100.0);

        dCDebug("----%lld bytes of data have been written, total progress: %f----", have_been_written, progress);
    };

    auto call_disk_pipe = [&print_fun, this, &from_info, &to_info] (DDiskInfo::DataScope scope, int index = 0) {
        QString error;

        if (!diskInfoPipe(from_info, to_info, scope, index, &error, &print_fun)) {
            setErrorString(error);

            return false;
        }

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

        int partition_count = from_info.childrenPartList().count();

        for (int i = 0; i < partition_count; ++i) {
            dCDebug("begin clone partition, index: %d......................\n", i);

            if (!call_disk_pipe(DDiskInfo::Partition, i)) {
                dCDebug("failed!!!");

                return;
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

    dCDebug("clone finished!");
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

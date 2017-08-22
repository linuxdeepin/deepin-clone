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

    QThread::start();

    return true;
}

CloneJob::Status CloneJob::status() const
{
    return m_status;
}

typedef std::function<void(qint64 accomplishBytes)> PipeNotifyFunction;

static bool diskInfoPipe(DDiskInfo &from, DDiskInfo &to, DDiskInfo::DataScope scope, int index = 0, PipeNotifyFunction *notify = 0)
{
    bool ok = false;

    constexpr int buffer_size = bufferSize * 2;
    char block[buffer_size];

    if (!from.beginScope(scope, DDiskInfo::Read, index)) {
        dCError("beginScope failed! device: %s, mode: Read, scope: %d, index: %d", qPrintable(from.filePath()), scope, index);

        goto exit;
    }

    if (!to.beginScope(scope, DDiskInfo::Write, index)) {
        dCError("beginScope failed! device: %s, mode: Write, scope: %d, index: %d", qPrintable(to.filePath()), scope, index);

        goto exit;
    }

    while (!from.atEnd()) {
        qint64 read_size = from.read(block, buffer_size);

        if (read_size <= 0) {
            dCError("read data from device: %s is failed, hope size: %lld, actual size: %lld", qPrintable(from.filePath()), buffer_size, read_size);

            goto exit;
        }

        qint64 write_size = to.write(block, read_size);

        if (write_size < read_size) {
            dCError("write data to device: %s is failed, hope size: %lld, actual size: %lld", qPrintable(to.filePath()), read_size, write_size);

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
        dCError("%s not found", qPrintable(m_from));

        return;
    }

    if (Helper::isBlockSpecialFile(m_from)) {
        dCDebug("refresh device: %s", qPrintable(m_from));

        Helper::refreshSystemPartList(m_from);
    }

    DDiskInfo from_info = DDiskInfo::getInfo(m_from);

    if (!from_info) {
        dCError("%s is invalid file.", qPrintable(m_from));

        return;
    }

    if (Helper::isBlockSpecialFile(m_to)) {
        dCDebug("refresh device: %s", qPrintable(m_to));

        Helper::refreshSystemPartList(m_to);
    }

    DDiskInfo to_info = DDiskInfo::getInfo(m_to);

    if (!to_info) {
        dCError("%s is invalid file.", qPrintable(m_to));

        return;
    }

    if (to_info.totalSize() < from_info.maxReadableDataSize()) {
        dCError("device %s must be larger than %lld of device %s", qPrintable(m_to), from_info.maxReadableDataSize(), qPrintable(m_from));

        return;
    }

    qint64 from_info_total_data_size = from_info.totalReadableDataSize();
    qint64 have_been_written = 0;

    dCDebug("The total amount of data to be backed up: %lld", from_info_total_data_size);

    if (to_info.totalWritableDataSize() < from_info_total_data_size) {
        if (!to_info.setTotalWritableDataSize(from_info_total_data_size)) {
            dCError("the writeable size of device %s must be greater than the readable size of device %s", qPrintable(m_to), qPrintable(m_from));

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

    if (from_info.hasScope(DDiskInfo::Headgear)) {
        setStatus(Clone_Headgear);

        dCDebug("begin clone headgear......................\n");

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::Headgear, 0, &print_fun)) {
            dCDebug("failed!!!");

            return;
        }

        fflush(stdout);
    }

    if (from_info.hasScope(DDiskInfo::PartitionTable)) {
        setStatus(Clone_PartitionTable);

        dCDebug("begin clone partition table......................\n");

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::PartitionTable, 0, &print_fun)) {
            dCDebug("failed!!!");

            return;
        }

        fflush(stdout);
    }

    if (from_info.hasScope(DDiskInfo::Partition)) {
        setStatus(Clone_Partition);

        int partition_count = from_info.childrenPartList().count();

        for (int i = 0; i < partition_count; ++i) {
            dCDebug("begin clone partition, index: %d......................\n", i);

            if (!diskInfoPipe(from_info, to_info, DDiskInfo::Partition, i, &print_fun)) {
                dCDebug("failed!!!");

                return;
            }

            fflush(stdout);
        }
    }

    if (from_info.hasScope(DDiskInfo::JsonInfo) && to_info.hasScope(DDiskInfo::JsonInfo, DDiskInfo::Write)) {
        setStatus(Save_Info);

        dCDebug("begin clone json info\n");

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::JsonInfo, 0, &print_fun)) {
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

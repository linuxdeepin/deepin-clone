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

static DDiskInfo getInfo(const QString &file)
{
    DDiskInfo info;

    if (Helper::isBlockSpecialFile(file)) {
        info = DDeviceDiskInfo(file);
    } else if (QFileInfo(file).isFile() && QFileInfo(file).suffix() == "dgi") {
        info = DFileDiskInfo(file);
    }

    return info;
}

typedef std::function<void(quint64 accomplishBytes)> PipeNotifyFunction;

static bool diskInfoPipe(DDiskInfo &from, DDiskInfo &to, DDiskInfo::DataScope scope, int index = 0, PipeNotifyFunction *notify = 0)
{
    bool ok = false;
    quint64 accomplish = 0;

    if (!from.beginScope(scope, DDiskInfo::Read, index)) {
        dCError("beginScope failed! device: %s, mode: Read, scope: %d, index: %d", from.filePath().toUtf8().constData(), scope, index);

        goto exit;
    }

    if (!to.beginScope(scope, DDiskInfo::Write, index)) {
        dCError("beginScope failed! device: %s, mode: Write, scope: %d, index: %d", to.filePath().toUtf8().constData(), scope, index);

        goto exit;
    }


    while (!from.atEnd()) {
        char block[2048];
        qint64 read_size = from.read(block, 2048);

        if (read_size <= 0) {
            dCError("read data from device: %s is failed, hope size: %lld, actual size: %lld", from.filePath().toUtf8().constData(), 2048, read_size);

            goto exit;
        }

        qint64 write_size = to.write(block, read_size);

        if (write_size < read_size) {
            dCError("write data to device: %s is failed, hope size: %lld, actual size: %lld", to.filePath().toUtf8().constData(), read_size, write_size);

            goto exit;
        }

        accomplish += write_size;

        if (notify)
            (*notify)(accomplish);
    }

    ok = true;

exit:

    from.endScope();
    to.endScope();

    return ok;
}

static void print_diskInfoPipe(quint64 bytes)
{
    dCDebug("%lld bytes of data have been written", bytes);
    printf("\033[A");
    fflush(stdout);
}

void CloneJob::run()
{
    setStatus(Started);

    dCDebug("refresh device: %s", m_from.toUtf8().constData());

    Helper::refreshSystemPartList(m_from);
    DDiskInfo from_info = getInfo(m_from);

    if (!from_info) {
        dCError("%s is invalid file.", m_from.toUtf8().constData());

        return;
    }

    dCDebug("refresh device: %s", m_to.toUtf8().constData());

    Helper::refreshSystemPartList(m_to);
    DDiskInfo to_info = getInfo(m_to);

    if (!to_info) {
        dCError("%s is invalid file.", m_to.toUtf8().constData());

        return;
    }

    if (to_info.totalSize() < from_info.totalSize()) {
        dCError("device %s must be larger than the size of device %s", m_to.toUtf8().constData(), m_from.toUtf8().constData());

        return;
    }

    if (to_info.totalWritableDataSize() < from_info.totalReadableDataSize()) {
        dCError("the writeable size of device %s must be greater than the readable size of device %s", m_to.toUtf8().constData(), m_from.toUtf8().constData());

        return;
    }

    PipeNotifyFunction print_fun = print_diskInfoPipe;

    if (from_info.hasScope(DDiskInfo::Headgear)) {
        setStatus(Clone_Headgear);

        dCDebug("begin clone headgear......................");

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::Headgear, 0, &print_fun)) {
            dCDebug("failed!!!");

            return;
        }

        printf("\033[B");
        fflush(stdout);
    }

    if (from_info.hasScope(DDiskInfo::PartitionTable)) {
        setStatus(Clone_PartitionTable);

        dCDebug("begin clone partition table......................");

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::PartitionTable, 0, &print_fun)) {
            dCDebug("failed!!!");

            return;
        }

        printf("\033[B");
        fflush(stdout);
    }

    if (from_info.hasScope(DDiskInfo::Partition)) {
        setStatus(Clone_Partition);

        int partition_count = from_info.childrenPartList().count();

        for (int i = 0; i < partition_count; ++i) {
            dCDebug("begin clone partition, index: %d......................", i);

            if (!diskInfoPipe(from_info, to_info, DDiskInfo::Partition, i, &print_fun)) {
                dCDebug("failed!!!");

                return;
            }

            printf("\033[B");
            fflush(stdout);
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

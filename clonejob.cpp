#include "clonejob.h"
#include "ddiskinfo.h"
#include "ddevicediskinfo.h"
#include "dfilediskinfo.h"
#include "helper.h"

#include <QDir>

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

static bool diskInfoPipe(DDiskInfo &from, DDiskInfo &to, DDiskInfo::DataScope scope, int index = 0)
{
    if (!from.beginScope(scope, DDiskInfo::Read, index)) {
        dCError("beginScope failed! device: %s", from.filePath().toUtf8().constData());

        return false;
    }

    if (!to.beginScope(scope, DDiskInfo::Write, index)) {
        dCError("beginScope failed! device: %s", to.filePath().toUtf8().constData());

        return false;
    }

    while (!from.atEnd()) {
        char block[2048];
        qint64 read_size = from.read(block, 2048);

        if (read_size <= 0) {
            dCError("read data from device: %s is failed, hope size: %lld, actual size: %lld", from.filePath().toUtf8().constData(), 2048, read_size);

            return false;
        }

        qint64 write_read_size = to.write(block, read_size);

        if (write_read_size < write_read_size) {
            dCError("write data to device: %s is failed, hope size: %lld, actual size: %lld", from.filePath().toUtf8().constData(), read_size, write_read_size);

            return false;
        }
    }

    return true;
}

void CloneJob::run()
{
    setStatus(Started);

    DDiskInfo from_info = getInfo(m_from);

    if (!from_info) {
        dCError("%s is invalid file.", m_from.toUtf8().constData());

        return;
    }

    DDiskInfo to_info = getInfo(m_to);

    if (!to_info) {
        dCError("%s is invalid file.", m_to.toUtf8().constData());

        return;
    }

    if (from_info.hasScope(DDiskInfo::Headgear)) {
        if (!to_info.hasScope(DDiskInfo::Headgear)) {

        }

        setStatus(Clone_Headgear);

        if (!diskInfoPipe(from_info, to_info, DDiskInfo::Headgear))
            return;
    }

    setStatus(Clone_PartitionTable);

    if (!diskInfoPipe(from_info, to_info, DDiskInfo::PartitionTable))
        return;

    setStatus(Clone_Partition);

    int partition_count = from_info.childrenPartList().count();

    for (int i = 0; i < partition_count; ++i) {
        if (!diskInfoPipe(from_info, to_info, DDiskInfo::Partition, i))
            break;
    }
}

void CloneJob::setStatus(CloneJob::Status s)
{
    if (s == m_status)
        return;

    m_status = s;

    emit statusChanged(s);
}

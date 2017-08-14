#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QDebug>

#include "ddevicediskinfo.h"
#include "ddevicepartinfo.h"
#include "util.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (a.arguments().count() != 3)
        return -1;

    // If true clone disk. else clone partition
    bool isDisk = false;
    bool isRestore = false;

    DDiskInfo disk_info;

    const QString from = a.arguments().at(1);
    const QString to = a.arguments().last();

    if (Util::isBlockSpecialFile(from)) {
        disk_info = DDeviceDiskInfo(from);
        isDisk = disk_info.ptType() != DDiskInfo::Unknow;
    } else if (Util::isBlockSpecialFile(to)) {
        disk_info = DDeviceDiskInfo(to);
        isRestore = true;
        isDisk = QFile::exists(from + "/partition.table");
    } else {
        return -1;
    }

    qDebug() << disk_info;

    if (isDisk)
        qDebug() << "begin" << (isRestore ? "restore" : "clone") << "disk";
    else
        qDebug() << "begin" << (isRestore ? "restore" : "clone") << "partition";

    qDebug() << "from:" << from;
    qDebug() << "to:" << to;

    if (isRestore) {
        QDir dir(from);

        if (!dir.exists()) {
            qDebug() << dir.absolutePath() << "is not exists";

            return -1;
        }

        if (isDisk) {
            if (dir.exists("fpsb")) {
                qDebug() << "begin to restore the sector before the first partition, size:" << QFileInfo(dir.absoluteFilePath("fpsb")).size();
                int code = -1;
                Util::processExec(QString("dd if=%1 of=%2").arg(dir.absoluteFilePath("fpsb")).arg(disk_info.device()), -1, &code);

                if (code != 0) {
                    qDebug() << "failed";

                    return -1;
                }
            }

            bool ok = Util::setPartitionTable(disk_info.device(), dir.absoluteFilePath("partition.table"));

            if (ok) {
                qDebug() << "restore partition table is finished";
            } else {
                qDebug() << "restore partition table is failed";

                return -1;
            }

            qDebug() << "refresh device:" << disk_info.device();
            disk_info.refresh();

            qDebug() << disk_info;
        }

        int index = 1;

        for (const DPartInfo &part : disk_info.childrenPartList()) {
            if (part.isMounted()) {
                qDebug() << part.device() << "is mounted";

                return -1;
            }

            const QString &source = dir.absoluteFilePath(QString::number(index));

            ++index;

            qDebug() << "begion restore partion:" << part.device();
            qDebug() << "from file:" << source;
            int code = Util::restorePartition(source, part);

            if (code != 0) {
                qDebug() << "failed";

                return -1;
            }
        }
    } else {
        QDir::current().mkpath(to);

        if (isDisk) {
            bool ok = Util::saveToFile(to + "/" + "partition.table", Util::getPartitionTable(disk_info.device()));

            if (ok) {
                qDebug() << "backup partition table is finished";
            } else {
                qDebug() << "backup partition table is failed";

                return -1;
            }

            const QList<DPartInfo> &info_list = disk_info.childrenPartList();

            if (Q_LIKELY(!info_list.isEmpty())) {
                quint64 first_part_start = info_list.first().sizeStart();

                if (first_part_start > 0 && first_part_start <= 2048000) {
                    qDebug() << "begin to backup the sector before the first partition";
                    int code = -1;
                    Util::processExec(QString("dd if=%1 bs=%2 count=1 of=%3").arg(disk_info.device()).arg(first_part_start).arg(to + "/fpsb"), -1, &code);

                    if (code != 0) {
                        qDebug() << "failed";

                        return -1;
                    }
                }
            }
        }

        int index = 1;

        for (const DPartInfo &part : disk_info.childrenPartList()) {
            if (part.isMounted()) {
                qDebug() << part.device() << "is mounted";

                return -1;
            }

            QString part_bak_name = to + "/" + QString::number(index);

            ++index;

            qDebug() << "begin backup partition:" << part.device();
            qDebug() << "to file:" << part_bak_name;
            int code = Util::clonePartition(part, part_bak_name);

            if (code != 0 && (part.type() == DPartInfo::Unknow || part.guidType() != DPartInfo::InvalidGUID)) {
                qDebug() << "failed";

                return -1;
            }

            if (part.type() != DPartInfo::Invalid && !Util::isPartcloneFile(part_bak_name)) {
                qDebug() << part_bak_name << "is invalid file";

                return -1;
            }
        }
    }

    qDebug() << "finished!";

    return 0;
}

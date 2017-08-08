#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QDebug>

#include "ddiskinfo.h"
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
        disk_info = DDiskInfo(from);
        isDisk = disk_info.ptType() != DDiskInfo::Unknow;
    } else if (Util::isBlockSpecialFile(to)) {
        disk_info = DDiskInfo(to);
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
            bool ok = Util::setPartitionTable(disk_info.device(), dir.absoluteFilePath("partition.table"));

            if (ok) {
                qDebug() << "restore partition table is finished";
            } else {
                qDebug() << "restore partition table is failed";

                return -1;
            }

            disk_info.refresh();

            int index = 1;

            for (const DPartInfo &part : disk_info.childrenPartList()) {
                const QString &source = dir.absoluteFilePath(QString::number(index));

                ++index;

                qDebug() << "begion restore partion:" << part.device();
                qDebug() << "from file:" << source;
                qDebug() << Util::restorePartition(source, part.device());
            }
        } else {
            const QString &source = dir.absoluteFilePath("1");

            qDebug() << "begion restore partion:" << disk_info.device();
            qDebug() << "from file:" << source;
            qDebug() << Util::restorePartition(source, disk_info.device());
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
                qDebug() << Util::clonePartition(part, part_bak_name);
            }
        } else {
            DPartInfo part(disk_info.device());

            if (part.isMounted()) {
                qDebug() << part.device() << "is mounted";

                return -1;
            }

            QString part_bak_name = to + "/1";

            qDebug() << "begin backup partition:" << part.device();
            qDebug() << Util::clonePartition(part, part_bak_name);
        }
    }

    qDebug() << "finished!";

    return 0;
}

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

#include "ddevicepartinfo.h"
#include "dpartinfo_p.h"
#include "helper.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

class DDevicePartInfoPrivate : public DPartInfoPrivate
{
public:
    DDevicePartInfoPrivate(DDevicePartInfo *qq);

    void refresh() Q_DECL_OVERRIDE;

    void init(const QJsonObject &obj);
};

DDevicePartInfoPrivate::DDevicePartInfoPrivate(DDevicePartInfo *qq)
    : DPartInfoPrivate(qq)
{

}

void DDevicePartInfoPrivate::refresh()
{
    *q = DDevicePartInfo(name);
}

void DDevicePartInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    filePath = name;
    kname = obj.value("kname").toString();
    parentDiskFilePath = obj.value("pkname").toString();
    size = obj.value("size").toString().toLongLong();
    typeName = obj.value("fstype").toString();
    type = toType(typeName);
    mountPoint = obj.value("mountpoint").toString();
    label = obj.value("label").toString();
    partLabel = obj.value("partlabel").toString();
    partType = obj.value("parttype").toString();
    guidType = DPartInfo::guidType(partType.toLatin1().toUpper());
    blockSize = obj.value("phy-sec").toInt(4096);
    readonly = obj.value("ro").toString() == "1" || obj.value("type").toString() == "rom";
    removeable = obj.value("rm").toString() == "1";
    transport = obj.value("tran").toString();
    partUUID = obj.value("partuuid").toString();

    const QString &device = name;

    if (obj.value("pkname").isNull()) {
        sizeStart = 0;
        sizeEnd = size - 1;
    } else {
        int code = Helper::processExec(QStringLiteral("partx %1 -b -P -o START,END,SECTORS").arg(device));

        if (code == 0) {
            const QByteArray &data = Helper::lastProcessStandardOutput();
            const QByteArrayList &list = data.split(' ');

            if (list.count() != 3) {
                dCError("Get partition START/END/SECTORS/SIZE info error by partx, device: %s", qPrintable(device));

                return;
            }

            qint64 start = list.first().split('"').at(1).toLongLong();
            qint64 end = list.at(1).split('"').at(1).toLongLong();
            qint64 sectors = list.at(2).split('"').at(1).toLongLong();

            Q_ASSERT(sectors > 0);

            sizeStart = start * (size / sectors);
            sizeEnd = (end + 1) * (size / sectors) - 1;
        }
    }

    if (type == DPartInfo::Invalid || type == DPartInfo::Unknow) {
        usedSize = size;
        freeSize = 0;
    } else if (!DThreadUtil::runInNewThread(&Helper::getPartitionSizeInfo, *q, &usedSize, &freeSize, &blockSize)) {
        dCError("Get partition used sieze/free size info failed, device: %s", qPrintable(device));
    }
}

DDevicePartInfo::DDevicePartInfo()
    : DPartInfo(new DDevicePartInfoPrivate(this))
{

}

DDevicePartInfo::DDevicePartInfo(const QString &name)
    : DPartInfo(new DDevicePartInfoPrivate(this))
{
    const QJsonArray &block_devices = Helper::getBlockDevices("-s " + name);

    if (!block_devices.isEmpty()) {
        const QJsonObject &obj = block_devices.first().toObject();

        d_func()->init(obj);

        const QJsonArray &children = obj.value("children").toArray();

        if (!children.isEmpty())
            d->transport = children.first().toObject().value("tran").toString();
    }
}

QList<DDevicePartInfo> DDevicePartInfo::localePartList()
{
    const QJsonArray &block_devices = Helper::getBlockDevices();

    QList<DDevicePartInfo> list;

    for (const QJsonValue &value : block_devices) {
        const QJsonObject &obj = value.toObject();

        if (Global::disableLoopDevice && obj.value("type").toString() == "loop")
            continue;

        const QString &fstype = obj.value("fstype").toString();
        const QString &transport = obj.value("tran").toString();

        if (fstype.isEmpty()) {
            QStringList children_uuids;

            for (const QJsonValue &children : obj.value("children").toArray()) {
                const QJsonObject &obj = children.toObject();

                if (children_uuids.contains(obj.value("partuuid").toString()))
                    continue;

                DDevicePartInfo info;

                info.d_func()->init(obj);
                info.d->transport = transport;
                list << info;
                children_uuids << info.partUUID();
            }
        } else {
            DDevicePartInfo info;

            info.d_func()->init(obj);
            info.d->transport = transport;
            list << info;
        }
    }

    return list;
}

void DDevicePartInfo::init(const QJsonObject &obj)
{
    d_func()->init(obj);
}
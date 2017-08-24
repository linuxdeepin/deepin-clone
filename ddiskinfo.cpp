#include "ddiskinfo.h"
#include "ddiskinfo_p.h"
#include "dpartinfo_p.h"
#include "helper.h"
#include "ddevicediskinfo.h"
#include "dfilediskinfo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QDebug>

DDiskInfoPrivate::DDiskInfoPrivate(DDiskInfo *qq)
    : q(qq)
{

}

void DDiskInfoPrivate::initFromJson(const QByteArray &json)
{
    DDiskInfo::fromJson(json, this);
}

qint64 DDiskInfoPrivate::maxReadableDataSize() const
{
    return size;
}

bool DDiskInfoPrivate::setTotalWritableDataSize(qint64 size)
{
    Q_UNUSED(size)

    return false;
}

void DDiskInfoPrivate::setErrorString(const QString &error)
{
    this->error = error;
}

QString DDiskInfoPrivate::errorString() const
{
    return error;
}

QString DDiskInfoPrivate::scopeString(DDiskInfo::DataScope scope)
{
    switch (scope) {
    case DDiskInfo::Headgear:
        return "Headgear";
    case DDiskInfo::PartitionTable:
        return "PartitionTable";
    case DDiskInfo::Partition:
        return "Partition";
    case DDiskInfo::JsonInfo:
        return "JsonInfo";
    case DDiskInfo::NullScope:
        return "NullScope";
    default:
        break;
    }

    return QString();
}

QString DDiskInfoPrivate::modeString(DDiskInfo::ScopeMode mode)
{
    if (mode == DDiskInfo::Read)
        return "Read";

    return "Write";
}

DDiskInfo::DDiskInfo()
{

}

DDiskInfo::DDiskInfo(const DDiskInfo &other)
    : d(other.d)
{
    d->q = this;
}

DDiskInfo::~DDiskInfo()
{

}

void DDiskInfo::swap(DDiskInfo &other)
{
    qSwap(d, other.d);

    d->q = this;
}

DDiskInfo::DataScope DDiskInfo::currentScope() const
{
    return d->currentScope;
}

bool DDiskInfo::hasScope(DDiskInfo::DataScope scope, ScopeMode mode) const
{
    if (scope == NullScope)
        return true;

    return d->hasScope(scope, mode);
}

bool DDiskInfo::beginScope(DDiskInfo::DataScope scope, ScopeMode mode, int index)
{
    endScope();

    d->error.clear();

    if (!d->hasScope(scope, mode)) {
        d->setErrorString(QObject::tr("Device %1 not support scope: %2 mode: %3").arg(d->scopeString(scope)).arg(d->modeString(mode)));

        return false;
    }

    d->currentScope = scope;
    d->currentMode = mode;

    return d->openDataStream(index);
}

void DDiskInfo::endScope()
{
    d->closeDataStream();
    d->currentScope = NullScope;
}

qint64 DDiskInfo::readableDataSize(DDiskInfo::DataScope scope) const
{
    return d->readableDataSize(scope);
}

qint64 DDiskInfo::totalReadableDataSize() const
{
    return d->totalReadableDataSize();
}

qint64 DDiskInfo::maxReadableDataSize() const
{
    return d->maxReadableDataSize();
}

qint64 DDiskInfo::totalWritableDataSize() const
{
    return d->totalWritableDataSize();
}

bool DDiskInfo::setTotalWritableDataSize(qint64 size)
{
    return d->setTotalWritableDataSize(size);
}

qint64 DDiskInfo::read(char *data, qint64 maxSize)
{
    return d->read(data, maxSize);
}

qint64 DDiskInfo::write(const char *data, qint64 maxSize)
{
    return d->write(data, maxSize);
}

qint64 DDiskInfo::write(const char *data)
{
    return write(data, qstrlen(data));
}

bool DDiskInfo::atEnd() const
{
    return d->atEnd();
}

QString DDiskInfo::filePath() const
{
    return d->filePath();
}

DDiskInfo &DDiskInfo::operator=(const DDiskInfo &other)
{
    d = other.d;
    return *this;
}

QString DDiskInfo::name() const
{
    return d->name;
}

QString DDiskInfo::kname() const
{
    return d->kname;
}

qint64 DDiskInfo::totalSize() const
{
    return d->size;
}

QString DDiskInfo::typeName() const
{
    return d->typeName;
}

DDiskInfo::Type DDiskInfo::type() const
{
    return d->type;
}

DDiskInfo::PTType DDiskInfo::ptType() const
{
    return d->ptType;
}

QList<DPartInfo> DDiskInfo::childrenPartList() const
{
    return d->children;
}

void DDiskInfo::refresh()
{
    d->refresh();
}

QByteArray DDiskInfo::toJson() const
{
    QJsonObject root
    {
        {"totalReadableDataSize", QString::number(totalReadableDataSize())},
        {"maxReadableDataSize", QString::number(maxReadableDataSize())},
        {"totalWritableDataSize", QString::number(totalWritableDataSize())},
        {"filePath", filePath()},
        {"name", name()},
        {"kname", kname()},
        {"totalSize", QString::number(totalSize())},
        {"typeName", typeName()},
        {"type", type()},
        {"ptTypeName", d->ptTypeName},
        {"ptType", ptType()}
    };

    QJsonArray children;

    for (const DPartInfo &part : childrenPartList())
        children.append(QJsonDocument::fromJson(part.toJson()).object());

    root.insert("childrenPartList", children);

    QJsonDocument doc(root);

    return doc.toJson();
}

QString DDiskInfo::errorString() const
{
    return d->errorString();
}

DDiskInfo DDiskInfo::getInfo(const QString &file)
{
    DDiskInfo info;

    if (Helper::isBlockSpecialFile(file)) {
        info = DDeviceDiskInfo(file);
    } else {
        QFileInfo file_info(file);

        if (file_info.suffix() != "dim") {
            return info;
        }

        if (file_info.exists()) {
            if (file_info.isFile())
                info = DFileDiskInfo(file);
        } else {
            QFile touch(file);

            if (touch.open(QIODevice::WriteOnly)) {
                touch.close();
                info = DFileDiskInfo(file);
            }
        }
    }

    return info;
}

DDiskInfo::DDiskInfo(DDiskInfoPrivate *dd)
    : d(dd)
{
    dd->q = this;
}

void DDiskInfo::fromJson(const QByteArray &json, DDiskInfoPrivate *dd)
{
    const QJsonDocument &doc = QJsonDocument::fromJson(json);
    const QJsonObject &root = doc.object();

    dd->name = root.value("name").toString();
    dd->kname = root.value("kname").toString();
    dd->size = root.value("totalSize").toString().toLongLong();
    dd->typeName = root.value("typeName").toString();
    dd->type = (Type)root.value("type").toInt();
    dd->ptTypeName = root.value("ptTypeName").toString();
    dd->ptType = (PTType)root.value("ptType").toInt();
    dd->havePartitionTable = dd->type == Disk && !dd->ptTypeName.isEmpty();

    for (const QJsonValue &v : root.value("childrenPartList").toArray()) {
        DPartInfoPrivate *part_dd = new DPartInfoPrivate(0);
        DPartInfo::fromJson(v.toObject(), part_dd);

        dd->children << DPartInfo(part_dd);
    }
}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DDiskInfo &info)
{
    QDebugStateSaver saver(deg);
    Q_UNUSED(saver)

    deg.space() << "name:" << info.name()
                << "size:" << Helper::sizeDisplay(info.totalSize());
    deg << "partitions:" << info.childrenPartList();

    return deg;
}
#endif
QT_END_NAMESPACE

#include "ddiskinfo.h"
#include "ddiskinfo_p.h"

#include <QDebug>

DDiskInfoPrivate::DDiskInfoPrivate(DDiskInfo *qq)
    : q(qq)
{

}

DDiskInfo::DDiskInfo()
{

}

DDiskInfo::DDiskInfo(const DDiskInfo &other)
    : d(other.d)
{

}

DDiskInfo::~DDiskInfo()
{

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

    d->currentScope = scope;
    d->currentMode = mode;

    return d->openDataStream(index);
}

void DDiskInfo::endScope()
{
    d->closeDataStream();
    d->currentScope = NullScope;
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

QString DDiskInfo::displaySize() const
{
    return d->sizeDisplay;
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

DDiskInfo::DDiskInfo(DDiskInfoPrivate *dd)
    : d(dd)
{

}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DDiskInfo &info)
{
    QDebugStateSaver saver(deg);
    Q_UNUSED(saver)

    deg.space() << "name:" << info.name()
                << "size:" << info.displaySize();
    deg << "partitions:" << info.childrenPartList();

    return deg;
}
#endif
QT_END_NAMESPACE

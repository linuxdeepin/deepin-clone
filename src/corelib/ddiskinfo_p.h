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

#ifndef DDISKINFO_P_H
#define DDISKINFO_P_H

#include <QSharedData>

#include "ddiskinfo.h"

class DDiskInfoPrivate : public QSharedData
{
public:
    DDiskInfoPrivate(DDiskInfo *qq);

    void initFromJson(const QByteArray &json);

    virtual QString filePath() const {return QString();}
    virtual void refresh() = 0;

    virtual bool hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode, int index) const = 0;
    virtual bool openDataStream(int index) = 0;
    virtual void closeDataStream() = 0;

    virtual qint64 readableDataSize(DDiskInfo::DataScope scope) const = 0;

    virtual qint64 totalReadableDataSize() const = 0;
    virtual qint64 maxReadableDataSize() const;
    virtual qint64 totalWritableDataSize() const = 0;
    virtual bool setTotalWritableDataSize(qint64 size);

    virtual qint64 read(char *data, qint64 maxSize) = 0;
    virtual qint64 write(const char *data, qint64 maxSize) = 0;

    virtual bool atEnd() const = 0;

    void setErrorString(const QString &error);
    virtual QString errorString() const;

    static QString scopeString(DDiskInfo::DataScope scope);
    static QString modeString(DDiskInfo::ScopeMode mode);

    DDiskInfo *q;
    DDiskInfo::DataScope currentScope = DDiskInfo::NullScope;
    DDiskInfo::ScopeMode currentMode = DDiskInfo::Read;
    QString error;

    QString model;
    QString name;
    QString kname;
    qint64 size = 0;
    QString typeName;
    DDiskInfo::Type type;
    bool havePartitionTable = true;
    QList<DPartInfo> children;
    QString ptTypeName;
    DDiskInfo::PTType ptType;

    bool readonly = false;
    bool removeable = false;
    QString transport;
    QString serial;
};

#endif // DDISKINFO_P_H

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

#ifndef DZLIBIODEVICE_H
#define DZLIBIODEVICE_H

#include <QIODevice>

class DZlibIODevice : public QIODevice
{
    Q_OBJECT

public:
    explicit DZlibIODevice(QObject *parent = 0);
    explicit DZlibIODevice(QIODevice *m_device, QObject *parent = 0);
    ~DZlibIODevice();

    void setDevice(QIODevice *device);

    bool isSequential() const Q_DECL_OVERRIDE;

    bool open(OpenMode mode) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    qint64 pos() const Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;
    bool atEnd() const Q_DECL_OVERRIDE;

    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    qint64 bytesToWrite() const Q_DECL_OVERRIDE;

    bool canReadLine() const Q_DECL_OVERRIDE;

    bool waitForReadyRead(int msecs) Q_DECL_OVERRIDE;
    bool waitForBytesWritten(int msecs) Q_DECL_OVERRIDE;

    QString errorString() const;

    static constexpr int metaDataSize();

protected:
    qint64 readData(char *data, qint64 maxlen)  Q_DECL_OVERRIDE;
    qint64 writeData(const char *data, qint64 len)  Q_DECL_OVERRIDE;

    QByteArray compress(const QByteArray &data) const;
    QByteArray uncompress(const QByteArray &data) const;

private:
    bool isReadMode() const;
    bool isWriteMode() const;
    void readNextBlock();
    bool writeToBlock();

    QIODevice *m_device;
    QByteArray m_readBuffer;
    QByteArray m_writeBuffer;
    qint64 m_currentBlock = -1;

    qint64 m_size = 0;
    qint64 m_blockCount = 0;
    qint32 m_lastBlockSize = 0;
};

#endif // DZLIBIODEVICE_H

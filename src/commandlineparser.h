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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>

class CommandLineParser
{
public:
    CommandLineParser();

    void process(const QCoreApplication &app);
    void process(const QStringList &arguments);

    QString source() const;
    QString target() const;
    QString logFile() const;

    bool isSetOverride() const;
    bool isSetDebug() const;

private:
    QCommandLineParser parser;
    QCommandLineOption o_info;
    QCommandLineOption o_dim_info;
    QCommandLineOption o_override;
    QCommandLineOption o_compress_level;
    QCommandLineOption o_buffer_size;
    QCommandLineOption o_non_ui;
    QCommandLineOption o_to_serial_url;
    QCommandLineOption o_from_serial_url;
    QCommandLineOption o_disable_check_dim;
    QCommandLineOption o_log_file;
    QCommandLineOption o_loop_device;
    QCommandLineOption o_debug_level;
};

#endif // COMMANDLINEPARSER_H

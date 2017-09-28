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

#include <QDebug>
#include <DLog>

#ifdef ENABLE_GUI
#include <DApplication>
#include <DTitlebar>
#include <DThemeManager>

#include "mainwindow.h"

DWIDGET_USE_NAMESPACE
#else
#include <QCoreApplication>
#endif

#include "helper.h"
#include "dglobal.h"
#include "clonejob.h"
#include "commandlineparser.h"

bool Global::isOverride = true;
bool Global::disableMD5CheckForDimFile = false;
bool Global::disableLoopDevice = true;
#ifdef ENABLE_GUI
bool Global::isTUIMode = false;
#else
bool Global::isTUIMode = true;
#endif

int Global::bufferSize = 1024 * 1024;
int Global::compressionLevel = 0;
int Global::debugLevel = 1;

DCORE_USE_NAMESPACE

inline static bool isTUIMode(int argc, char *argv[])
{
#ifndef ENABLE_GUI
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    return true;
#endif

    if (qEnvironmentVariableIsEmpty("DISPLAY"))
        return true;

    for (int i = 1; i < argc; ++i)
        if (argv[i] == QByteArray("--tui"))
            return true;

    return false;
}

static QString logFormat = "[%{time}{yyyy-MM-dd, HH:mm:ss.zzz}] [%{type:-7}] [%{file}=>%{function}: %{line}] %{message}\n";

int main(int argc, char *argv[])
{
    QCoreApplication *a;

    if (isTUIMode(argc, argv)) {
        Global::isTUIMode = true;

        a = new QCoreApplication(argc, argv);
    }
#ifdef ENABLE_GUI
    else {
        DApplication::loadDXcbPlugin();
        DApplication *app = new DApplication(argc, argv);

        if (!app->loadTranslator()) {
            dError("Load translator failed");
        }

        app->setApplicationDisplayName(QObject::tr("Deepin Clone"));
        app->setApplicationDescription(QObject::tr("Deepin Clone is a tool to backup and restore in deepin. "
                                                   "It supports to clone, backup and restore disk or partition and other functions."));
        app->setTheme("light");
        a = app;
    }
#endif

    a->setApplicationName("deepin-clone");
    a->setApplicationVersion("0.0.1");
    a->setOrganizationName("deepin");

    CommandLineParser parser;

    QFile arguments_file("/lib/live/mount/medium/.tmp/deepin-clone.arguments");
    QStringList arguments;

    if (arguments_file.exists()) {
        while (!arguments_file.atEnd())
            arguments.append(QString::fromUtf8(arguments_file.readLine()));

        arguments_file.close();
    } else {
        arguments = a->arguments();
    }

    parser.process(arguments);

    ConsoleAppender *consoleAppender = new ConsoleAppender;
    consoleAppender->setFormat(logFormat);

    RollingFileAppender *rollingFileAppender = new RollingFileAppender(parser.logFile());
    rollingFileAppender->setFormat(logFormat);
    rollingFileAppender->setLogFilesLimit(5);
    rollingFileAppender->setDatePattern(RollingFileAppender::DailyRollover);

    logger->registerCategoryAppender("deepin.ghost", consoleAppender);
    logger->registerCategoryAppender("deepin.ghost", rollingFileAppender);

    if (arguments_file.exists()) {
        dCDebug("Load arguments from \"%s\"", qPrintable(arguments_file.fileName()));
    }

    dCDebug("Application command line: %s", qPrintable(arguments.join(' ')));

    if (Global::debugLevel == 0) {
        QLoggingCategory::setFilterRules("deepin.ghost.debug=false");
    }

    if (Global::isTUIMode) {
        if (!parser.target().isEmpty()) {
            CloneJob job;

            QObject::connect(&job, &QThread::finished, a, &QCoreApplication::quit);

            job.start(parser.source(), parser.target());

            return a->exec();
        }
    }
#ifdef ENABLE_GUI
    else {
        if (!qApp->setSingleInstance("_deepin_clone_")) {
            dError("As well as the process is running");

            return -1;
        }

        if (!parser.isSetOverride())
            Global::isOverride = true;

        if (!parser.isSetDebug())
            Global::debugLevel = 2;

        MainWindow *window = new MainWindow;

        window->setFixedSize(860, 660);
        window->setStyleSheet(DThemeManager::instance()->getQssForWidget("main", window));
        window->setWindowIcon(QIcon::fromTheme("deepin-clone"));
        window->titlebar()->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint
                                           | Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
        window->titlebar()->setIcon(window->windowIcon().pixmap(24));
        window->titlebar()->setTitle(QString());
        window->show();

        qApp->setProductIcon(window->windowIcon().pixmap(128));

        if (!parser.source().isEmpty()) {
            window->startWithFile(parser.source(), parser.target());
        }

        QObject::connect(a, &QCoreApplication::aboutToQuit, window, &MainWindow::deleteLater);
    }
#endif

    return Global::isTUIMode ? qApp->exec() : a->exec();
}

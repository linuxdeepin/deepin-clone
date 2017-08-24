#include <QFile>
#include <QDebug>

#include <DApplication>

#include "helper.h"
#include "dglobal.h"
#include "clonejob.h"
#include "commandlineparser.h"

#include "mainwindow.h"

DWIDGET_USE_NAMESPACE

bool Global::isOverride = true;
bool Global::isTUIMode = false;

int Global::bufferSize = 1024 * 1024;
int Global::compressionLevel = 4;

static bool isTUIMode(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("DISPLAY"))
        return true;

    for (int i = 1; i < argc; ++i)
        if (argv[i] == QByteArray("--tui"))
            return true;

    return false;
}

int main(int argc, char *argv[])
{
    QCoreApplication *a;

    if (isTUIMode(argc, argv)) {
        Global::isTUIMode = true;

        a = new QCoreApplication(argc, argv);
    } else {
        DApplication::loadDXcbPlugin();
        DApplication *app = new DApplication(argc, argv);

        app->setApplicationDisplayName(QObject::tr("Deepin Clone"));
        app->setTheme("light");
        a = app;

        MainWindow *window = new MainWindow;

        window->setFixedSize(600, 400);
        window->show();
    }

    a->setApplicationName("deepin-clone");
    a->setApplicationVersion("0.0.1");
    a->setOrganizationName("deepin");

    CommandLineParser parser;

    parser.process(*a);

    if (Global::isTUIMode) {
        if (!parser.target().isEmpty()) {
            CloneJob job;

            QObject::connect(&job, &QThread::finished, a, &QCoreApplication::quit);

            if (Global::isOverride) {
                QFile file(parser.target());

                if (file.open(QIODevice::WriteOnly))
                    file.close();
            }

            job.start(parser.source(), parser.target());

            return a->exec();
        }
    }

    return Global::isTUIMode ? qApp->exec() : a->exec();
}

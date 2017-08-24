#include <QApplication>
#include <QFile>
#include <QDebug>

#include "helper.h"
#include "dglobal.h"
#include "clonejob.h"
#include "commandlineparser.h"

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
        QApplication *app = new QApplication(argc, argv);

        app->setApplicationDisplayName(QObject::tr("Deepin Clone"));
        a = app;
    }

    a->setApplicationName("deepin-clone");
    a->setApplicationVersion("0.0.1");
    a->setOrganizationName("deepin");

    CommandLineParser parser;

    parser.process(*a);

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

    return a->exec();
}

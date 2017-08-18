#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QLockFile>
#include <QDebug>

#include "ddevicediskinfo.h"
#include "ddevicepartinfo.h"
#include "helper.h"
#include "clonejob.h"

#include "dvirtualimagefileio.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (a.arguments().count() != 3)
        return -1;

    CloneJob job;

    QObject::connect(&job, &QThread::finished, &a, &QCoreApplication::quit);

    job.start(a.arguments().at(1), a.arguments().at(2));

    return a.exec();
}

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

#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (a.arguments().count() != 3)
        return -1;

    if (a.arguments().contains("-i")) {
        const DDiskInfo &info = DDiskInfo::getInfo(a.arguments().last());

        if (!info)
            return -1;

        qDebug().noquote() << info.toJson();

        return 0;
    } else if (a.arguments().contains("--dim-info")) {
        DVirtualImageFileIO io(a.arguments().last());

        if (!io.isValid())
            return -1;

        for (const QString &file : io.fileList()) {
            qDebug() << "file:" << file;
            qDebug() << "size:" << io.size(file);
            qDebug() << "start:" << io.start(file);
            qDebug() << "end:" << io.end(file);
            qDebug() << endl;
        }

        return 0;
    }

    CloneJob job;

    QObject::connect(&job, &QThread::finished, &a, &QCoreApplication::quit);

    job.start(a.arguments().at(1), a.arguments().at(2));

    return a.exec();
}

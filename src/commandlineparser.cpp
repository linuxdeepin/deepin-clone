#include "commandlineparser.h"
#include "ddiskinfo.h"
#include "dvirtualimagefileio.h"
#include "helper.h"
#include "dglobal.h"

#include <cstdio>

class MainWindow;
extern QString parseSerialUrl(const QString &url, MainWindow *window = 0);
extern QString toSerialUrl(const QString &file);

CommandLineParser::CommandLineParser()
    : o_info(QStringList() << "i" << "info")
    , o_dim_info("dim-info")
    , o_override(QStringList() << "O" << "override")
    , o_compress_level(QStringList() << "C" << "compress-level")
    , o_buffer_size(QStringList() << "B" << "buffer-size")
    , o_non_ui("tui")
    , o_to_serial_url("to-serial-url")
    , o_from_serial_url("from-serial-url")
{
    o_info.setDescription("Get the device info.");
    o_dim_info.setDescription("Get the dim file info.");
    o_override.setDescription("If the target file exists when the override.");
    o_compress_level.setDescription("Output to the dim file when the data compression level.");
    o_compress_level.setValueName("Compress Level");
    o_compress_level.setDefaultValue(QString::number(Global::compressionLevel));
    o_buffer_size.setDescription("The size of the buffer when data is transferred.");
    o_buffer_size.setValueName("Buffer Size");
    o_buffer_size.setDefaultValue(QString::number(Global::bufferSize));
    o_non_ui.setDescription("Run in TUI mode.");
    o_to_serial_url.setDescription("File path format to serial url format");
    o_to_serial_url.setValueName("Serial URL");
    o_from_serial_url.setDescription("Serial url format to file path format");
    o_from_serial_url.setValueName("File Path");

    parser.addOption(o_info);
    parser.addOption(o_dim_info);
    parser.addOption(o_override);
    parser.addOption(o_buffer_size);
    parser.addOption(o_compress_level);
    parser.addOption(o_buffer_size);
    parser.addOption(o_non_ui);
    parser.addOption(o_to_serial_url);
    parser.addOption(o_from_serial_url);
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("source", "Srouce file.", "[path/serial]");
    parser.addPositionalArgument("target", "Output file.", "[path/serial]");

    parser.setApplicationDescription(QString("e.g(path):   %1 /dev/sda ~/sda.dim\n"
                                             "             %1 /dev/sda /dev/sdb\n"
                                             "             %1 ~/sda.dim /dev/sda\n\n"
                                             "e.g(serial): %1 serial://W530B6RT ~/W530B6RT.dim\n"
                                             "             %1 serial://W530B6RT:1 serial://W530B6RT:2\n"
                                             "             %1 serial://W530B6RT.dim serial://W530B6RT:0").arg(qApp->applicationName()));
}

void CommandLineParser::process(const QCoreApplication &app)
{
    parser.process(app);

    Global::isOverride = parser.isSet(o_override);

    if (parser.isSet(o_buffer_size)) {
        bool ok = false;

        Global::bufferSize = parser.value(o_buffer_size).toInt(&ok);

        if (!ok || Global::bufferSize <= 0) {
            parser.showHelp(EXIT_FAILURE);
        }
    }

    if (parser.isSet(o_compress_level)) {
        bool ok = false;

        Global::compressionLevel = parser.value(o_compress_level).toInt(&ok);

        if (!ok) {
            parser.showHelp(EXIT_FAILURE);
        }
    }

    if (parser.isSet(o_info)) {
        if (parser.positionalArguments().isEmpty()) {
            parser.showHelp(EXIT_FAILURE);
        }

        const DDiskInfo &info = DDiskInfo::getInfo(parser.positionalArguments().first());

        if (!info) {
            fputs(qPrintable(QString("%1 is invalid file").arg(parser.positionalArguments().first())), stderr);
            ::exit(EXIT_FAILURE);
        }

        printf(info.toJson().constData());
        ::exit(EXIT_SUCCESS);
    } else if (parser.isSet(o_dim_info)) {
        if (parser.positionalArguments().isEmpty()) {
            parser.showHelp(EXIT_FAILURE);
        }

        DVirtualImageFileIO io(parser.positionalArguments().first());

        if (!io.isValid()) {
            fputs(qPrintable(QString("%1 is invalid file").arg(parser.positionalArguments().first())), stderr);
            ::exit(EXIT_FAILURE);
        }

        for (const QString &file : io.fileList()) {
            printf("File Name: %s\n", qPrintable(file));
            printf("Size: %s (%lld bytes)\n", qPrintable(Helper::sizeDisplay(io.size(file))), io.size(file));
            printf("Data Start: %lld\n", io.start(file));
            printf("Data End: %lld\n\n", io.end(file));
        }

        ::exit(EXIT_SUCCESS);
    } else if (parser.isSet(o_to_serial_url)) {
        printf(toSerialUrl(parser.value(o_to_serial_url)).toUtf8().constData());
        printf("\n");

        ::exit(EXIT_SUCCESS);
    } else if (parser.isSet(o_from_serial_url)) {
        printf(parseSerialUrl(parser.value(o_from_serial_url)).toUtf8().constData());
        printf("\n");

        ::exit(EXIT_SUCCESS);
    } else {
        if ((Global::isTUIMode || !parser.positionalArguments().isEmpty()) && parser.positionalArguments().count() != 2) {
            parser.showHelp(EXIT_FAILURE);
        }
    }
}

QString CommandLineParser::source() const
{
    if (parser.positionalArguments().isEmpty())
        return QString();

    return parser.positionalArguments().first();
}

QString CommandLineParser::target() const
{
    if (parser.positionalArguments().isEmpty())
        return QString();

    return parser.positionalArguments().last();
}

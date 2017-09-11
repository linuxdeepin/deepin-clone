#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>

class CommandLineParser
{
public:
    CommandLineParser();

    void process(const QCoreApplication &app);

    QString source() const;
    QString target() const;

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
};

#endif // COMMANDLINEPARSER_H

#ifndef CLONEJOB_H
#define CLONEJOB_H

#include <QThread>

class CloneJob : public QThread
{
    Q_OBJECT

public:
    enum Status {
        Stoped,
        Started,
        Clone_Headgear,
        Clone_PartitionTable,
        Clone_Partition,
        Save_Info
    };

    explicit CloneJob(QObject *parent = 0);

    bool start(const QString &from, const QString &to);
    Status status() const;

    QString errorString() const;

signals:
    void statusChanged(Status status);
    void failed(const QString &error);

private:
    using QThread::start;

    void run() Q_DECL_OVERRIDE;

    void setStatus(Status s);
    void setErrorString(const QString &error);

    Status m_status;

    QString m_from;
    QString m_to;

    QString m_errorString;
};

#endif // CLONEJOB_H

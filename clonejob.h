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
        Clone_Partition
    };

    explicit CloneJob(QObject *parent = 0);

    bool start(const QString &from, const QString &to);
    Status status() const;

signals:
    void statusChanged(Status status);

private:
    void run() Q_DECL_OVERRIDE;

    void setStatus(Status s);

    Status m_status;

    QString m_from;
    QString m_to;
};

#endif // CLONEJOB_H

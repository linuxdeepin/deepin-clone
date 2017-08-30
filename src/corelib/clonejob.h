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
    qreal progress() const;
    int estimateTime() const; // seconds

    QString errorString() const;

signals:
    void statusChanged(Status status);
    void failed(const QString &error);
    void progressChanged(qreal progress);

private:
    using QThread::start;

    void run() Q_DECL_OVERRIDE;

    void setStatus(Status s);
    void setErrorString(const QString &error);

    Status m_status;

    QString m_from;
    QString m_to;

    QString m_errorString;

    qreal m_progress = 0;
    int m_estimateTime = 0;
};

#endif // CLONEJOB_H

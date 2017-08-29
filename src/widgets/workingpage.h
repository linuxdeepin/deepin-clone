#ifndef WORKINGPAGE_H
#define WORKINGPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
QT_END_NAMESPACE

class CloneJob;
class WorkingPage : public QWidget
{
    Q_OBJECT
public:
    explicit WorkingPage(const QString &from, const QString &to, QWidget *parent = 0);

    bool isError() const;

private:
    CloneJob *m_job;

    QProgressBar *m_progress;
    QLabel *m_writtenSizeLabel;
    QLabel *m_timeRemainingLabel;
};

#endif // WORKINGPAGE_H

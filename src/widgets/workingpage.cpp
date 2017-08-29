#include "workingpage.h"
#include "ddiskinfo.h"
#include "helper.h"
#include "clonejob.h"

#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

WorkingPage::WorkingPage(const QString &from, const QString &to, QWidget *parent)
    : QWidget(parent)
{
    DDiskInfo from_info = DDiskInfo::getInfo(from);
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_progress->setFixedWidth(500);

    qint64 total_readable_data_size = 0;

    if (from_info)
        total_readable_data_size = from_info.totalReadableDataSize();

    QLabel *tip_label = new QLabel(tr("任务正在进行，请稍后..."), this);
    QLabel *totalSize_label = new QLabel(tr("数据总大小：%1").arg(Helper::sizeDisplay(total_readable_data_size)), this);

    m_writtenSizeLabel = new QLabel(this);
    m_timeRemainingLabel = new QLabel(this);

    layout->addStretch();
    layout->addWidget(m_progress, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(tip_label, 0, Qt::AlignHCenter);
    layout->addWidget(totalSize_label, 0, Qt::AlignHCenter);
    layout->addWidget(m_writtenSizeLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_timeRemainingLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    m_job = new CloneJob(this);

    connect(m_job, &CloneJob::progressChanged, this, [this, total_readable_data_size] (qreal progress) {
        m_progress->setValue(progress * m_progress->maximum());
        m_writtenSizeLabel->setText(tr("已写入大小：%1").arg(Helper::sizeDisplay(total_readable_data_size * progress)));
        m_timeRemainingLabel->setText(tr("预计剩余时间："));
    });

    m_job->start(from, to);
}

bool WorkingPage::isError() const
{
    return !m_job->errorString().isEmpty();
}

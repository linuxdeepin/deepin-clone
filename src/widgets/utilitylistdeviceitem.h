#ifndef UTILITYLISTDEVICEITEM_H
#define UTILITYLISTDEVICEITEM_H

#include "utilitylistitem.h"

QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

class UtilityListDeviceItem : public UtilityListItem
{
    Q_OBJECT
public:
    explicit UtilityListDeviceItem(QWidget *parent = 0);

    void setSize(qint64 used, qint64 total);

private:
    QProgressBar *m_progressBar;
    QLabel *m_sizeLabel;
};

#endif // UTILITYLISTDEVICEITEM_H

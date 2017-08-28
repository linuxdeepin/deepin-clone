#ifndef UTILITYLISTITEM_H
#define UTILITYLISTITEM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

class UtilityListItem : public QWidget
{
public:
    explicit UtilityListItem(QWidget *parent);

    void setIcon(const QIcon &icon, const QSize &size = QSize());
    void setWidget(QWidget *w);

private:
    QLabel *m_icon;
    QWidget *m_widget = NULL;
    QVBoxLayout *m_layout;
};

#endif // UTILITYLISTITEM_H

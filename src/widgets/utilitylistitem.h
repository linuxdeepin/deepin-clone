#ifndef UTILITYLISTITEM_H
#define UTILITYLISTITEM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

class RightContentLabel;
class UtilityListItem : public QWidget
{
public:
    explicit UtilityListItem(QWidget *parent);

    void setTitle(const QString &title);
    void setMessage(const QString &message);

    void setIcon(const QIcon &icon, const QSize &size = QSize());
    void addWidget(QWidget *w, int stretch = 0, Qt::Alignment alignment = Qt::Alignment());

private:
    QLabel *m_icon;
    RightContentLabel *m_label;
    QHBoxLayout *m_layout;
};

#endif // UTILITYLISTITEM_H

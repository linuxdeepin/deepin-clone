#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QWidget>
#include <QBoxLayout>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class IconLabel : public QWidget
{
public:
    explicit IconLabel(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon, int extent = 0);
    void setDirection(QBoxLayout::Direction direction);

private:
    QLabel *m_icon;
    QLabel *m_label;
};

#endif // ICONLABEL_H

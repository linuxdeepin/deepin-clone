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

    QString title() const;
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon, int extent = 0);
    void setDirection(QBoxLayout::Direction direction);

    void setChecked(bool checked);

private:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    QLabel *m_icon;
    QLabel *m_label;

    bool m_isChecked = false;
};

#endif // ICONLABEL_H

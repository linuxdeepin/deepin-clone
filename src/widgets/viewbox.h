#ifndef VIEWBOX_H
#define VIEWBOX_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QFrame;
class QBoxLayout;
QT_END_NAMESPACE

class IconLabel;
class ViewBox : public QWidget
{
    Q_OBJECT

public:
    explicit ViewBox(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    void setContent(QWidget *widget, bool full = false);

private:
    void init();

    IconLabel *m_title;
    QFrame *m_borderFrame;
    QBoxLayout *m_borderFrameLayout;
    QWidget *m_contentWidget = NULL;
};

#endif // VIEWBOX_H

#ifndef CONTENTPAIRPAGE_H
#define CONTENTPAIRPAGE_H

#include <QWidget>

class ViewBox;
class ContentPairPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContentPairPage(QWidget *parent = 0);

    void setLeftTitle(const QString &title);
    void setRightTitle(const QString &title);

    void setLeftContent(QWidget *widget, bool full = false);
    void setRightContent(QWidget *widget, bool full = false);

private:
    void init();

    ViewBox *m_leftBox;
    ViewBox *m_rightBox;
};

#endif // CONTENTPAIRPAGE_H

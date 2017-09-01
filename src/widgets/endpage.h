#ifndef ENDPAGE_H
#define ENDPAGE_H

#include <QWidget>
#include <QLabel>

class EndPage : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        Success,
        Failed
    };

    explicit EndPage(Mode mode, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setMessage(const QString &message);

private:
    QLabel *m_title;
    QLabel *m_message;
};

#endif // ENDPAGE_H

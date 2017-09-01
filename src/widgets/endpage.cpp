#include "endpage.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QDebug>

EndPage::EndPage(Mode mode, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *icon = new QLabel(this);

    if (mode == Success)
        icon->setPixmap(QIcon(":/icons/success.svg").pixmap(128));
    else if (mode == Fail)
        icon->setPixmap(QIcon(":/icons/fail.svg").pixmap(128));
    else
        icon->setPixmap(QIcon(":/icons/warning.svg").pixmap(128));

    m_title = new QLabel(this);
    m_message = new QLabel(this);
    m_message->setObjectName("MessageLabel");
    m_message->setWordWrap(true);

    layout->addStretch();
    layout->addWidget(icon, 0, Qt::AlignHCenter);
    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addWidget(m_message, 0, Qt::AlignHCenter);
    layout->addStretch();
}

void EndPage::setTitle(const QString &title)
{
    m_title->setText(title);
}

void EndPage::setMessage(const QString &message)
{
    m_message->setText(message);
}

void EndPage::resizeEvent(QResizeEvent *e)
{
    int width = this->width() * 0.606;

    if (m_message->sizeHint().width() > width)
        m_message->resize(width, m_message->heightForWidth(width));
    else
        m_message->adjustSize();

    return QWidget::resizeEvent(e);
}

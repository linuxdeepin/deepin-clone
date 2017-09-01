#include "endpage.h"

#include <QIcon>
#include <QVBoxLayout>

EndPage::EndPage(Mode mode, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *icon = new QLabel(this);

    if (mode == Success)
        icon->setPixmap(QIcon(":/icons/success.svg").pixmap(128));
    else
        icon->setPixmap(QIcon(":/icons/fail.svg").pixmap(128));

    m_title = new QLabel(this);
    m_message = new QLabel(this);

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

#include "iconlabel.h"

#include <QLabel>
#include <QIcon>

IconLabel::IconLabel(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);
    m_label = new QLabel(this);

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addWidget(m_label, 0, Qt::AlignHCenter);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void IconLabel::setTitle(const QString &title)
{
    m_label->setText(title);
}

void IconLabel::setIcon(const QIcon &icon, int extent)
{
    if (extent <= 0) {
        QBoxLayout::Direction d = static_cast<QBoxLayout*>(layout())->direction();

        if (d == QBoxLayout::LeftToRight || d == QBoxLayout::RightToLeft)
            extent = m_label->sizeHint().height();
        else
            extent = 50;
    }

    m_icon->setPixmap(icon.pixmap(extent));
}

void IconLabel::setDirection(QBoxLayout::Direction direction)
{
    static_cast<QBoxLayout*>(layout())->setDirection(direction);
}

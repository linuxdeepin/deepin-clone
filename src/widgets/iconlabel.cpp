#include "iconlabel.h"

#include <QLabel>
#include <QIcon>
#include <QPainter>

IconLabel::IconLabel(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);
    m_label = new QLabel(this);

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addWidget(m_label, 0, Qt::AlignHCenter);
    layout->addStretch();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QString IconLabel::title() const
{
    return m_label->text();
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

void IconLabel::setChecked(bool checked)
{
    if (m_isChecked == checked)
        return;

    m_isChecked = checked;

    update();
}

void IconLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!m_isChecked)
        return;

    QPainter pa(this);
    QPen pen = pa.pen();

    pen.setColor(QColor("#2ca7f8"));
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    pa.setPen(pen);
    pa.setRenderHint(QPainter::Antialiasing);
    pa.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 8, 8);
}

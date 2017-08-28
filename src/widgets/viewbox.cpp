#include "viewbox.h"
#include "iconlabel.h"

#include <QLabel>
#include <QVBoxLayout>

ViewBox::ViewBox(QWidget *parent)
    : QWidget(parent)
{
    init();
}

void ViewBox::setTitle(const QString &title)
{
    m_title->setTitle(title);
}

void ViewBox::setIcon(const QIcon &icon)
{
    m_title->setIcon(icon);
}

void ViewBox::setContent(QWidget *widget, bool full)
{
    if (m_contentWidget)
        m_borderFrameLayout->removeWidget(m_contentWidget);

    m_contentWidget = widget;

    if (full)
        m_borderFrameLayout->addWidget(widget);
    else
        m_borderFrameLayout->addWidget(widget, 0, Qt::AlignLeft | Qt::AlignTop);
}

void ViewBox::init()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_title = new IconLabel(this);
    m_borderFrame = new QFrame(this);
    m_borderFrame->setObjectName("BorderFrame");

    layout->addWidget(m_title);
    layout->addWidget(m_borderFrame);

    m_borderFrameLayout = new QVBoxLayout(m_borderFrame);
    m_borderFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

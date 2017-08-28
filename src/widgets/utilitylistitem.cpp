#include "utilitylistitem.h"

#include <dseparatorhorizontal.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QDebug>

DWIDGET_USE_NAMESPACE

UtilityListItem::UtilityListItem(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);

    QHBoxLayout *main_layout = new QHBoxLayout(this);

    m_layout = new QVBoxLayout();

    main_layout->addWidget(m_icon);
    main_layout->addLayout(m_layout);
}

void UtilityListItem::setIcon(const QIcon &icon, const QSize &size)
{
    if (size.isValid())
        m_icon->setPixmap(icon.pixmap(size));
    else
        m_icon->setPixmap(icon.pixmap(height()));
}

void UtilityListItem::setWidget(QWidget *w)
{
    if (!w)
        return;

    if (m_widget)
        return;

    m_widget = w;
    m_layout->addWidget(m_widget);
    m_layout->addWidget(new DSeparatorHorizontal());
}

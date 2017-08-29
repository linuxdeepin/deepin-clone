#include "utilitylistitem.h"

#include <dseparatorhorizontal.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QDebug>

DWIDGET_USE_NAMESPACE

class RightContentLabel : public QWidget
{
public:
    explicit RightContentLabel(const QString &title, const QString &message, QWidget *parent = 0);
    explicit RightContentLabel(QWidget *parent = 0);

private:
    QLabel *titleLabel;
    QLabel *messageLabel;

    friend class UtilityListItem;
};

RightContentLabel::RightContentLabel(const QString &title, const QString &message, QWidget *parent)
    : QWidget(parent)
{
    titleLabel->setText(title);
    messageLabel->setText(message);
}

RightContentLabel::RightContentLabel(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    titleLabel = new QLabel(this);
    titleLabel->setObjectName("RightContentLabel_Title");

    messageLabel = new QLabel(this);
    messageLabel->setObjectName("RightContentLabel_Message");

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
}

UtilityListItem::UtilityListItem(QWidget *parent)
    : QWidget(parent)
{
    m_icon = new QLabel(this);

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    QVBoxLayout *content_layout = new QVBoxLayout();

    m_layout = new QHBoxLayout();
    m_label = new RightContentLabel(this);
    m_layout->addWidget(m_label);

    main_layout->addWidget(m_icon);
    main_layout->addLayout(content_layout);

    content_layout->addLayout(m_layout);
    content_layout->addWidget(new DSeparatorHorizontal());
}

void UtilityListItem::setTitle(const QString &title)
{
    m_label->titleLabel->setText(title);
}

void UtilityListItem::setMessage(const QString &message)
{
    m_label->messageLabel->setText(message);
}

QString UtilityListItem::title() const
{
    return m_label->titleLabel->text();
}

void UtilityListItem::setIcon(const QIcon &icon, const QSize &size)
{
    m_iconFile = icon;

    if (size.isValid())
        m_icon->setPixmap(icon.pixmap(size));
    else
        m_icon->setPixmap(icon.pixmap(height()));
}

QIcon UtilityListItem::icon() const
{
    return m_iconFile;
}

void UtilityListItem::addWidget(QWidget *w, int stretch, Qt::Alignment alignment)
{
    m_layout->addWidget(w, stretch, alignment);
}

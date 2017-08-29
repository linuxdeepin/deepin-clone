#include "utilitylist.h"
#include "utilitylistitem.h"

UtilityList::UtilityList(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionRectVisible(false);
}

void UtilityList::addItem(UtilityListItem *item)
{
    QListWidgetItem *lwi = new QListWidgetItem(this);
    QListWidget::addItem(lwi);

    QSize size = item->sizeHint();

    size.setWidth(qMin(size.width(), width()));

    lwi->setSizeHint(size);
    setItemWidget(lwi, item);

    if (count() == 1)
        lwi->setSelected(true);
}

void UtilityList::addItem(const QIcon &icon, const QString &title, const QString &message, const QSize &iconSize)
{
    UtilityListItem *item = new UtilityListItem(this);

    item->setIcon(icon, iconSize);
    item->setTitle(title);
    item->setMessage(message);

    addItem(item);
}

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

    lwi->setSizeHint(item->sizeHint());
    setItemWidget(lwi, item);
}

void UtilityList::addItem(const QIcon &icon, QWidget *widget, const QSize &iconSize)
{
    UtilityListItem *item = new UtilityListItem(this);

    item->setIcon(icon, iconSize);
    item->setWidget(widget);

    addItem(item);
}

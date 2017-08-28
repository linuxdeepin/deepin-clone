#ifndef UTILITYLIST_H
#define UTILITYLIST_H

#include <QListWidget>

class UtilityListItem;
class UtilityList : public QListWidget
{
    Q_OBJECT
public:
    explicit UtilityList(QWidget *parent = 0);

    void addItem(UtilityListItem *item);
    void addItem(const QIcon &icon, QWidget *widget, const QSize &iconSize = QSize());
};

#endif // UTILITYLIST_H

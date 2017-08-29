#ifndef SELECTACTIONPAGE_H
#define SELECTACTIONPAGE_H

#include "contentpairpage.h"

class IconLabel;
class UtilityList;
class SelectActionPage : public ContentPairPage
{
    Q_OBJECT

public:
    enum Mode {
        Disk,
        Partition
    };

    enum Action {
        Clone,
        Backup,
        Restore
    };

    explicit SelectActionPage(QWidget *parent = 0);

    Mode mode() const;
    Action action() const;

private:
    void init();
    void setMode(Mode mode);
    void setAction(Action action);

    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    Mode m_mode = Disk;
    Action m_action = Clone;
    IconLabel *m_diskItem = NULL;
    IconLabel *m_partItem = NULL;
    UtilityList *m_rightContent = NULL;
};

#endif // SELECTACTIONPAGE_H

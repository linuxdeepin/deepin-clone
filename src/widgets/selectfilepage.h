#ifndef SELECTFILEPAGE_H
#define SELECTFILEPAGE_H

#include "contentpairpage.h"
#include "selectactionpage.h"

class SelectFilePage : public ContentPairPage
{
public:
    explicit SelectFilePage(SelectActionPage::Mode mode, SelectActionPage::Action action, QWidget *parent = 0);

    QString source() const;
    QString target() const;
};

#endif // SELECTFILEPAGE_H

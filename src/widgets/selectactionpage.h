#ifndef SELECTACTIONPAGE_H
#define SELECTACTIONPAGE_H

#include "contentpairpage.h"

class SelectActionPage : public ContentPairPage
{
    Q_OBJECT

public:
    explicit SelectActionPage(QWidget *parent = 0);

private:
    void init();
};

#endif // SELECTACTIONPAGE_H

#include <dfileinfo.h>

class TestFile : public DFileInfo
{
    QIODevice *createIODevice() const override;
};

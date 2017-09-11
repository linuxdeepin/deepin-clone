#ifndef DGLOBAL_H
#define DGLOBAL_H

#define DG_DFUNC(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d)); }

class Global {
public:
    static bool isOverride;
    static bool isTUIMode;

    static int bufferSize;
    static int compressionLevel;

    static bool disableMD5CheckForDimFile;
};

#endif // DGLOBAL_H

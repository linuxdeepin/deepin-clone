#ifndef DGLOBAL_H
#define DGLOBAL_H

#define DG_DFUNC(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d)); }

#endif // DGLOBAL_H

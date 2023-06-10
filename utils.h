#include <sys/types.h>
#include <sys/systm.h>

#define _dump(_v, _k) do { \
    kprintf("[%s] %s: ", __FUNCTION__, #_v); \
    for (int i = 0; i < (_k); i++) \
        kprintf("%d ", ((uint8_t*)(_v))[i]); \
    kprintf("\n"); \
} while(0)

#define _debug(_fmt, ...) \
    kprintf("[%s] "_fmt, __FUNCTION__, ##__VA_ARGS__)

#define _printd(_v, _t) \
    kprintf("[%s] %s: %d\n", __FUNCTION__, #_v, (_t)(_v))

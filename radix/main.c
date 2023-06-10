#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <netinet/ip.h>
#include <net/radix.h>
#include <stdio.h>

#include "./radix.c"
#include "../utils.h"

struct radix_data {
    uint8_t len;
    union {
        uint32_t ip;
        struct in_addr in;
    };
    struct radix_node nodes[2];
};

struct {
    struct radix_node_head *addr, *mask;
} r;

static int
test_radix_load(void) {
    struct radix_data addr, mask;
    char buf[32];

    _debug("addr: %s\n", kinet_ntoa(addr.in, buf));
    _debug("mask: %s\n", kinet_ntoa(mask.in, buf));

    addr.ip = htonl((192LL << 24) + (168LL << 16) + (1LL << 8) + 1);
    mask.ip = htonl(~((1LL << (32-24)) - 1) & 0xffffffff);
    addr.len = mask.len = offsetof(struct radix_data, in) + sizeof(struct in_addr);

    r.mask = r.addr = NULL;

    //if (!rn_inithead((void**)&r.mask, NULL, 0))
    //    return ENOMEM;

    if (!rn_inithead((void**)&r.mask, NULL, offsetof(struct radix_data, in) * NBBY))
        return ENOMEM;

    if (!rn_inithead((void**)&r.addr, r.mask, offsetof(struct radix_data, in) * NBBY))
        return ENOMEM;

    _dump(r.mask->rnh_treetop->rn_left->rn_key, RN_MAXKEYLEN);
    _dump(r.mask->rnh_treetop->rn_right->rn_key, RN_MAXKEYLEN);

    _dump(r.addr->rnh_treetop->rn_left->rn_key, RN_MAXKEYLEN);
    _dump(r.addr->rnh_treetop->rn_right->rn_key, RN_MAXKEYLEN);

    _printd(r.mask->rnh_treetop->rn_offset, int);
    _printd(r.addr->rnh_treetop->rn_offset, int);
    _printd(r.mask->rnh_treetop->rn_bmask, uint8_t);
    _printd(r.addr->rnh_treetop->rn_bmask, uint8_t);

    r.addr->rnh_addaddr((char*)&addr, (char*)&mask, r.addr, addr.nodes);

    return 0;
}

static void
test_radix_unload(void) {
    _debug("\n");
    if (r.mask != NULL)
        Free(r.mask);
    if (r.addr != NULL)
        Free(r.addr);
}

static int
test_radix_handler(module_t mod, int what, void *arg) {
	switch (what) {
		case MOD_LOAD:
            test_radix_load();
			break;
		case MOD_UNLOAD:
            test_radix_unload();
			break;
		default:
			return (EOPNOTSUPP);
	}
	return (0);
}

static moduledata_t moduledata = {
	"test_radix",
    test_radix_handler,
	NULL
};

DECLARE_MODULE(test_radix, moduledata, SI_SUB_PSEUDO, SI_ORDER_ANY);
MODULE_VERSION(test_radix, 1);


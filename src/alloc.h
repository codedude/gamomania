#ifndef __ALLOC__H
#define __ALLOC__H

#include <stdlib.h>
#include <unistd.h>

#define ALLOC_ZERO(s, t) calloc((s), sizeof(t));
#define ALLOC(s, t) malloc((s) * sizeof(t));

#define FREE(p) free((void *)(p));

#define CHECK_ALLOC(p, r)                                                      \
	if (!(p)) {                                                                \
		write(2, "alloc error\n", 12);                                         \
		return (r);                                                            \
	}

#define RET_IF_NULL(p, r)                                                      \
	if (p == NULL)                                                             \
		return (r);
#define RET_IF_ZERO(p, r)                                                      \
	if (p == 0)                                                                \
		return (r);
#define RET_IF_NEG(p, r)                                                       \
	if (p < 0)                                                                 \
		return (r);
#define RET_IF_POS(p, r)                                                       \
	if (p > 0)                                                                 \
		return (r);
#define RET_IF_FALSE(p, r)                                                     \
	if (!p)                                                                    \
		return (r);
#define RET_IF_TRUE(p, r)                                                      \
	if (p)                                                                     \
		return (r);

#endif

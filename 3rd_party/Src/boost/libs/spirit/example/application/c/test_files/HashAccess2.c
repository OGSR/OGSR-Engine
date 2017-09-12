/* -*- mode: c -*-
 * $Id: HashAccess2.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../Include/simple_hash.h"
*/

int main(int argc, char *argv[]) {
    int i, n = ((argc == 2) ? atoi(argv[1]) : 1);
    char buf[32];
    struct ht_ht *ht1 = ht_create(10000);
    struct ht_ht *ht2 = ht_create(10000);
    struct ht_node *node;

    for (i=0; i<=9999; ++i) {
    sprintf(buf, "foo_%d", i);
    ht_find_new(ht1, buf)->val = i;
    }

    for (i=0; i<n; ++i) {
    for (node=ht_first(ht1); node; node=ht_next(ht1)) {
        ht_find_new(ht2, node->key)->val += node->val;
    }
    }

    printf("%d %d %d %d\n",
       (ht_find(ht1, "foo_1"))->val,
       (ht_find(ht1, "foo_9999"))->val,
       (ht_find(ht2, "foo_1"))->val,
       (ht_find(ht2, "foo_9999"))->val);

    ht_destroy(ht1);
    ht_destroy(ht2);
    return(0);
}

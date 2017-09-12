/* -*- mode: c -*-
 * $Id: HeapSort.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define IM 139968
#define IA   3877
#define IC  29573
*/

double
gen_random(double max) {
    static long last = 42;
    return( max * (last = (last * IA + IC) % IM) / IM );
}

void
heapsort(int n, double *ra) {
    int i, j;
    int ir = n;
    int l = (n >> 1) + 1;
    double rra;

    for (;;) {
    if (l > 1) {
        rra = ra[--l];
    } else {
        rra = ra[ir];
        ra[ir] = ra[1];
        if (--ir == 1) {
        ra[1] = rra;
        return;
        }
    }
    i = l;
    j = l << 1;
    while (j <= ir) {
        if (j < ir && ra[j] < ra[j+1]) { ++j; }
        if (rra < ra[j]) {
        ra[i] = ra[j];
        j += (i = j);
        } else {
        j = ir + 1;
        }
    }
    ra[i] = rra;
    }
}

int
main(int argc, char *argv[]) {
    int N = ((argc == 2) ? atoi(argv[1]) : 1);
    double *ary;
    int i;
    
    /* create an array of N random doubles */
    ary = (double *)malloc((N+1) * sizeof(double));
    for (i=1; i<=N; i++) {
    ary[i] = gen_random(1);
    }

    heapsort(N, ary);

    printf("%.10f\n", ary[N]);

    free(ary);
    return(0);
}


/* -*- mode: c -*-
 * $Id: RandomGenerator.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IM 139968
#define IA 3877
#define IC 29573
*/

/*inline*/ double gen_random(double max) {
    static long last = 42;
    
    last = (last * IA + IC) % IM;
    return( max * last / IM );
}

int main(int argc, char *argv[]) {
    int N = ((argc == 2) ? atoi(argv[1]) : 1) - 1;
    
    while (N--) {
    gen_random(100.0);
    }
    printf("%.9f\n", gen_random(100.0));
    return(0);
}

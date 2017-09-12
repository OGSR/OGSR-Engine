/* -*- mode: c -*-
 * $Id: NestedLoops.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
*/

int
main(int argc, char *argv[]) {
    int n = ((argc == 2) ? atoi(argv[1]) : 1);
    int a, b, c, d, e, f, x=0;
    
    for (a=0; a<n; a++)
    for (b=0; b<n; b++)
        for (c=0; c<n; c++)
        for (d=0; d<n; d++)
            for (e=0; e<n; e++)
            for (f=0; f<n; f++)
                x++;

    printf("%d\n", x);
    return(0);
}

/* -*- mode: c -*-
 * $Id: SumCol.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdio.h>
#include <stdlib.h>

#define MAXLINELEN 128
*/

int
main() {
    int sum = 0;
    char line[128];

    while (fgets(line, MAXLINELEN, stdin)) {
    sum += atoi(line);
    }
    printf("%d\n", sum);
    return(0);
}


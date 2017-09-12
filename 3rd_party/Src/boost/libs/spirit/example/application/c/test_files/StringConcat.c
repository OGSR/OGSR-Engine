/* -*- mode: c -*-
 * $Id: StringConcat.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 */

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STUFF "hello\n"
*/

int
main(int argc, char *argv[]) {
    int n = ((argc == 2) ? atoi(argv[1]) : 1);
    int i, buflen = 32;
    char *strbuf = calloc(sizeof(char), buflen);
    char *strend = strbuf;
    int stufflen = strlen(STUFF);

    if (!strbuf) { perror("calloc strbuf"); exit(1); }
    for (i=0; i<n; i++) {
    if (((strbuf+buflen)-strend) < (stufflen+1)) {
        buflen = 2*buflen;
        strbuf = realloc(strbuf, buflen);
        if (!strbuf) { perror("realloc strbuf"); exit(1); }
        strend = strbuf + strlen(strbuf);
    }
    /* much faster to strcat to strend than to strbuf */
    strcat(strend, STUFF);
    strend += stufflen;
    }
    fprintf(stdout, "%d\n", strlen(strbuf));
    free(strbuf);

    sleep(1);
    return(0);
}

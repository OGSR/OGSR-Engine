/* -*- mode: c -*-
 * $Id: ReverseFile.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 * from Brad Knotwell
 */

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXREAD 4096
*/

int main(int argc, char *argv[]) {
    int nread, len = 0, size = (4 * MAXREAD);
    char *cp, *buf = malloc(size + 1);

    while((nread = read(0,(buf+len),MAXREAD)) > 0) {
        len += nread;
        if(MAXREAD > (size - len)) {
            size <<= 1;
            if((buf = realloc(buf,size+1)) == NULL)
                return(fprintf(stderr,"realloc failed\n"),EXIT_FAILURE);
        }
    }
    
    if(nread == -1) return(fprintf(stderr,"read\n"),EXIT_FAILURE);

    for (cp = buf+len-1; cp != buf; --cp,nread++)
    if ('\n' == *cp) {
            fwrite(cp+1,nread,1,stdout);
            nread = 0;
    }

    fwrite(cp,nread+1,1,stdout);
    free(buf);
    return(EXIT_SUCCESS);
}

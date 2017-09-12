/* -*- mode: c -*-
 * $Id: CountLinesWordsChars.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 * Author: Waldemar Hebisch (hebisch@math.uni.wroc.pl)
 * Optimizations: Michael Herf (mike@herfconsulting.com)
 */

//#include <stdio.h>
//#include <unistd.h>

//#define CHAR int
//#define BSIZ 4096

unsigned long ws[256];
char buff[4096];

int main() {

    int w_cnt=0,l_cnt=0,b_cnt=0,cnt;
    unsigned long was_sp = 1;
    unsigned char *pp, *pe;

    /* Fill tables */
    for (cnt = 0; cnt < 256; cnt++) ws[cnt] = 0;

    /* also: ws['\r']=ws['\v']=ws['\f']= */
    ws[' ']=ws['\t']=ws['\n']=1;
    ws['\n']=65536 + 1;

    /* Main loop */
    while((cnt=read(0,buff,BSIZ))) {
        unsigned long tcnt = 0;

    b_cnt += cnt;
    pe = buff + cnt;
    pp = buff;

    while(pp < pe) {
        tcnt += ws[*pp] ^ was_sp;
        was_sp = ws[*pp] & 0xFFFF;
        pp ++;
    }
    w_cnt += tcnt  & 0xFFFF;
    l_cnt += tcnt >> 16;
    }

    w_cnt += (ws[ buff[b_cnt&(BSIZ-1)-1] ]^1)&1;
    w_cnt>>=1;

    printf("%d %d %d\n", l_cnt, w_cnt, b_cnt);
    return 0;
}

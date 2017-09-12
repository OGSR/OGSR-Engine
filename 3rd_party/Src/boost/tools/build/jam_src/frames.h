/*  
 *  (C) Copyright David Abrahams 2001. Permission to copy, use,
 *  modify, sell and distribute this software is granted provided this
 *  copyright notice appears in all copies. This software is provided
 *  "as is" without express or implied warranty, and with no claim as
 *  to its suitability for any purpose.
 */
#ifndef FRAMES_DWA20011021_H
# define FRAMES_DWA20011021_H

# include "lists.h"
# include "modules.h"

typedef struct _PARSE PARSE;
typedef struct frame FRAME;

struct frame
{
    FRAME* prev;
    LOL args[1];
    module* module;
    PARSE* procedure;
    char*  rulename;
};

void frame_init( FRAME* ); /* implemented in compile.c */
void frame_free( FRAME* ); /* implemented in compile.c */

#endif


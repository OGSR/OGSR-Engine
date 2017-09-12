/*  
 *  (C) Copyright David Abrahams 2001. Permission to copy, use,
 *  modify, sell and distribute this software is granted provided this
 *  copyright notice appears in all copies. This software is provided
 *  "as is" without express or implied warranty, and with no claim as
 *  to its suitability for any purpose.
 */

# include "frames.h"
# include "lists.h"


void frame_init( FRAME* frame )
{
    frame->prev = 0;
    lol_init(frame->args);
    frame->module = root_module();
    frame->rulename = "module scope";
    frame->procedure = 0;
}

void frame_free( FRAME* frame )
{
    lol_free( frame->args );
}

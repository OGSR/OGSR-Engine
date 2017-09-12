/*
 * Copyright 1993-2002 Christopher Seiwald and Perforce Software, Inc.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

/*  This file is ALSO:
 *  (C) Copyright David Abrahams 2001. Permission to copy, use,
 *  modify, sell and distribute this software is granted provided this
 *  copyright notice appears in all copies. This software is provided
 *  "as is" without express or implied warranty, and with no claim as
 *  to its suitability for any purpose.
 */

# include "jam.h"
# include "lists.h"
# include "search.h"
# include "timestamp.h"
# include "pathsys.h"
# include "variable.h"
# include "newstr.h"
# include "compile.h"
# include "strings.h"
# include <string.h>

void call_bind_rule(
    char* target_,
    char* boundname_ )
{
    LIST* bind_rule = var_get( "BINDRULE" );
    if( bind_rule )
    {
        /* No guarantee that target is an allocated string, so be on the
         * safe side */
        char* target = copystr( target_ );
        
        /* Likewise, don't rely on implementation details of newstr.c: allocate
         * a copy of boundname */
        char* boundname = copystr( boundname_ );
        if( boundname && target )
        {
            /* Prepare the argument list */
            FRAME frame[1];
            frame_init( frame );
                    
            /* First argument is the target name */
            lol_add( frame->args, list_new( L0, target ) );
                    
            lol_add( frame->args, list_new( L0, boundname ) );
            if( lol_get( frame->args, 1 ) )
                evaluate_rule( bind_rule->string, frame );
            
            /* Clean up */
            frame_free( frame );
        }
        else
        {
            if( boundname )
                freestr( boundname );
            if( target )
                freestr( target );
        }
    }
}

/*
 * search.c - find a target along $(SEARCH) or $(LOCATE) 
 */

char *
search( 
	char	*target,
 	time_t	*time )
{
	PATHNAME f[1];
    LIST    *varlist;
    string    buf[1];
    int     found = 0;
    char    *boundname = 0;

    string_new( buf );
    /* Parse the filename */

	path_parse( target, f );

    f->f_grist.ptr = 0;
    f->f_grist.len = 0;

    if( varlist = var_get( "LOCATE" ) )
      {
        f->f_root.ptr = varlist->string;
        f->f_root.len = strlen( varlist->string );

	    path_build( f, buf, 1 );

        if( DEBUG_SEARCH )
            printf( "locate %s: %s\n", target, buf->value );

        timestamp( buf->value, time );
        found = 1;
    }
    else if( varlist = var_get( "SEARCH" ) )
    {
        while( varlist )
        {
            f->f_root.ptr = varlist->string;
            f->f_root.len = strlen( varlist->string );

            string_truncate( buf, 0 );
            path_build( f, buf, 1 );

            if( DEBUG_SEARCH )
                printf( "search %s: %s\n", target, buf->value );

            timestamp( buf->value, time );

            if( *time )
            {
                found = 1;
                break;
            }

            varlist = list_next( varlist );
        }
    }

    if (!found)
    {
        /* Look for the obvious */
        /* This is a questionable move.  Should we look in the */
        /* obvious place if SEARCH is set? */

        f->f_root.ptr = 0;
        f->f_root.len = 0;

        string_truncate( buf, 0 );
        path_build( f, buf, 1 );

        if( DEBUG_SEARCH )
            printf( "search %s: %s\n", target, buf->value );

        timestamp( buf->value, time );
    }

    boundname = newstr( buf->value );
    string_free( buf );
        
    /* prepare a call to BINDRULE if the variable is set */
    call_bind_rule( target, boundname );

    return boundname;
}

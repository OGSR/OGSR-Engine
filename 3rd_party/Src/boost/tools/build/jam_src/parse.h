/*
 * Copyright 1993, 2000 Christopher Seiwald.
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

#ifndef PARSE_DWA20011020_H
# define PARSE_DWA20011020_H
# include "frames.h"
# include "modules.h"
# include "lists.h"

/*
 * parse.h - make and destroy parse trees as driven by the parser
 */

/*
 * parse tree node
 */

struct _PARSE {
    LIST    *(*func)( PARSE *p, FRAME *frame );
    PARSE   *left;
    PARSE   *right;
    PARSE   *third;
    char    *string;
    char    *string1;
    int      num;
    int      refs;
/*    module*  module; */
    char*    rulename;
    char*    file;
    int      line;
} ;

void    parse_file( char *f, FRAME* frame );
void    parse_save( PARSE *p );

PARSE * parse_make( 
    LIST    *(*func)( PARSE *p, FRAME* frame ),
    PARSE   *left,
    PARSE   *right,
    PARSE   *third,
    char    *string,
    char    *string1,
    int      num );

void    parse_refer( PARSE *p );
void    parse_free( PARSE *p );
LIST*   parse_evaluate( PARSE *p, FRAME* frame );

#endif


/*
 * Copyright 1993-2002 Christopher Seiwald and Perforce Software, Inc.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

#ifndef JAM_BUILTINS_H
# define JAM_BUILTINS_H

# include "frames.h"

/*
 * builtins.h - compile parsed jam statements
 */

void load_builtins();

LIST *builtin_depends( PARSE *parse, FRAME *args );
LIST *builtin_echo( PARSE *parse, FRAME *args );
LIST *builtin_exit( PARSE *parse, FRAME *args );
LIST *builtin_flags( PARSE *parse, FRAME *args );
LIST *builtin_glob( PARSE *parse, FRAME *args );
LIST *builtin_subst( PARSE  *parse, FRAME *args );
LIST *builtin_match( PARSE *parse, FRAME *args );
LIST *builtin_hdrmacro( PARSE *parse, FRAME *args );
LIST *builtin_rulenames( PARSE *parse, FRAME *args );
LIST *builtin_import( PARSE *parse, FRAME *args );
LIST *builtin_export( PARSE *parse, FRAME *args );
LIST *builtin_caller_module( PARSE *parse, FRAME *args );
LIST *builtin_backtrace( PARSE *parse, FRAME *args );
LIST *builtin_pwd( PARSE *parse, FRAME *args );
LIST *builtin_update( PARSE *parse, FRAME *args );
LIST *builtin_search_for_target( PARSE *parse, FRAME *args );

#endif

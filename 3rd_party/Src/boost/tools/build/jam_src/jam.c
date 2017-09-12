/*
 * /+\
 * +\	Copyright 1993-2002 Christopher Seiwald and Perforce Software, Inc.
 * \+/
 *
 * This file is part of jam.
 *
 * License is hereby granted to use this software and distribute it
 * freely, as long as this copyright notice is retained and modifications 
 * are clearly marked.
 *
 * ALL WARRANTIES ARE HEREBY DISCLAIMED.
 */

/*  This file is ALSO:
 *  (C) Copyright David Abrahams 2001. Permission to copy, use,
 *  modify, sell and distribute this software is granted provided this
 *  copyright notice appears in all copies. This software is provided
 *  "as is" without express or implied warranty, and with no claim as
 *  to its suitability for any purpose.
 */

/*
 * jam.c - make redux
 *
 * See Jam.html for usage information.
 *
 * These comments document the code.
 *
 * The top half of the code is structured such:
 *
 *                       jam 
 *                      / | \ 
 *                 +---+  |  \
 *                /       |   \ 
 *         jamgram     option  \ 
 *        /  |   \              \
 *       /   |    \              \
 *      /    |     \             |
 *  scan     |     compile      make
 *   |       |    /  | \       / |  \
 *   |       |   /   |  \     /  |   \
 *   |       |  /    |   \   /   |    \
 * jambase parse     |   rules  search make1
 *                   |           |      |   \
 *                   |           |      |    \
 *                   |           |      |     \
 *               builtins    timestamp command execute
 *                               |
 *                               |
 *                               |
 *                             filesys
 *
 *
 * The support routines are called by all of the above, but themselves
 * are layered thus:
 *
 *                     variable|expand
 *                      /  |   |   |
 *                     /   |   |   |
 *                    /    |   |   |
 *                 lists   |   |   pathsys
 *                    \    |   |
 *                     \   |   |
 *                      \  |   |
 *                     newstr  |
 *                        \    |
 *                         \   |
 *                          \  |
 *                          hash
 *
 * Roughly, the modules are:
 *
 *	builtins.c - jam's built-in rules
 *	command.c - maintain lists of commands
 *	compile.c - compile parsed jam statements
 *	execunix.c - execute a shell script on UNIX
 *	execvms.c - execute a shell script, ala VMS
 *	expand.c - expand a buffer, given variable values
 *	file*.c - scan directories and archives on *
 *	hash.c - simple in-memory hashing routines 
 *  hdrmacro.c - handle header file parsing for filename macro definitions
 *	headers.c - handle #includes in source files
 *	jambase.c - compilable copy of Jambase
 *	jamgram.y - jam grammar
 *	lists.c - maintain lists of strings
 *	make.c - bring a target up to date, once rules are in place
 *	make1.c - execute command to bring targets up to date
 *	newstr.c - string manipulation routines
 *	option.c - command line option processing
 *	parse.c - make and destroy parse trees as driven by the parser
 *	path*.c - manipulate file names on *
 *	hash.c - simple in-memory hashing routines 
 *	regexp.c - Henry Spencer's regexp
 *	rules.c - access to RULEs, TARGETs, and ACTIONs
 *	scan.c - the jam yacc scanner
 *	search.c - find a target along $(SEARCH) or $(LOCATE) 
 *	timestamp.c - get the timestamp of a file or archive member
 *	variable.c - handle jam multi-element variables
 *
 * 05/04/94 (seiwald) - async multiprocess (-j) support
 * 02/08/95 (seiwald) - -n implies -d2.
 * 02/22/95 (seiwald) - -v for version info.
 * 09/11/00 (seiwald) - PATCHLEVEL folded into VERSION.
 * 01/10/01 (seiwald) - pathsys.h split from filesys.h
 */

# include "jam.h"
# include "option.h"
# include "patchlevel.h"

/* These get various function declarations. */

# include "lists.h"
# include "parse.h"
# include "variable.h"
# include "compile.h"
# include "builtins.h"
# include "rules.h"
# include "newstr.h"
# include "scan.h"
# include "timestamp.h"
# include "make.h"
# include "strings.h"

/* Macintosh is "special" */

# ifdef OS_MAC
# include <QuickDraw.h>
# endif

/* And UNIX for this */

# ifdef unix
# include <sys/utsname.h>
# endif

struct globs globs = {
	0,			/* noexec */
	1,			/* jobs */
	0,			/* quitquick */
	0,			/* newestfirst */
# ifdef OS_MAC
	{ 0, 0 },		/* debug - suppress tracing output */
# else
	{ 0, 1 }, 		/* debug ... */
# endif
	0			/* output commands, not run them */
} ;

/* Symbols to be defined as true for use in Jambase */

static char *othersyms[] = { OSMAJOR, OSMINOR, OSPLAT, JAMVERSYM, 0 } ;

/* Known for sure: 
 *	mac needs arg_enviro
 *	OS2 needs extern environ
 */

# ifdef OS_MAC
# define use_environ arg_environ
# ifdef MPW
QDGlobals qd;
# endif
# endif

/* on Win32-LCC */
# if defined( OS_NT ) && defined( __LCC__ )
#   define  use_environ _environ
# endif

# if defined( __MWERKS__)
# define use_environ _environ
extern char **_environ;
#endif

# ifndef use_environ
# define use_environ environ
# if !defined( __WATCOM__ ) && !defined( OS_OS2 ) && !defined( OS_NT ) 
extern char **environ;
# endif
# endif

# if YYDEBUG != 0
extern int yydebug;
# endif

#ifndef NDEBUG
static void run_unit_tests()
{
# if defined( USE_EXECNT )
    extern void execnt_unit_test();
    execnt_unit_test();
# endif 
    string_unit_test();
    var_expand_unit_test();
}
#endif

int  main( int argc, char **argv, char **arg_environ )
{
    int		n;
    char		*s;
    struct option	optv[N_OPTS];
    char		*all = "all";
    int		anyhow = 0;
    int		status;
    int arg_c = argc;
    char ** arg_v = argv;

# ifdef OS_MAC
    InitGraf(&qd.thePort);
# endif

    argc--, argv++;

	if( ( n = getoptions( argc, argv, "-:d:j:f:gs:t:ano:qv", optv ) ) < 0 )
    {
        printf( "\nusage: jam [ options ] targets...\n\n" );

        printf( "-a      Build all targets, even if they are current.\n" );
        printf( "-dx     Set the debug level to x (0-9).\n" );
        printf( "-fx     Read x instead of Jambase.\n" );
	    /* printf( "-g      Build from newest sources first.\n" ); */
        printf( "-jx     Run up to x shell commands concurrently.\n" );
        printf( "-n      Don't actually execute the updating actions.\n" );
        printf( "-ox     Write the updating actions to file x.\n" );
		printf( "-q      Quit quickly as soon as a target fails.\n" );
        printf( "-sx=y   Set variable x=y, overriding environment.\n" );
        printf( "-tx     Rebuild x, even if it is up-to-date.\n" );
        printf( "-v      Print the version of jam and exit.\n" );
        printf( "--x     Option is ignored.\n\n" );

        exit( EXITBAD );
    }

    argc -= n, argv += n;

    /* Version info. */

    if( ( s = getoptval( optv, 'v', 0 ) ) )
    {
        printf( "Boost.Jam  " );
        printf( "Version %s. %s.\n", VERSION, OSMINOR );
	   printf( "   Copyright 1993-2002 Christopher Seiwald and Perforce Software, Inc.  \n" );
        printf( "   Copyright 2001 David Turner.\n" );
        printf( "   Copyright 2001-2002 David Abrahams.\n" );

        return EXITOK;
    }

    /* Pick up interesting options */

    if( ( s = getoptval( optv, 'n', 0 ) ) )
        globs.noexec++, globs.debug[2] = 1;

	if( ( s = getoptval( optv, 'q', 0 ) ) )
 	    globs.quitquick = 1;
    if( ( s = getoptval( optv, 'a', 0 ) ) )
        anyhow++;

    if( ( s = getoptval( optv, 'j', 0 ) ) )
        globs.jobs = atoi( s );

	if( ( s = getoptval( optv, 'g', 0 ) ) )
	    globs.newestfirst = 1;

    /* Turn on/off debugging */

    for( n = 0; s = getoptval( optv, 'd', n ); n++ )
    {
        int i;

        /* First -d, turn off defaults. */

        if( !n )
            for( i = 0; i < DEBUG_MAX; i++ )
                globs.debug[i] = 0;

        i = atoi( s );

        if( i < 0 || i >= DEBUG_MAX )
        {
            printf( "Invalid debug level '%s'.\n", s );
            continue;
        }

        /* n turns on levels 1-n */
        /* +n turns on level n */

        if( *s == '+' )
            globs.debug[i] = 1;
        else while( i )
            globs.debug[i--] = 1;
    }

#ifndef NDEBUG
    run_unit_tests();
#endif
#if YYDEBUG != 0
    if ( DEBUG_PARSE )
        yydebug = 1;
#endif

    /* Set JAMDATE first */

    {
        char *date;
        time_t clock;
        time( &clock );
        date = newstr( ctime( &clock ) );

        /* Trim newline from date */

        if( strlen( date ) == 25 )
            date[ 24 ] = 0;

        var_set( "JAMDATE", list_new( L0, newstr( date ) ), VAR_SET );
    }

    var_set( "JAM_VERSION",
             list_new( list_new( list_new( L0, newstr( "03" ) ), newstr( "01" ) ), 
                       newstr( "03" ) ),
             VAR_SET );

    /* And JAMUNAME */
# ifdef unix
    {
        struct utsname u;

        if( uname( &u ) >= 0 )
        {
            var_set( "JAMUNAME", 
                     list_new( 
                         list_new(
                             list_new(
                                 list_new(
                                     list_new( L0, 
                                               newstr( u.sysname ) ),
                                     newstr( u.nodename ) ),
                                 newstr( u.release ) ),
                             newstr( u.version ) ),
                         newstr( u.machine ) ), VAR_SET );
        }
    }
# endif /* unix */

	/*
	 * Jam defined variables OS, OSPLAT
	 */

    var_defines( othersyms );

    /* load up environment variables */

    var_defines( use_environ );

    /* Load up variables set on command line. */

    for( n = 0; s = getoptval( optv, 's', n ); n++ )
    {
        char *symv[2];
        symv[0] = s;
        symv[1] = 0;
        var_defines( symv );
    }

    /* Set the ARGV to reflect the complete list of arguments of invocation. */

    for ( n = 0; n < arg_c; ++n )
    {
        var_set( "ARGV", list_new( L0, newstr( arg_v[n] ) ), VAR_APPEND );
    }

	/* Initialize built-in rules */

	load_builtins();

    /* Add the targets in the command line to update list */

    for ( n = 0; n < argc; ++n )
    {
        mark_target_for_updating(argv[n]);
    }

    /* Parse ruleset */

    {
        FRAME frame[1];
        frame_init( frame );
	for( n = 0; s = getoptval( optv, 'f', n ); n++ )
	    parse_file( s, frame );

	if( !n )
	    parse_file( "+", frame );
    }

    status = yyanyerrors();

    /* Manually touch -t targets */

    for( n = 0; s = getoptval( optv, 't', n ); n++ )
        touchtarget( s );

    /* If an output file is specified, set globs.cmdout to that */

    if( s = getoptval( optv, 'o', 0 ) )
    {
        if( !( globs.cmdout = fopen( s, "w" ) ) )
        {
            printf( "Failed to write to '%s'\n", s );
            exit( EXITBAD );
        }
        globs.noexec++;
    }

    /* Now make target */

    {
        LIST* targets = targets_to_update();
        if ( !targets )
        {
            status |= make( 1, &all, anyhow );
        }
        else 
        {
            int targets_count = list_length(targets);
            char **targets2 = (char **)malloc(targets_count * sizeof(char *));
            int n = 0;
            for ( ; targets; targets = list_next(targets) )
            {
                targets2[n++] = targets->string;
            }
            status |= make( targets_count, targets2, anyhow );       
            free(targets);
        }
    }


    if ( DEBUG_PROFILE )
        profile_dump();

    /* Widely scattered cleanup */

    var_done();
    donerules();
    donestamps();
    donestr();

    /* close cmdout */

    if( globs.cmdout )
        fclose( globs.cmdout );

    return status ? EXITBAD : EXITOK;
}

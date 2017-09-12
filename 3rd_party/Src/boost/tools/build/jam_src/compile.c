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

# include "jam.h"

# include "lists.h"
# include "parse.h"
# include "compile.h"
# include "variable.h"
# include "expand.h"
# include "rules.h"
# include "newstr.h"
# include "make.h"
# include "search.h"
# include "hdrmacro.h"
# include "hash.h"
# include "modules.h"
# include "strings.h"

# include <time.h>
# include <assert.h>
# include <string.h>

/*
 * compile.c - compile parsed jam statements
 *
 * External routines:
 *
 *  compile_append() - append list results of two statements
 *	compile_eval() - evaluate if to determine which leg to compile
 *  compile_foreach() - compile the "for x in y" statement
 *  compile_if() - compile 'if' rule
 *  compile_while() - compile 'while' rule
 *  compile_include() - support for 'include' - call include() on file
 *  compile_list() - expand and return a list 
 *  compile_local() - declare (and set) local variables
 *  compile_null() - do nothing -- a stub for parsing
 *  compile_on() - run rule under influence of on-target variables
 *  compile_rule() - compile a single user defined rule
 *  compile_rules() - compile a chain of rules
 *  compile_set() - compile the "set variable" statement
 *  compile_setcomp() - support for `rule` - save parse tree 
 *  compile_setexec() - support for `actions` - save execution string 
 *  compile_settings() - compile the "on =" (set variable on exec) statement
 *  compile_switch() - compile 'switch' rule
 *
 * Internal routines:
 *
 *  debug_compile() - printf with indent to show rule expansion.
 *  evaluate_rule() - execute a rule invocation
 *
 *  builtin_depends() - DEPENDS/INCLUDES rule
 *  builtin_echo() - ECHO rule
 *  builtin_exit() - EXIT rule
 *  builtin_flags() - NOCARE, NOTFILE, TEMPORARY rule
 *
 * 02/03/94 (seiwald) - Changed trace output to read "setting" instead of 
 *          the awkward sounding "settings".
 * 04/12/94 (seiwald) - Combined build_depends() with build_includes().
 * 04/12/94 (seiwald) - actionlist() now just appends a single action.
 * 04/13/94 (seiwald) - added shorthand L0 for null list pointer
 * 05/13/94 (seiwald) - include files are now bound as targets, and thus
 *          can make use of $(SEARCH)
 * 06/01/94 (seiwald) - new 'actions existing' does existing sources
 * 08/23/94 (seiwald) - Support for '+=' (append to variable)
 * 12/20/94 (seiwald) - NOTIME renamed NOTFILE.
 * 01/22/95 (seiwald) - Exit rule.
 * 02/02/95 (seiwald) - Always rule; LEAVES rule.
 * 02/14/95 (seiwald) - NoUpdate rule.
 * 09/11/00 (seiwald) - new evaluate_rule() for headers().
 * 09/11/00 (seiwald) - compile_xxx() now return LIST *.
 *          New compile_append() and compile_list() in
 *          support of building lists here, rather than
 *          in jamgram.yy.
 * 01/10/00 (seiwald) - built-ins split out to builtin.c.
 */

static void debug_compile( int which, char *s, FRAME* frame );
int glob( char *s, char *c );
/* Internal functions from builtins.c */
void backtrace_line( FRAME *frame );
void print_source_line( PARSE* p );


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

/*
 * compile_append() - append list results of two statements
 *
 *  parse->left more compile_append() by left-recursion
 *  parse->right    single rule
 */

LIST *
compile_append(
    PARSE   *parse,
    FRAME *frame )
{
    /* Append right to left. */

    return list_append( 
        parse_evaluate( parse->left, frame ),
        parse_evaluate( parse->right, frame ) );
}

/*
 * compile_eval() - evaluate if to determine which leg to compile
 *
 * Returns:
 *	list 	if expression true - compile 'then' clause
 *	L0	if expression false - compile 'else' clause
 */

static int
lcmp( LIST *t, LIST *s )
{
	int status = 0;

	while( !status && ( t || s ) )
	{
	    char *st = t ? t->string : "";
	    char *ss = s ? s->string : "";

	    status = strcmp( st, ss );

	    t = t ? list_next( t ) : t;
	    s = s ? list_next( s ) : s;
	}

	return status;
}

LIST *
compile_eval(
	PARSE	*parse,
	FRAME	*frame )
{
	LIST *ll, *lr, *s, *t;
	int status = 0;

	/* Short circuit lr eval for &&, ||, and 'in' */

	ll = parse_evaluate( parse->left, frame );
	lr = 0;

	switch( parse->num )
	{
	case EXPR_AND: 
	case EXPR_IN: 	if( ll ) goto eval; break;
	case EXPR_OR: 	if( !ll ) goto eval; break;
	default: eval: 	lr = parse_evaluate( parse->right, frame );
	}

	/* Now eval */

	switch( parse->num )
	{
	case EXPR_NOT:	
		if( !ll ) status = 1;
		break;

	case EXPR_AND:
		if( ll && lr ) status = 1;
		break;

	case EXPR_OR:
		if( ll || lr ) status = 1;
		break;

	case EXPR_IN:
		/* "a in b": make sure each of */
		/* ll is equal to something in lr. */

		for( t = ll; t; t = list_next( t ) )
		{
		    for( s = lr; s; s = list_next( s ) )
			if( !strcmp( t->string, s->string ) )
			    break;
		    if( !s ) break;
		}

		/* No more ll? Success */

		if( !t ) status = 1;

		break;

	case EXPR_EXISTS:       if( lcmp( ll, L0 ) != 0 ) status = 1; break;
	case EXPR_EQUALS:	if( lcmp( ll, lr ) == 0 ) status = 1; break;
	case EXPR_NOTEQ:	if( lcmp( ll, lr ) != 0 ) status = 1; break;
	case EXPR_LESS:		if( lcmp( ll, lr ) < 0  ) status = 1; break;
	case EXPR_LESSEQ:	if( lcmp( ll, lr ) <= 0 ) status = 1; break;
	case EXPR_MORE:		if( lcmp( ll, lr ) > 0  ) status = 1; break;
	case EXPR_MOREEQ:	if( lcmp( ll, lr ) >= 0 ) status = 1; break;

	}

	if( DEBUG_IF )
	{
	    debug_compile( 0, "if", frame );
	    list_print( ll );
	    printf( "(%d) ", status );
	    list_print( lr );
	    printf( "\n" );
	}

	/* Find something to return. */
	/* In odd circumstances (like "" = "") */
	/* we'll have to return a new string. */

	if( !status ) t = 0;
	else if( ll ) t = ll, ll = 0;
	else if( lr ) t = lr, lr = 0;
	else t = list_new( L0, newstr( "1" ) );

	if( ll ) list_free( ll );
	if( lr ) list_free( lr );
	return t;
}


/*
 * compile_foreach() - compile the "for x in y" statement
 *
 * Compile_foreach() resets the given variable name to each specified
 * value, executing the commands enclosed in braces for each iteration.
 *
 *  parse->string   index variable
 *  parse->left variable values
 *  parse->right    rule to compile
 */

LIST *
compile_foreach(
    PARSE   *parse,
    FRAME *frame )
{
    LIST    *nv = parse_evaluate( parse->left, frame );
    LIST    *l;
    SETTINGS *s = 0;
        
        if ( parse->num )
        {
            s = addsettings( s, 0, parse->string, L0 );
            pushsettings( s );
        }

    /* Call var_set to reset $(parse->string) for each val. */

    for( l = nv; l; l = list_next( l ) )
    {
        LIST *val = list_new( L0, copystr( l->string ) );

        var_set( parse->string, val, VAR_SET );

        list_free( parse_evaluate( parse->right, frame ) );
    }

        if ( parse->num )
        {
            popsettings( s );
            freesettings( s );
        }

    list_free( nv );

    return L0;
}

/*
 * compile_if() - compile 'if' rule
 *
 *  parse->left     condition tree
 *  parse->right        then tree
 *  parse->third        else tree
 */

LIST *
compile_if(
    PARSE   *p,
    FRAME *frame )
{
    LIST *l = parse_evaluate( p->left, frame );
    if( l )
    {
        list_free( l );
        return parse_evaluate( p->right, frame );
    }
    else
    {
        return parse_evaluate( p->third, frame );
    }
}

LIST *
compile_while(
    PARSE   *p,
    FRAME *frame )
{
    LIST *r = 0;
    LIST *l;
    while ( l = parse_evaluate( p->left, frame ) )
    {
        list_free( l );
        if( r ) list_free( r );
        r = parse_evaluate( p->right, frame );
    }
    return r;
}


/*
 * compile_include() - support for 'include' - call include() on file
 *
 *  parse->left list of files to include (can only do 1)
 */

LIST *
compile_include(
    PARSE   *parse,
    FRAME *frame )
{
    LIST    *nt = parse_evaluate( parse->left, frame );

    if( DEBUG_COMPILE )
    {
        debug_compile( 0, "include", frame);
        list_print( nt );
        printf( "\n" );
    }

    if( nt )
    {
        TARGET *t = bindtarget( nt->string );

            /* DWA 2001/10/22 - Perforce Jam clears the arguments here, which
             * prevents an included file from being treated as part of the body
             * of a rule. I didn't see any reason to do that, so I lifted the
             * restriction.
             */
               
        /* Bind the include file under the influence of */
        /* "on-target" variables.  Though they are targets, */
        /* include files are not built with make(). */

        pushsettings( t->settings );
        t->boundname = search( t->name, &t->time );
        popsettings( t->settings );

        parse_file( t->boundname, frame );
    }

    list_free( nt );

    return L0;
}

LIST *
compile_module(
    PARSE   *p,
    FRAME *frame )
{
    /* Here we are entering a module declaration block. 
     */
    LIST* module_name = parse_evaluate( p->left, frame );
    LIST* result;

    module* outer_module = frame->module;
    frame->module = module_name ? bindmodule( module_name->string ) : root_module();

    if ( outer_module != frame->module )
    {
        exit_module( outer_module );
        enter_module( frame->module );
    }
    
    result = parse_evaluate( p->right, frame );
    
    if ( outer_module != frame->module )
    {
        exit_module( frame->module );
        enter_module( outer_module );
        frame->module = outer_module;
    }
    
    list_free( module_name );
    return result;
}


/*
 * compile_list() - expand and return a list 
 *
 *  parse->string - character string to expand
 */

LIST *
compile_list(
    PARSE   *parse,
    FRAME *frame )
{
    /* voodoo 1 means: s is a copyable string */
    char *s = parse->string;
    return var_expand( L0, s, s + strlen( s ), frame->args, 1 );
}

/*
 * compile_local() - declare (and set) local variables
 *
 *  parse->left list of variables
 *  parse->right    list of values
 *  parse->third    rules to execute
 */

LIST *
compile_local(
    PARSE   *parse,
    FRAME *frame )
{
    LIST *l;
    SETTINGS *s = 0;
    LIST    *nt = parse_evaluate( parse->left, frame );
    LIST    *ns = parse_evaluate( parse->right, frame );
    LIST    *result;

    if( DEBUG_COMPILE )
    {
        debug_compile( 0, "local", frame);
        list_print( nt );
        printf( " = " );
        list_print( ns );
        printf( "\n" );
    }

    /* Initial value is ns */

    for( l = nt; l; l = list_next( l ) )
        s = addsettings( s, 0, l->string, list_copy( (LIST*)0, ns ) );

    list_free( ns );
    list_free( nt );

    /* Note that callees of the current context get this "local" */
    /* variable, making it not so much local as layered. */

    pushsettings( s );
    result = parse_evaluate( parse->third, frame );
    popsettings( s );

    freesettings( s );

    return result;
}

/*
 * compile_null() - do nothing -- a stub for parsing
 */

LIST *
compile_null(
    PARSE   *parse,
    FRAME *frame )
{
    return L0;
}

/*
 * compile_on() - run rule under influence of on-target variables
 *
 * 	parse->left	list of files to include (can only do 1)
 *	parse->right	rule to run
 *
 * EXPERIMENTAL!
 */

LIST *
compile_on(
	PARSE	*parse,
	FRAME	*frame )
{
	LIST    *nt = parse_evaluate( parse->left, frame );
	LIST	*result = 0;

	if( DEBUG_COMPILE )
	{
	    debug_compile( 0, "on", frame );
	    list_print( nt );
	    printf( "\n" );
	}

	if( nt )
	{
	    TARGET *t = bindtarget( nt->string );
	    pushsettings( t->settings );

	    result = parse_evaluate( parse->right, frame );

	    popsettings( t->settings );
	}

	list_free( nt );

	return result;
}


/*
 * compile_rule() - compile a single user defined rule
 *
 *  parse->string   name of user defined rule
 *  parse->left parameters (list of lists) to rule, recursing left
 *
 * Wrapped around evaluate_rule() so that headers() can share it.
 */

LIST *
compile_rule(
    PARSE   *parse,
    FRAME *frame )
{
    FRAME       inner[1];
    LIST    *result;
    PARSE   *p;
    

    /* Build up the list of arg lists */

    frame_init( inner );
    inner->prev = frame;
    inner->module = frame->module; /* This gets fixed up in evaluate_rule(), below */
    inner->procedure = parse;

    for( p = parse->left; p; p = p->left )
        lol_add( inner->args, parse_evaluate( p->right, frame ) );

    /* And invoke rule */

    result = evaluate_rule( parse->string, inner );

    frame_free( inner );

    return result;
}

static void argument_error( char* message, RULE* rule, FRAME* frame, LIST* arg )
{
    LOL* actual = frame->args;
    assert( frame->procedure != 0 );
    backtrace_line( frame->prev );
    printf( "*** argument error\n* rule %s ( ", frame->rulename );
    lol_print( rule->arguments->data );
    printf( " )\n* called with: ( " );
    lol_print( actual );
    printf( " )\n* %s %s\n", message, arg ? arg->string : "" );
    print_source_line( rule->procedure );
    printf( "see definition of rule '%s' being called\n", rule->name );
    backtrace( frame->prev );
    exit(1);
}


/*
 * collect_arguments() - local argument checking and collection
 */
static SETTINGS *
collect_arguments( RULE* rule, FRAME* frame )
{
    SETTINGS *locals = 0;
    
    LOL* all_actual = frame->args;
    LOL *all_formal = rule->arguments ? rule->arguments->data : 0;
    if ( all_formal ) /* Nothing to set; nothing to check */
    {
        int max = all_formal->count > all_actual->count
            ? all_formal->count
            : all_actual->count;
        
        int n;
        for ( n = 0; n < max ; ++n )
        {
            LIST *formal = lol_get( all_formal, n );
            LIST *actual = lol_get( all_actual, n );
            
            while ( formal )
            {
                char* name = formal->string;
                char modifier = 0;
                LIST* value = 0;

                /* Stop now if a variable number of arguments are specified */
                if ( name[0] == '*' && name[1] == 0 )
                    return locals;
                
                if ( formal->next )
                {
                    char *next = formal->next->string;
                    if ( next && next[0] != 0 && next[1] == 0 )
                        modifier = next[0];
                }
                
                if ( !actual && modifier != '?' && modifier != '*' )
                {
                    argument_error( "missing argument", rule, frame, formal );
                }

                switch ( modifier )
                {
                case '+':
                case '*':
                    value = list_copy( 0, actual );
                    actual = 0;
                    /* skip an extra element for the modifier */
                    formal = formal->next; 
                    break;
                case '?':
                    /* skip an extra element for the modifier */
                    formal = formal->next; 
                    /* fall through */
                default:
                    if ( actual ) /* in case actual is missing */
                    {
                        value = list_new( 0, actual->string );
                        actual = actual->next;
                    }
                }
                
                locals = addsettings( locals, 0, name, value );
                formal = formal->next;
            }
            
            if ( actual )
            {
                argument_error( "extra argument", rule, frame, actual );
            }
        }
    }
    return locals;
}

struct profile_info
{
    char* name;                 /* name of rule being called */
    clock_t cumulative;         /* cumulative time spent in rule */
    clock_t net;                /* time spent in rule proper */
    unsigned long num_entries;  /* number of time rule was entered */
    unsigned long stack_count;  /* number of the times this function is present in stack */
};
typedef struct profile_info profile_info;

struct profile_frame
{
    profile_info* info;               /* permanent storage where data accumulates */
    clock_t overhead;                 /* overhead for profiling in this call */
    clock_t entry_time;               /* time of last entry to rule */
    struct profile_frame* caller;     /* stack frame of caller */
    clock_t subrules;                 /* time spent in subrules */
};
typedef struct profile_frame profile_frame;

static profile_frame* profile_stack = 0;
static struct hash* profile_hash = 0;

static void profile_enter( char* rulename, profile_frame* frame )
{
    clock_t start = clock();
    profile_info info, *p = &info;
    
    if ( !profile_hash )
        profile_hash = hashinit(sizeof(profile_info), "profile");

    info.name = rulename;
    
    if ( hashenter( profile_hash, (HASHDATA **)&p ) )
        p->cumulative = p->net = p->num_entries = p->stack_count = 0;

    ++(p->num_entries);
    ++(p->stack_count);
    
    frame->info = p;
    
    frame->caller = profile_stack;
    profile_stack = frame;

    frame->entry_time = clock();
    frame->overhead = 0;
    frame->subrules = 0;

    /* caller pays for the time it takes to play with the hash table */
    if ( frame->caller )
        frame->caller->overhead += frame->entry_time - start;
}
    
static void profile_exit(profile_frame* frame)
{
    /* cumulative time for this call */
    clock_t t = clock() - frame->entry_time - frame->overhead;
    /* If this rule is already present on the stack, don't add the time for
       this instance. */
    if (frame->info->stack_count == 1)
        frame->info->cumulative += t;
    /* Net time does not depend on presense of the same rule in call stack. */
    frame->info->net += t - frame->subrules;
        
    if (frame->caller)
    {
        /* caller's cumulative time must account for this overhead */
        frame->caller->overhead += frame->overhead;
        frame->caller->subrules += t;
    }
    /* pop this stack frame */
    --frame->info->stack_count;
    profile_stack = frame->caller;
}

static void dump_profile_entry(void* p_, void* ignored)
{
    profile_info* p = p_;
    printf("%10d %10d %10d %s\n", p->cumulative, p->net, p->num_entries, p->name);
}

void profile_dump()
{
    if ( profile_hash )
    {
        printf("%10s %10s %10s %s\n", "gross", "net", "# entries", "name");
        hashenumerate( profile_hash, dump_profile_entry, 0 );
    }
}

/*
 * evaluate_rule() - execute a rule invocation
 */

LIST *
evaluate_rule(
    char    *rulename,
    FRAME *frame )
{
    LIST      *result = L0;
    RULE          *rule;
    profile_frame prof[1];
    module    *prev_module = frame->module;
    
    LIST      *l;
    {
        LOL arg_context_, *arg_context = &arg_context_;
        if ( !frame->prev )
            lol_init(arg_context);
        else
            arg_context = frame->prev->args;
        
        l = var_expand( L0, rulename, rulename+strlen(rulename), arg_context, 0 );
    }

    if ( !l )
    {
        backtrace_line( frame->prev );
        printf( "warning: rulename %s expands to empty string\n", rulename );
        backtrace( frame->prev );
        return result;
    }

    rulename = l->string;
    rule = bindrule( l->string, frame->module );

    /* drop the rule name */
    l = list_pop_front( l );

    /* tack the rest of the expansion onto the front of the first argument */
    frame->args->list[0] = list_append( l, lol_get( frame->args, 0 ) );

    if ( DEBUG_COMPILE )
    {
        /* Try hard to indicate in which module the rule is going to execute */
        if ( rule->module != frame->module
             && rule->procedure != 0 && strcmp(rulename, rule->procedure->rulename) )
        {
            char buf[256] = "";
            strncat( buf, rule->module->name, sizeof(buf) - 1 );
            strncat( buf, rule->name, sizeof(buf) - 1 );
            debug_compile( 1, buf, frame);
        }
        else
        {
            debug_compile( 1, rulename, frame);
        }

        lol_print( frame->args );
        printf( "\n" );
    }
    
    if ( rule->procedure && rule->module != prev_module )
    {
        /* propagate current module to nested rule invocations */
        frame->module = rule->module;
        
        /* swap variables */
        exit_module( prev_module );
        enter_module( rule->module );
    }
        
    /* record current rule name in frame */
    if ( rule->procedure )
    {
        frame->rulename = rulename;
        /* and enter record profile info */
        if ( DEBUG_PROFILE )
            profile_enter( rule->procedure->rulename, prof );
    }

    /* Check traditional targets $(<) and sources $(>) */

    if( !rule->actions && !rule->procedure )
    {
        backtrace_line( frame->prev );
        printf( "warning: unknown rule %s\n", rule->name );
        backtrace( frame->prev );
    }

    /* If this rule will be executed for updating the targets */
    /* then construct the action for make(). */

    if( rule->actions )
    {
        TARGETS *t;
        ACTION  *action;

        /* The action is associated with this instance of this rule */

        action = (ACTION *)malloc( sizeof( ACTION ) );
        memset( (char *)action, '\0', sizeof( *action ) );

        action->rule = rule;
        action->targets = targetlist( (TARGETS *)0, lol_get( frame->args, 0 ) );
        action->sources = targetlist( (TARGETS *)0, lol_get( frame->args, 1 ) );

        /* Append this action to the actions of each target */

        for( t = action->targets; t; t = t->next )
            t->target->actions = actionlist( t->target->actions, action );
    }

    /* Now recursively compile any parse tree associated with this rule */
    /* refer/free to ensure rule not freed during use */

    if( rule->procedure )
    {
        SETTINGS *local_args = collect_arguments( rule, frame );
        PARSE *parse = rule->procedure;
        parse_refer( parse );
        
        pushsettings( local_args );
        result = parse_evaluate( parse, frame );
        popsettings( local_args );
        freesettings( local_args );
        
        parse_free( parse );
    }

    if ( frame->module != prev_module )
    {
        exit_module( frame->module );
        enter_module( prev_module );
    }

    if ( DEBUG_PROFILE && rule->procedure )
        profile_exit( prof );

    if( DEBUG_COMPILE )
        debug_compile( -1, 0, frame);

    return result;
}

/*
 * compile_rules() - compile a chain of rules
 *
 *	parse->left	single rule
 *	parse->right	more compile_rules() by right-recursion
 */

LIST *
compile_rules(
    PARSE   *parse,
    FRAME *frame )
{
    /* Ignore result from first statement; return the 2nd. */
	/* Optimize recursion on the right by looping. */

    do list_free( parse_evaluate( parse->left, frame ) );
    while( (parse = parse->right)->func == compile_rules );

    return parse_evaluate( parse, frame );
}

/*
 * compile_set() - compile the "set variable" statement
 *
 *  parse->left variable names
 *  parse->right    variable values 
 *  parse->num  ASSIGN_SET/APPEND/DEFAULT
 */

LIST *
compile_set(
    PARSE   *parse,
    FRAME *frame )
{
    LIST    *nt = parse_evaluate( parse->left, frame );
    LIST    *ns = parse_evaluate( parse->right, frame );
    LIST    *l;
    int     setflag;
    char    *trace;

    switch( parse->num )
    {
    case ASSIGN_SET:    setflag = VAR_SET; trace = "="; break;
    case ASSIGN_APPEND: setflag = VAR_APPEND; trace = "+="; break;
    case ASSIGN_DEFAULT:    setflag = VAR_DEFAULT; trace = "?="; break;
    default:        setflag = VAR_SET; trace = ""; break;
    }

    if( DEBUG_COMPILE )
    {
        debug_compile( 0, "set", frame);
        list_print( nt );
        printf( " %s ", trace );
        list_print( ns );
        printf( "\n" );
    }

    /* Call var_set to set variable */
    /* var_set keeps ns, so need to copy it */

    for( l = nt; l; l = list_next( l ) )
        var_set( l->string, list_copy( L0, ns ), setflag );

    list_free( nt );

    return ns;
}

/*
 * compile_setcomp() - support for `rule` - save parse tree 
 *
 *  parse->string   rule name
 *  parse->left rules for rule
 *  parse->right optional list-of-lists describing arguments
 */

LIST *
compile_setcomp(
    PARSE   *parse,
    FRAME *frame)
{
    argument_list* arg_list = 0;
    
    /* Create new LOL describing argument requirements if supplied */
    if ( parse->right )
    {
        PARSE *p;
        arg_list = args_new();
        for( p = parse->right; p; p = p->left )
            lol_add( arg_list->data, parse_evaluate( p->right, frame ) );
    }
    
    new_rule_body( frame->module, parse->string, arg_list, parse->left, !parse->num );
    return L0;
}

/*
 * compile_setexec() - support for `actions` - save execution string 
 *
 *  parse->string   rule name
 *  parse->string1  OS command string
 *  parse->num  flags
 *  parse->left `bind` variables
 *
 * Note that the parse flags (as defined in compile.h) are transfered
 * directly to the rule flags (as defined in rules.h).
 */

LIST *
compile_setexec(
    PARSE   *parse,
    FRAME *frame )
{
    LIST* bindlist = parse_evaluate( parse->left, frame );

    new_rule_actions( frame->module, parse->string, parse->string1, bindlist, parse->num );

    return L0;
}

/*
 * compile_settings() - compile the "on =" (set variable on exec) statement
 *
 *  parse->left variable names
 *  parse->right    target name 
 *  parse->third    variable value 
 *  parse->num  ASSIGN_SET/APPEND   
 */

LIST *
compile_settings(
    PARSE   *parse,
    FRAME *frame )
{
    LIST    *nt = parse_evaluate( parse->left, frame );
    LIST    *ns = parse_evaluate( parse->third, frame );
    LIST    *targets = parse_evaluate( parse->right, frame );
    LIST    *ts;
    int append = parse->num == ASSIGN_APPEND;

    if( DEBUG_COMPILE )
    {
        debug_compile( 0, "set", frame);
        list_print( nt );
        printf( " on " );
        list_print( targets );
        printf( " %s ", append ? "+=" : "=" );
        list_print( ns );
        printf( "\n" );
    }

    /* Call addsettings to save variable setting */
    /* addsettings keeps ns, so need to copy it */
    /* Pass append flag to addsettings() */

    for( ts = targets; ts; ts = list_next( ts ) )
    {
        TARGET  *t = bindtarget( ts->string );
        LIST    *l;

        for( l = nt; l; l = list_next( l ) )
        t->settings = addsettings( t->settings, append, 
                l->string, list_copy( (LIST*)0, ns ) );
    }

    list_free( nt );
    list_free( targets );

    return ns;
}

/*
 * compile_switch() - compile 'switch' rule
 *
 *  parse->left switch value (only 1st used)
 *  parse->right    cases
 *
 *  cases->left 1st case
 *  cases->right    next cases
 *
 *  case->string    argument to match
 *  case->left  parse tree to execute
 */

LIST *
compile_switch(
    PARSE   *parse,
    FRAME *frame )
{
    LIST    *nt = parse_evaluate( parse->left, frame );
    LIST    *result = 0;

    if( DEBUG_COMPILE )
    {
        debug_compile( 0, "switch", frame);
        list_print( nt );
        printf( "\n" );
    }

    /* Step through cases */

    for( parse = parse->right; parse; parse = parse->right )
    {
        if( !glob( parse->left->string, nt ? nt->string : "" ) )
        {
        /* Get & exec parse tree for this case */
        parse = parse->left->left;
        result = parse_evaluate( parse, frame );
        break;
        }
    }

    list_free( nt );

    return result;
}

/*
 * debug_compile() - printf with indent to show rule expansion.
 */

static void
debug_compile( int which, char *s, FRAME* frame )
{
    static int level = 0;
    static char indent[36] = ">>>>|>>>>|>>>>|>>>>|>>>>|>>>>|>>>>|";

    if ( which >= 0 )
    {
      int i;
      
      print_source_line( frame->procedure );
      
      i = (level+1)*2;
      while ( i > 35 )
      {
        printf( indent );
        i -= 35;
      }

      printf( "%*.*s ", i, i, indent );
    }

    if( s )
        printf( "%s ", s );

    level += which;
}

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

/*
 * make1.c - execute command to bring targets up to date
 *
 * This module contains make1(), the entry point called by make() to 
 * recursively decend the dependency graph executing update actions as
 * marked by make0().
 *
 * External routines:
 *
 *	make1() - execute commands to update a TARGET and all its dependents
 *
 * Internal routines, the recursive/asynchronous command executors:
 *
 *	make1a() - recursively traverse target tree, calling make1b()
 *	make1b() - dependents of target built, now build target with make1c()
 *	make1c() - launch target's next command, call make1b() when done
 *	make1d() - handle command execution completion and call back make1c()
 *
 * Internal support routines:
 *
 *	make1cmds() - turn ACTIONS into CMDs, grouping, splitting, etc
 *	make1list() - turn a list of targets into a LIST, for $(<) and $(>)
 * 	make1settings() - for vars that get bound values, build up replacement lists
 * 	make1bind() - bind targets that weren't bound in dependency analysis
 *
 * 04/16/94 (seiwald) - Split from make.c.
 * 04/21/94 (seiwald) - Handle empty "updated" actions.
 * 05/04/94 (seiwald) - async multiprocess (-j) support
 * 06/01/94 (seiwald) - new 'actions existing' does existing sources
 * 12/20/94 (seiwald) - NOTIME renamed NOTFILE.
 * 01/19/95 (seiwald) - distinguish between CANTFIND/CANTMAKE targets.
 * 01/22/94 (seiwald) - pass per-target JAMSHELL down to execcmd().
 * 02/28/95 (seiwald) - Handle empty "existing" actions.
 * 03/10/95 (seiwald) - Fancy counts.
 */

# include "jam.h"

# include "lists.h"
# include "parse.h"
# include "assert.h"
# include "variable.h"
# include "rules.h"

# include "search.h"
# include "newstr.h"
# include "make.h"
# include "command.h"
# include "execcmd.h"

static CMD *make1cmds( TARGET *t );
static LIST *make1list( LIST *l, TARGETS *targets, int flags );
static SETTINGS *make1settings( LIST *vars );
static void make1bind( TARGET *t, int warn );

/* Ugly static - it's too hard to carry it through the callbacks. */

static struct {
	int	failed;
	int	skipped;
	int	total;
	int	made;
} counts[1] ;

/*
 * Target state - remove recursive calls by just keeping track of state target is in
 */
typedef struct _state
{
  struct _state *prev; /* previous state on stack */
  TARGET *t; /* current target */
  TARGET *parent; /* parent argument necessary for make1a() */
#define T_STATE_MAKE1A 0 /* make1a() should be called */
#define T_STATE_MAKE1ATAIL 1 /* make1atail() should be called */
#define T_STATE_MAKE1B 2 /* make1b() should be called */
#define T_STATE_MAKE1C 3 /* make1c() should be called */
#define T_STATE_MAKE1D 4 /* make1d() should be called */
  int curstate; /* current state */
  int status;
} state;

static void make1a( state *pState);
static void make1atail(state *pState);
static void make1b( state *pState );
static void make1c( state *pState );
static void make1d( state *pState );
static void make_closure(void *closure, int status);

typedef struct _stack
{
	state *stack;
} stack;

static stack state_stack = { NULL };

static state *state_freelist = NULL;

static state *alloc_state()
{
	if(state_freelist != NULL)
	{
		state *pState;

		pState = state_freelist;
		state_freelist = pState->prev;
		memset(pState, 0, sizeof(state));
		return pState;
	}
	else
	{
		return (state *)malloc(sizeof(state));
	}
}

static void free_state(state *pState)
{
	pState->prev = state_freelist;
	state_freelist = pState;
}

static void clear_state_freelist()
{
	while(state_freelist != NULL)
	{
		state *pState = state_freelist;
		state_freelist = state_freelist->prev;
		free(pState);
	}
}

static state *current_state(stack *pStack)
{
	return pStack->stack;
}

static void pop_state(stack *pStack)
{
	state *pState;

	if(pStack->stack != NULL)
	{
		pState = pStack->stack->prev;
		free_state(pStack->stack);
		pStack->stack = pState;
	}
}

static state *push_state(stack *pStack, TARGET *t, TARGET *parent, int curstate)
{
	state *pState;

	pState = alloc_state();

	pState->t = t;
	pState->parent = parent;
	pState->prev = pStack->stack;
	pState->curstate = curstate;

	pStack->stack = pState;

	return pStack->stack;
}

/* pushes a stack onto another stack, effectively reversing the order */
static void push_stack_on_stack(stack *pDest, stack *pSrc)
{
	while(pSrc->stack != NULL)
	{
		state *pState;

		pState = pSrc->stack;
		pSrc->stack = pSrc->stack->prev;
		pState->prev = pDest->stack;
		pDest->stack = pState;
	}
}

/*
 * make1() - execute commands to update a TARGET and all its dependents
 */

static int intr = 0;

int
make1( TARGET *t )
{
	state *pState;

	memset( (char *)counts, 0, sizeof( *counts ) );

	/* Recursively make the target and its dependents */
	push_state(&state_stack, t, NULL, T_STATE_MAKE1A);

	do
	{
		while((pState = current_state(&state_stack)) != NULL)
		{
            if (intr) 
                pop_state(&state_stack);

			switch(pState->curstate)
			{
			case T_STATE_MAKE1A:
				make1a(pState);
				break;
			case T_STATE_MAKE1ATAIL:
				make1atail(pState);
				break;
			case T_STATE_MAKE1B:
				make1b(pState);
				break;
			case T_STATE_MAKE1C:
				make1c(pState);
				break;
			case T_STATE_MAKE1D:
				make1d(pState);
				break;
			default:
				break;
			}
		}
	

	/* Wait for any outstanding commands to finish running. */
	} while( execwait() );

	clear_state_freelist();

	/* Talk about it */
	if( counts->failed )
	    printf( "...failed updating %d target%s...\n", counts->failed,
		        counts->failed > 1 ? "s" : "" );

	if( DEBUG_MAKE && counts->skipped )
	    printf( "...skipped %d target%s...\n", counts->skipped,
		        counts->skipped > 1 ? "s" : "" );

	if( DEBUG_MAKE && counts->made )
	    printf( "...updated %d target%s...\n", counts->made,
		        counts->made > 1 ? "s" : "" );

	return counts->total != counts->made;
}

/*
 * make1a() - recursively traverse target tree, calling make1b()
 */

static void
make1a( state *pState)
{
    TARGET* t = pState->t;
	TARGETS	*c;
	int i;

	/* If the parent is the first to try to build this target */
	/* or this target is in the make1c() quagmire, arrange for the */
	/* parent to be notified when this target is built. */

	if( pState->parent )
	    switch( pState->t->progress )
	{
	case T_MAKE_INIT:
	case T_MAKE_ACTIVE:
	case T_MAKE_RUNNING:
	    pState->t->parents = targetentry( pState->t->parents, pState->parent );
	    pState->parent->asynccnt++;
	}

	if( pState->t->progress != T_MAKE_INIT )
	{
		pop_state(&state_stack);
		return;
	}

	/* Asynccnt counts the dependents preventing this target from */
	/* proceeding to make1b() for actual building.  We start off with */
	/* a count of 1 to prevent anything from happening until we can */
	/* call all dependents.  This 1 is accounted for when we call */
	/* make1b() ourselves, below. */

	pState->t->asynccnt = 1;

	/* against circular dependency. */

	pState->t->progress = T_MAKE_ONSTACK;

	{
		stack temp_stack = { NULL };
		for( i = T_DEPS_DEPENDS; i <= T_DEPS_INCLUDES; i++ )
			for( c = pState->t->deps[i]; c && !intr; c = c->next )
			{
				push_state(&temp_stack, c->target, pState->t, T_STATE_MAKE1A);
			}

		/* using stacks reverses the order of execution. Reverse it back */
		push_stack_on_stack(&state_stack, &temp_stack);
	}

	pState->curstate = T_STATE_MAKE1ATAIL;
}

static void make1atail(state *pState)
{
	pState->t->progress = T_MAKE_ACTIVE;

	/* Now that all dependents have bumped asynccnt, we now allow */
	/* decrement our reference to asynccnt. */ 
	pState->curstate = T_STATE_MAKE1B;
}

/*
 * make1b() - dependents of target built, now build target with make1c()
 */

static void
make1b( state *pState )
{
    TARGET      *t = pState->t;
    TARGETS     *c;
    int         i;
    char        *failed = "dependents";

    /* If any dependents are still outstanding, wait until they */
    /* call make1b() to signal their completion. */

    if( --(pState->t->asynccnt) )
	{
		pop_state(&state_stack);
		return;
	}
    
    /* Try to aquire a semaphore. If it's locked, wait until the target
       that locked it is build and signals completition. */
#ifdef OPT_SEMAPHORE
	if( t->semaphore && t->semaphore->asynccnt )
	{
        /* Append 't' to the list of targets waiting on semaphore. */
	    t->semaphore->parents = targetentry( t->semaphore->parents, t );
	    t->asynccnt++;

	    if( DEBUG_EXECCMD )
		printf( "SEM: %s is busy, delaying launch of %s\n",
			t->semaphore->name, t->name);
		pop_state(&state_stack);
	    return;
	}
#endif


    /* Now ready to build target 't'... if dependents built ok. */

    /* Collect status from dependents */

    for( i = T_DEPS_DEPENDS; i <= T_DEPS_INCLUDES; i++ )
        for( c = pState->t->deps[i]; c; c = c->next )
            if( c->target->status > pState->t->status && !( c->target->flags & T_FLAG_NOCARE ) )
            {
                failed = c->target->name;
                pState->t->status = c->target->status;
            }

    /* If actions on deps have failed, bail. */
    /* Otherwise, execute all actions to make target */

    if( pState->t->status == EXEC_CMD_FAIL && pState->t->actions )
    {
        ++counts->skipped;
        if ( ( pState->t->flags & ( T_FLAG_RMOLD | T_FLAG_NOTFILE ) ) == T_FLAG_RMOLD )
        {
            if( !unlink( pState->t->boundname ) )
                printf( "...removing outdated %s\n", pState->t->boundname );
        }
        else
        printf( "...skipped %s for lack of %s...\n", pState->t->name, failed );
    }

    if( pState->t->status == EXEC_CMD_OK )
        switch( pState->t->fate )
        {
        case T_FATE_INIT:
        case T_FATE_MAKING:
            /* shouldn't happen */

        case T_FATE_STABLE:
        case T_FATE_NEWER:
            break;

        case T_FATE_CANTFIND:
        case T_FATE_CANTMAKE:
            pState->t->status = EXEC_CMD_FAIL;
            break;

        case T_FATE_ISTMP:
            if( DEBUG_MAKE )
                printf( "...using %s...\n", pState->t->name );
            break;

        case T_FATE_TOUCHED:
        case T_FATE_MISSING:
        case T_FATE_OUTDATED:
        case T_FATE_UPDATE:

            /* Set "on target" vars, build actions, unset vars */
            /* Set "progress" so that make1c() counts this target among */
            /* the successes/failures. */

            if( pState->t->actions )
            {
                ++counts->total;
                if( DEBUG_MAKE && !( counts->total % 100 ) )
                    printf( "...on %dth target...\n", counts->total );

                pState->t->cmds = (char *)make1cmds( pState->t );
                pState->t->progress = T_MAKE_RUNNING;
            }

            break;
        }

		/* Call make1c() to begin the execution of the chain of commands */
		/* needed to build target.  If we're not going to build target */
		/* (because of dependency failures or because no commands need to */
		/* be run) the chain will be empty and make1c() will directly */
		/* signal the completion of target. */

	/* Recurse on our dependents, manipulating progress to guard */

#ifdef OPT_SEMAPHORE
	/* If there is a semaphore, indicate that its in use */
	if( pState->t->semaphore )
	{
	    ++(pState->t->semaphore->asynccnt);

	    if( DEBUG_EXECCMD )
		printf( "SEM: %s now used by %s\n", pState->t->semaphore->name,
		       pState->t->name );
	}
#endif

	pState->curstate = T_STATE_MAKE1C;
}

/*
 * make1c() - launch target's next command, call make1b() when done
 */

static void
make1c( state *pState )
{
	CMD	*cmd = (CMD *)pState->t->cmds;

	/* If there are (more) commands to run to build this target */
	/* (and we haven't hit an error running earlier comands) we */
	/* launch the command with execcmd(). */
	
	/* If there are no more commands to run, we collect the status */
	/* from all the actions then report our completion to all the */
	/* parents. */

	if( cmd && pState->t->status == EXEC_CMD_OK )
	{
		if( DEBUG_MAKEQ || 
            ! ( cmd->rule->actions->flags & RULE_QUIETLY ) && DEBUG_MAKE)
	    {
		printf( "%s ", cmd->rule->name );
		list_print( lol_get( &cmd->args, 0 ) );
		printf( "\n" );
	    }

	    if( DEBUG_EXEC )
		printf( "%s\n", cmd->buf );

	    if( globs.cmdout )
		fprintf( globs.cmdout, "%s", cmd->buf );

	    if( globs.noexec )
	    {
			pState->curstate = T_STATE_MAKE1D;
			pState->status = EXEC_CMD_OK;
	    } 
	    else
	    {
			TARGET *t = pState->t;
			fflush( stdout );

			pop_state(&state_stack); /* pop state first because execcmd could push state */
			execcmd( cmd->buf, make_closure, t, cmd->shell );
	    }
	}
	else
	{
	    TARGETS	*c;
	    ACTIONS	*actions;

	    /* Collect status from actions, and distribute it as well */

	    for( actions = pState->t->actions; actions; actions = actions->next )
		if( actions->action->status > pState->t->status )
		    pState->t->status = actions->action->status;

	    for( actions = pState->t->actions; actions; actions = actions->next )
		if( pState->t->status > actions->action->status )
		    actions->action->status = pState->t->status;

	    /* Tally success/failure for those we tried to update. */

	    if( pState->t->progress == T_MAKE_RUNNING )
		switch( pState->t->status )
	    {
	    case EXEC_CMD_OK:
		++counts->made;
		break;
	    case EXEC_CMD_FAIL:
		++counts->failed;
		break;
	    }

	    /* Tell parents dependent has been built */
		{
			stack temp_stack = { NULL };
			TARGET *t = pState->t;

			t->progress = T_MAKE_DONE;

			for( c = t->parents; c; c = c->next )
				push_state(&temp_stack, c->target, NULL, T_STATE_MAKE1B);

#ifdef OPT_SEMAPHORE
	    /* If there is a semaphore, its now free */
	    if( t->semaphore )
	    {
		assert( t->semaphore->asynccnt == 1 );
		--(t->semaphore->asynccnt);

		if( DEBUG_EXECCMD )
		    printf( "SEM: %s is now free\n", t->semaphore->name);

		/* If anything is waiting, notify the next target. There's no
            point in notifying all waiting targets, since they'll be
            serialized again. */
		if( t->semaphore->parents )
		{
		    TARGETS *first = t->semaphore->parents;
		    if( first->next )
			first->next->tail = first->tail;
		    t->semaphore->parents = first->next;

		    if( DEBUG_EXECCMD )
			printf( "SEM: placing %s on stack\n", first->target->name);
            push_state(&temp_stack, first->target, NULL, T_STATE_MAKE1B);
		    free( first );
		}
	    }
#endif

		
			/* must pop state before pushing any more */
			pop_state(&state_stack);
		
			/* using stacks reverses the order of execution. Reverse it back */
			push_stack_on_stack(&state_stack, &temp_stack);

		}
	}
}

static void make_closure(void *closure, int status)
{
	push_state(&state_stack, (TARGET *)closure, NULL, T_STATE_MAKE1D)->status = status;
}

/*
 * make1d() - handle command execution completion and call back make1c()
 */

static void
make1d(state *pState)
{
	TARGET	*t = pState->t;
	CMD	*cmd = (CMD *)t->cmds;
	int status = pState->status;

	/* Execcmd() has completed.  All we need to do is fiddle with the */
	/* status and signal our completion so make1c() can run the next */
	/* command.  On interrupts, we bail heavily. */

        if ( t->flags & T_FLAG_FAIL_EXPECTED )
        {
          /* invert execution result when FAIL_EXPECTED was applied */
          switch (status)
          {
            case EXEC_CMD_FAIL: status = EXEC_CMD_OK; break;
            case EXEC_CMD_OK:   status = EXEC_CMD_FAIL; break;
            default:
              ;
          }
        }
        
	if( status == EXEC_CMD_FAIL && ( cmd->rule->actions->flags & RULE_IGNORE ) )
	    status = EXEC_CMD_OK;

	/* On interrupt, set intr so _everything_ fails */

	if( status == EXEC_CMD_INTR )
	    ++intr;

	if( status == EXEC_CMD_FAIL && DEBUG_MAKE )
	{
	    /* Print command text on failure */

	    if( !DEBUG_EXEC )
		printf( "%s\n", cmd->buf );

	    printf( "...failed %s ", cmd->rule->name );
	    list_print( lol_get( &cmd->args, 0 ) );
	    printf( "...\n" );
	}

	if (status == EXEC_CMD_FAIL)
		if( globs.quitquick ) ++intr;

	/* If the command was interrupted or failed and the target */
	/* is not "precious", remove the targets */

	if( status != EXEC_CMD_OK && !( cmd->rule->actions->flags & RULE_TOGETHER ) )
	{
	    LIST *targets = lol_get( &cmd->args, 0 );

	    for( ; targets; targets = list_next( targets ) )
		if( !unlink( targets->string ) )
		    printf( "...removing %s\n", targets->string );
	}

	/* Free this command and call make1c() to move onto next command. */

	t->status = status;
	t->cmds = (char *)cmd_next( cmd );

	cmd_free( cmd );

	pState->curstate = T_STATE_MAKE1C;
}

/*
 * swap_settings() - replace the settings from the current module and
 *                   target with those from the new module and target
 */
static void swap_settings(
    module** current_module
    , TARGET** current_target
    , module* new_module
    , TARGET* new_target)
{
    if (new_module == root_module())
        new_module = 0;
    
    if (new_target == *current_target && new_module == *current_module)
        return;

    if (*current_target)
        popsettings( (*current_target)->settings );
        
    if (new_module != *current_module)
    {
        if (*current_module)
            exit_module( *current_module );

        *current_module = new_module;
        
        if (new_module)
            enter_module( new_module );
    }

    *current_target = new_target;
    if (new_target)
        pushsettings( new_target->settings );
}

/*
 * make1cmds() - turn ACTIONS into CMDs, grouping, splitting, etc
 *
 * Essentially copies a chain of ACTIONs to a chain of CMDs, 
 * grouping RULE_TOGETHER actions, splitting RULE_PIECEMEAL actions,
 * and handling RULE_NEWSRCS actions.  The result is a chain of
 * CMDs which can be expanded by var_string() and executed with
 * execcmd().
 */

static CMD *
make1cmds( TARGET *t )
{
	CMD *cmds = 0;
	LIST *shell = 0;
        
        module *settings_module = 0;
        TARGET *settings_target = 0;
        
	/* Step through actions */
	/* Actions may be shared with other targets or grouped with */
	/* RULE_TOGETHER, so actions already seen are skipped. */
        
        ACTIONS* a0;
	for(a0 = t->actions ; a0; a0 = a0->next )
	{
	    RULE    *rule = a0->action->rule;
            rule_actions *actions = rule->actions;
	    SETTINGS *boundvars;
	    LIST    *nt, *ns;
	    ACTIONS *a1;
	    int	    start, chunk, length;

	    /* Only do rules with commands to execute. */
	    /* If this action has already been executed, use saved status */

	    if( !actions || a0->action->running )
		continue;

	    a0->action->running = 1;
	    
	    /* Make LISTS of targets and sources */
	    /* If `execute together` has been specified for this rule, tack */
	    /* on sources from each instance of this rule for this target. */

	    nt = make1list( L0, a0->action->targets, 0 );
	    ns = make1list( L0, a0->action->sources, actions->flags );

	    if( actions->flags & RULE_TOGETHER )
		for( a1 = a0->next; a1; a1 = a1->next )
		    if( a1->action->rule == rule && !a1->action->running )
	    {
		ns = make1list( ns, a1->action->sources, actions->flags );
		a1->action->running = 1;
	    }

	    /* If doing only updated (or existing) sources, but none have */
	    /* been updated (or exist), skip this action. */

	    if( !ns && ( actions->flags & ( RULE_NEWSRCS | RULE_EXISTING ) ) )
	    {
		list_free( nt );
		continue;
	    }

            swap_settings( &settings_module, &settings_target, rule->module, t );
            if (!shell)
                shell = var_get( "JAMSHELL" );	/* shell is per-target */
                
	    /* If we had 'actions xxx bind vars' we bind the vars now */

	    boundvars = make1settings( actions->bindlist );
	    pushsettings( boundvars );

	    /*
	     * Build command, starting with all source args. 
	     *
	     * If cmd_new returns 0, it's because the resulting command
	     * length is > MAXLINE.  In this case, we'll slowly reduce
	     * the number of source arguments presented until it does
	     * fit.  This only applies to actions that allow PIECEMEAL 
	     * commands.
	     *
	     * While reducing slowly takes a bit of compute time to get
	     * things just right, it's worth it to get as close to MAXLINE
	     * as possible, because launching the commands we're executing 
	     * is likely to be much more compute intensive!
	     *
	     * Note we loop through at least once, for sourceless actions.
	     */

	    start = 0;
	    chunk = length = list_length( ns );

	    do
	    {
		/* Build cmd: cmd_new consumes its lists. */

		CMD *cmd = cmd_new( rule, 
			list_copy( L0, nt ), 
			list_sublist( ns, start, chunk ),
			list_copy( L0, shell ) );

		if( cmd )
		{
		    /* It fit: chain it up. */

		    if( !cmds ) cmds = cmd;
		    else cmds->tail->next = cmd;
		    cmds->tail = cmd;
		    start += chunk;
		}
		else if( ( actions->flags & RULE_PIECEMEAL ) && chunk > 1 )
		{
		    /* Reduce chunk size slowly. */

		    chunk = chunk * 9 / 10;
		}
		else
		{
		    /* Too long and not splittable. */

		    printf( "%s actions too long (max %d):\n", 
			rule->name, MAXLINE );

                    /* Tell the user what didn't fit */
                    cmd = cmd_new(
                        rule, list_copy( L0, nt ), 
			list_sublist( ns, start, chunk ),
			list_new( L0, newstr( "%" ) ) );

                    printf( cmd->buf );
                
		    exit( EXITBAD );
		}
	    }
	    while( start < length );

	    /* These were always copied when used. */

	    list_free( nt );
	    list_free( ns );

	    /* Free the variables whose values were bound by */
	    /* 'actions xxx bind vars' */

	    popsettings( boundvars );
	    freesettings( boundvars );
	}

        swap_settings( &settings_module, &settings_target, 0, 0 );
	return cmds;
}

/*
 * make1list() - turn a list of targets into a LIST, for $(<) and $(>)
 */

static LIST *
make1list( 
	LIST	*l,
	TARGETS	*targets,
	int	flags )
{
    for( ; targets; targets = targets->next )
    {
	TARGET *t = targets->target;

	/* Sources to 'actions existing' are never in the dependency */
	/* graph (if they were, they'd get built and 'existing' would */
	/* be superfluous, so throttle warning message about independent */
	/* targets. */

	if( t->binding == T_BIND_UNBOUND )
	    make1bind( t, !( flags & RULE_EXISTING ) );

	if( ( flags & RULE_EXISTING ) && t->binding != T_BIND_EXISTS )
	    continue;

	if( ( flags & RULE_NEWSRCS ) && t->fate <= T_FATE_STABLE )
	    continue;

	/* Prohibit duplicates for RULE_TOGETHER */

	if( flags & RULE_TOGETHER )
	{
	    LIST *m;

	    for( m = l; m; m = m->next )
		if( !strcmp( m->string, t->boundname ) )
		    break;

	    if( m )
		continue;
	}

	/* Build new list */

	l = list_new( l, copystr( t->boundname ) );
    }

    return l;
}

/*
 * make1settings() - for vars that get bound values, build up replacement lists
 */

static SETTINGS *
make1settings( LIST *vars )
{
	SETTINGS *settings = 0;

	for( ; vars; vars = list_next( vars ) )
	{
	    LIST *l = var_get( vars->string );
	    LIST *nl = 0;

	    for( ; l; l = list_next( l ) ) 
	    {
		TARGET *t = bindtarget( l->string );

		/* Make sure the target is bound, warning if it is not in the */
		/* dependency graph. */

		if( t->binding == T_BIND_UNBOUND )
		    make1bind( t, 1 );

		/* Build new list */

		nl = list_new( nl, copystr( t->boundname ) );
	    }

	    /* Add to settings chain */

	    settings = addsettings( settings, 0, vars->string, nl );
	}

	return settings;
}

/*
 * make1bind() - bind targets that weren't bound in dependency analysis
 *
 * Spot the kludge!  If a target is not in the dependency tree, it didn't 
 * get bound by make0(), so we have to do it here.  Ugly.
 */

static void
make1bind( 
	TARGET	*t,
	int	warn )
{
	if( t->flags & T_FLAG_NOTFILE )
	    return;

	/* Sources to 'actions existing' are never in the dependency */
	/* graph (if they were, they'd get built and 'existing' would */
	/* be superfluous, so throttle warning message about independent */
	/* targets. */

	if( warn )
	    printf( "warning: using independent target %s\n", t->name );

	pushsettings( t->settings );
	t->boundname = search( t->name, &t->time );
	t->binding = t->time ? T_BIND_EXISTS : T_BIND_MISSING;
	popsettings( t->settings );
}

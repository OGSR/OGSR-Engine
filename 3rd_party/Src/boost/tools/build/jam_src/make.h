/*
 * Copyright 1993, 1995 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

/*
 * make.h - bring a target up to date, once rules are in place
 */

#include "lists.h"

int make( int n_targets, char **targets, int anyhow );
int make1( TARGET *t );

/*
 * Specifies that the target should be updated.
 */
void mark_target_for_updating(char *target);
/* 
 * Returns the list of all the target previously passed to 'mark_target_for_updating'.
 */
LIST *targets_to_update();
/*
 * Cleasr/unmarks all targets that are currently marked for update.
 */
void clear_targets_to_update();

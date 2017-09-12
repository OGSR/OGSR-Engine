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
 * filesys.h - OS specific file routines 
 */

#ifndef FILESYS_DWA20011025_H
# define FILESYS_DWA20011025_H

typedef void (*scanback)( void *closure, char *file, int found, time_t t );

void file_dirscan( char *dir, scanback func, void *closure );
void file_archscan( char *arch, scanback func, void *closure );

int file_time( char *filename, time_t *time );

#endif

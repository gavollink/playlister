/****************************************************************************
 * track_storage.c
 *
 * Track information hash management (memory storage)
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see LICENSE file in this distribution.
 */
#define TRACK_STORAGE_C 1
#include <stdio.h>
#include <stdlib.h> // malloc, realloc
#include <stdarg.h> // va_args (wrapping fprintf)
#include <sys/stat.h> // stat
#include <sys/errno.h> // errno
#include <string.h> // strerror
#include <regex.h> // POSIX Regular Expressions
#include "utils.h"
#include "options.h"
#include "storage.h"

struct trackmap    *track = NULL;

void
_set_track(int trid,  char* name, char* value)
{
    struct trackmap *work;
    HASH_FIND_INT(track, &trid, work);
    if ( NULL == work ) {
        work = malloc( sizeof(struct trackmap) );
        if ( NULL == work ) {
            mydebug("st:Unable to allocate %ld bytes of space: %s\n",
                    sizeof(struct trackmap), strerror(errno));
            exit(-2);
        }
        memset((void *)work, 0, sizeof(struct trackmap));
        work->id = trid;
        HASH_ADD_INT(track, id, work);
        Stats.tracks++;
    }
    if (0 == str_diffn("Name", name, 5) ) {
        strncpy(work->name, value, 1024);
    }
    else if (0 == str_diffn("Total Time", name, 11) ) {
        work->time = atoi(value);
    }
    else if (0 == str_diffn("Location", name, 6) ) {
        URIunescape(value);
        strncpy(work->file, value, 1024);
    }
    else if (0 == str_diffn("Album", name, 6) ) {
        strncpy(work->album, value, 1024);
    }
    else if (0 == str_diffn("Album Artist", name, 13) ) {
        // Always prefer the Album Artist.
        strncpy(work->artist, value, 1024);
    }
    else if (  (0 == str_diffn("Artist", name, 7))
            && (0 == strlen(work->artist))
        ) {
        // Always prefer the Album Artist over Artist.
        strncpy(work->artist, value, 1024);
    }
}

void
trackInfo()
{
    struct trackmap *curtrk, *ttmp;
    if ( 5 <= Opts.verbose ) {
        HASH_ITER(hh, track, curtrk, ttmp) {
            printf("tI: %i (%i) %s/%s/%s %s\n"
                , curtrk->id, curtrk->time
                , curtrk->artist, curtrk->album
                , curtrk->name, curtrk->file
                );
        }
    }
}

void
trackFree()
{
    struct trackmap *curtrk, *ttmp;
    HASH_ITER(hh, track, curtrk, ttmp) {
        HASH_DEL(track, curtrk);
        free(curtrk);
    }
}
/**
 * vim: sw=4 ts=4 expandtab
 * EOF track_storage.c
 */

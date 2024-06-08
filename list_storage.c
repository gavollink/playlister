/****************************************************************************
 * File: list_storage.c
 *
 * Playlist hash management (memory storage)
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define LIST_STORAGE_C 1
#include <stdio.h>
#include <stdlib.h>    // malloc, realloc
#include <stdarg.h>    // va_args (wrapping fprintf)
#include <sys/stat.h>  // stat
#include <sys/errno.h> // errno
#include <string.h>    // strerror
#include <regex.h>     // POSIX Regular Expressions
#include "utils.h"
#include "options.h"
#include "storage.h"


struct list *playlist = NULL;

int
want_list(int plid, char* name)
{
    if ( utarray_len(Opts.playlist) ) {
        for ( char ** list = (char **) utarray_front(Opts.playlist)
            ; list != NULL
            ; list = (char **) utarray_next(Opts.playlist, list)
            ) {
            if ( 0 == str_diffn(name, *list, 1024) ) {
                return plid;
            }
        }
    }
    extradebug("want_list:Reject iTunes Playlist: [%s]\n", name);
    return 0;
}

int
set_list(int plid, char* name, char* value)
{
    struct list *work = NULL;
    int cx = 0;

    HASH_FIND_INT(playlist, &plid, work);
    if ( NULL == work ) {
        work = malloc( sizeof(struct list) );
        if ( NULL == work ) {
            mydebug("setlist:Unable to allocate %ld bytes of space: %s\n",
                    sizeof(struct list), strerror(errno));
            exit(-2);
        }
        // Full Initialization of new playlist
        memset((void *)work, 0, sizeof(struct list));
        work->wanted = 1; // Default to wanted, until we have a playlist name.
        work->id = plid;  // id should be set to iTunes Playlist ID
        utarray_new(work->trid, &ut_int_icd);
        HASH_ADD_INT(playlist, id, work);
    }
    if (0 == str_diffn("Name", name, 5) ) {
        strncpy(work->name, value, 1024);
        if ( ! want_list(plid, value) ) {
            work->wanted = 0;
        }
        if ( 4 <= Opts.verbose ) {
            printf("Found Playlist Named: [%s]\n", work->name);
        }
    }
    if (0 == str_diffn("Track ID", name, 5) ) {
        if ( work->wanted ) {
            int v = atoi(value);
            utarray_push_back(work->trid, &v);
        }
    }
    return 0;
}


void
listInfo()
{
    struct list *curlst, *ltmp = NULL;
    int cx = 0;

    if ( 3 > Opts.verbose ) {
        return;
    }

    HASH_ITER(hh, playlist, curlst, ltmp) {
        cx = 0;
        if ( curlst->wanted ) {
            mydebug("lI: %s (id:%i)"
                , curlst->name, curlst->id
                );
            for (int *trackid = (int *) utarray_front(curlst->trid)
                ; NULL != trackid
                ; trackid = (int *) utarray_next(curlst->trid, trackid)
                  , cx++ ) {
            }
            if (cx) {
                mydebug("\twith %i track%s\n", cx, (1<cx?"s":""));
            }
            else {
                mydebug(" is empty\n");
            }
        }
    }
}


void
listFree()
{
    struct list *curlst, *ltmp;
    HASH_ITER(hh, playlist, curlst, ltmp) {
        if (curlst->trid) {
            utarray_free(curlst->trid);
        }
        HASH_DEL(playlist, curlst);
        free(curlst);
    }
}

/**
 * vim: sw=4 ts=4 expandtab
 * EOF list_storage.c
 */

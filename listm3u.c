/****************************************************************************
 * File listm3u.c
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define LISTM3U_C 1
#include <stdio.h>
#include <sys/errno.h>   // errno
#include <string.h>      // strerror
#include "utils.h"       // string manipulation stuff
#include "storage.h"     // struct list *playlist, struct trackmap *track
#include "options.h"     // struct options Opts
#include "listm3u.h"     // Probably not needed.


char * _mk_list_filename(char *filepath, struct list *work, size_t pathsz);
struct trackmap * _get_track        (int trackid);
char            * _fix_track_path   (int trackid, char *trackpath, size_t tpsz);
int               _fprintM3UExtended(FILE *fh, int trackid);
FILE            * _open_list_file   (char *filepath);
void              _write_list       (struct list *work);


void
createLists()
{
    struct list *curlst, *ltmp = NULL;

    HASH_ITER(hh, playlist, curlst, ltmp) {
        if ( curlst->wanted ) {
            if ( 0 < utarray_len( curlst->trid ) ) {
                _write_list(curlst);
            }
            else {
                mywarning("createLists: Requested list %s has no tracks.\n"
                        , curlst->name);
            }
        }
    }
}


char *
_mk_list_filename(char *filepath, struct list *work, size_t pathsz)
{
    char filebase[1024] = "\0\0\0\0\0\0";
    char * tmp;

    strncpy(filebase, work->name, 1024);
    if (    ( 0 < strlen(filebase) )
        && ( 1024 > strlen(filebase) )
        ) {
        for ( int cx = 0; cx < strlen(filebase); cx++ ) {
            if ( ' ' == filebase[cx] ) {
                filebase[cx] = '_';
            }
        }
        if ( '.' != Opts.extension[0] ) {
            strncpy(filebase + strlen(filebase), "."
                    , ( 1024 - strlen(filebase)) );
        }
        strncpy(filebase + strlen(filebase), Opts.extension
                , ( 1024 - strlen(filebase)) );
        mydebug("List %s will be created as filebase [%s]\n"
                , work->name, filebase);
    }
    else {
        mywarning("Name of list %i is too long...\n", work->id);
        mywarning("[%s]\n", work->name);
        return NULL;
    }

    strncpy(filepath, Opts.output_path, pathsz);
    if ( '/' != filepath[strlen(filepath)-1] ) {
        strncpy(filepath + strlen(filepath), "/", pathsz - strlen(filepath));
    }
    strncpy(filepath + strlen(filepath), filebase, pathsz - strlen(filepath));

    mydebug("List %i path and filename [%s]\n"
            , work->id, filepath);

    return filepath;
}

struct trackmap *
_get_track(int trackid)
{
    struct trackmap *work = NULL;

    HASH_FIND_INT(track, &trackid, work);
    if ( NULL == work ) {
        mywarning("Track ID %i Referenced in playlist, but not found.\n"
                , trackid);
        return NULL;
    }
    return work;
}


char *
_fix_track_path(int trackid, char *trackpath, size_t tpsz)
{
    struct  trackmap  *work = NULL;
    char              *find = NULL;
    char               *ret = NULL;

    if ( NULL == ( work = _get_track(trackid) ) ) {
        return NULL;
    }
    strncpy(trackpath, work->file, tpsz);

    if ( 0 == removeString(trackpath, "file://localhost", tpsz) ) {
        removeString(trackpath, "file://", tpsz);
    }
    removeString(trackpath, Opts.itune_path, tpsz);

    prependString(trackpath, Opts.replace_path, tpsz);

    if (Opts.verify) {
        if ( strlen( Opts.verify_path ) ) {
            find = (char *)malloc( tpsz+1 );
            if ( NULL == find ) {
                myfatal("Out of memory.\n");
                exit(2);
            }
            memset(find, 0, tpsz+1);
            strncpy(find, work->file, tpsz);
            if ( 0 == removeString(find, "file://localhost", tpsz) ) {
                removeString(find, "file://", tpsz);
            }
            removeString(find, Opts.itune_path, tpsz);
            prependString(find, Opts.verify_path, tpsz);
        }
        else {
            find = trackpath;
        }

        ret = checkFileExists(find, tpsz);
        if ( find != trackpath ) {
            free(find);
        }
        if ( NULL == ret ) {
            return NULL;
        }
    }

    superdebug("_fix_track_path: [%s]\n", trackpath);
    return trackpath;
}


FILE *
_open_list_file(char *filepath)
{
    FILE *fh;

    if ( NULL == ( fh = fopen(filepath, "w") ) ) {
        mywarning("_open_list_file: opening %s: %s\n"
                , filepath, strerror(errno) );
        return NULL;
    }
    return fh;
}


int
_fprintM3UExtended(FILE *fh, int trackid)
{
    int       time = 0;
    struct trackmap *work;

    if ( NULL == ( work = _get_track(trackid) ) ) {
        return 0;
    }

    time = (int)( work->time / 1000 );

    return fprintf(fh, "#EXTINF:%i, %s - %s\n"
                , time, work->artist, work->name );
}


void
_write_list(struct list *work)
{
    char  filepath[2048] = "\0\0\0\0\0\0\0\0";
    char trackpath[2048] = "\0\0\0\0\0\0\0\0";
    FILE *fh;
    int cx = 0;

    if ( NULL == _mk_list_filename(filepath, work, 2048) ) {
        return;
    }

    if ( NULL == ( fh = _open_list_file(filepath) ) ) {
        return;
    }

    if ( Opts.randomize ) {
        randomUTarray(work->trid);
    }

    if ( Opts.m3uextended ) {
        fprintf(fh, "#EXTM3U\n");
    }

    for (int *trackid = (int *) utarray_front(work->trid)
            ; NULL != trackid
            ; trackid = (int *) utarray_next(work->trid, trackid), cx++
        ) {
        mydebug("_write_list: List %s (%i), Track ID %i\n"
                , work->name, work->id, *trackid);
        if ( NULL != _fix_track_path(*trackid, trackpath, 2048) ) {
            if ( Opts.m3uextended ) {
                _fprintM3UExtended(fh, *trackid);
            }
            fprintf(fh, "%s\n", trackpath);
        }
    }

    fclose(fh);
}


/**
 * vim: sw=4 ts=4 expandtab
 * EOF: listm3u.c
 */

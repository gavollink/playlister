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

/****************************************************************************
 * We're taking the NAME of the Playlist and turning it into a filename.
 * That means we're substituting or skipping invalid characters.
 */
char *
_mk_list_filename(char *filepath, struct list *work, size_t pathsz)
{
    char filebase[1024] = "\0\0\0\0\0\0";
    char * tmp;
    // I'm using this to track utf8 expected, but I'm not actually doing
    // anything with an error, since I'm killing anything over 127 anyway.
    char utf8step = 0;

    strncpy(filebase, work->name, 1024);
    if (   (    0 < strlen(filebase) )
        && ( 1024 > strlen(filebase) ) )
    {
        for ( int cx = 0; cx < strlen(filebase); cx++ ) {
            if ( 0 == ( 0x80 & filebase[cx] ) ) {
                // This is less than 127, so 7-bit clean...
                if (   ( 0x1F < filebase[cx] )
                    && ( 0x7f > filebase[cx] ) )
                {
                    // This is NOT a control character.
                    switch ( filebase[cx] ) {
                        case '#':
                        case '$':
                        case '%':
                        case '!':
                        case '&':
                        case '\'':
                        case '{':
                        case '"':
                        case '}':
                        case ':':
                        case '\\':
                        case '@':
                        case '<':
                        case '+':
                        case '>':
                        case '`':
                        case '*':
                        case '|':
                        case '?':
                        case '=':
                        case '/':
                            char fbcxorig = filebase[cx];
                            filebase[cx] = '-';
                            mydebug( "make filename sub '%c'->'%c' [%s]\n",
                                        fbcxorig, filebase[cx], filebase);
                            break;
                        case ' ':
                            filebase[cx] = '_';
                            break;
                    }
                } /* END ** if ( ( 0x1F < filebase[cx] ) ... ) */
                else {
                    // ctrl character territory, skip it
                    removeStringIdx(filebase, cx, 1024);
                }
                if ( 0 < utf8step ) {
                    // The initial utf8step match was probably not valid,
                    // so I'm cancelling the utf8step
                    utf8step = 0;
                }
            } /* END ** if ( 0 == ( 0x80 && filebase[cx] ) ) */
            else {
                // above 127, either ambiguous `extended` or UTF-8, skip it.
                // First, look for UTF-8 pattern...
                if ( 0x80 == ( 0xC0 & filebase[cx] ) ) {
                    // bin 1000-0000 == ( 1100-0000 & )
                    // UTF-8 Continuation byte, probably.
                    if ( 0 < utf8step ) {
                        utf8step--;
                    }
                }
                else if ( 0xC0 == ( 0xE0 & filebase[cx] ) ) {
                    // bin 1100-0000 == ( 1110-0000 & )
                    // UTF-8 Start byte, with two byte total
                    utf8step = 1;
                }
                else if ( 0xE0 == ( 0xF0 & filebase[cx] ) ) {
                    // bin 1110-0000 == ( 1111-0000 & )
                    // UTF-8 Start byte, with three byte total
                    utf8step = 2;
                }
                else if ( 0xF0 == ( 0xF8 & filebase[cx] ) ) {
                    // bin 1111-0000 == ( 1111-1000 & )
                    // UTF-8 Start byte, with four byte total
                    utf8step = 3;
                }
                mydebug( "make filename remove '%c' in [%s]\n",
                            filebase[cx], filebase);
                removeStringIdx(filebase, cx, 1024);
                cx--;
            }
        } /* END ** for ( cx < size of filebase ) */
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

/****************************************************************************
 * storage.c
 * $Rev: 47 $
 *
 * Parsed XML tracking
 *
 * Copyright (c) 2019, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define STORAGE_C 1
#include <stdio.h>
#include <stdlib.h> // malloc, realloc
#include <sys/errno.h> // errno
#include <string.h> // strerror
#include <regex.h> // POSIX Regular Expressions
#include "utils.h"
#include "options.h"
#include "storage.h"

/****************************************************************************
 * MODULE LOCAL DECLARATIONS
 */
struct level {
    int   level;
    int   in_tracks;
    int   in_playlists;
    int   is_key;
    int   is_trid;
    int   is_plid;
    int   skip;
    int   lvl_state;
    int   id;
    char  open_el[1024];
    char  open_text[1024];
    // char  last_key[1024];
    int   exptype;
    char  sibling_el[1024];
    char  sibling_text[1024];
    UT_hash_handle hh;
};

struct statistics Stats;

/****************************************************************************
 * MODULE GLOBALS
 */
int            node_depth = 0;
struct level   *node_tree = NULL;

/**
 * storageInit
 *
 * based on xml file size, allocate a storage buffer.
 */
void
storageInit()
{
    struct level *work;
    int level = 0;
    work = malloc( sizeof(struct level) );
    if ( NULL == work ) {
        mydebug("Unable to allocate %ld bytes of space: %s",
                sizeof(struct level), strerror(errno));
        exit(2);
    }
    memset((void *)work, 0, sizeof(struct level));
    memset((void *)&Stats, 0, sizeof(struct statistics));
    work->level        = level;
    HASH_ADD_INT(node_tree, level, work);

    return;
}


void
node_init(struct level *work)
{
    struct level *prevwork;

    work->in_tracks       = 0;
    work->in_playlists    = 0;
    work->is_key          = 0;
    work->is_trid         = 0;
    work->lvl_state       = 0;
    work->id            = 0;
    work->open_el[0]      = '\0';
    work->open_text[0]    = '\0';
    work->exptype         = 0;
    work->sibling_el[0]   = '\0';
    work->sibling_text[0] = '\0';
    prevwork              = work->hh.prev;

    if ( NULL != prevwork ) {
        // extradebug("node_init: Deep init node level %i from %i\n"
        //         , work->level, prevwork->level);
        if ( 1 == prevwork->in_tracks ) {
            work->in_tracks = 1;
        }
        if ( 1 == prevwork->in_playlists ) {
            work->in_playlists = 1;
        }
        if ( ( 0 == str_diffn( "key"
                            , prevwork->open_el, 4) )
                && ( _check_for_id(prevwork->open_text ) )
                && ( 0 == str_diffn("dict"
                            , prevwork->sibling_el, 5) )
            ) {
            prevwork->is_trid = 1;
            work->id = atoi(prevwork->open_text);
        }
    }
}


int
node_alloc(int depth, int ntype)
{
    int lvl_init = 0;
    struct level *work = NULL;

    if ( node_depth != depth ) {
        if ( depth < node_depth ) {
            while ( depth < node_depth ) {
                HASH_FIND_INT(node_tree, &node_depth, work);
                HASH_DEL(node_tree, work);
                free(work);
                node_depth--;
            }
        }
        else if ( depth > node_depth ) {
            if ( 1 != ntype ) {
                return lvl_init;
            }
            while ( depth > node_depth ) {
                work         = malloc( sizeof(struct level) );
                if ( NULL == work ) {
                    mydebug("sn:Unable to allocate %ld bytes of space: %s\n",
                            sizeof(struct level), strerror(errno));
                    exit(2);
                }
                memset((void *)work, 0, sizeof(struct level));
                // extradebug("sn:Allocated for level %d\n", depth);
                work->level = depth;
                HASH_ADD_INT(node_tree, level, work);
                lvl_init++;
                node_depth++;
            }
        }
    }
    return lvl_init;
}


void
set_node(int depth, int ntype, char* name, int emptyel, int hasval, char* value)
{
    char   trid[1024] = "\0\0\0";
    struct level *work, *prevwork = NULL;
    int    cx         = 0;
    int    init_level = node_alloc(depth, ntype);

    HASH_FIND_INT(node_tree, &node_depth, work);

    if (NULL == work) {
        mydebug("sn:%s unable to find depth %d\n"
                , "HASH_FIND_INT(node_tree, &node_depth, work)"
                , node_depth);
        exit(-2);
    }

    prevwork = work->hh.prev;

    if ( 1 == init_level ) {
        node_init(work);
        if ( NULL != prevwork ) {
            if ( ( 0 == str_diffn( "key"
                                , prevwork->open_el, 4) )
                    && ( _check_for_id(prevwork->open_text ) )
                    && ( 0 == str_diffn("dict"
                                , prevwork->sibling_el, 5) )
                ) {
                // extradebug("sn: Copy trid into node level %i from %i\n"
                //         , work->level, prevwork->level);
                strncpy(trid, prevwork->open_text, 1024);
            }
            else if (
                     ( prevwork->is_plid )
                    ) {
                work->is_plid = prevwork->is_plid;
                work->id = prevwork->id;
                work->skip = prevwork->skip;
            }
        }
        // extradebug("sn:Level %d initialized\n", node_depth);
    }

    // extradebug("sn:node_tree[%d], work, ptr is %p\n"
    //          , node_depth, work);

    if ( 1 == ntype ) {         // ELEMENT (Open)
        if ( 0 == str_diffn( "key", name, 4 ) ) {
            work->is_key = 1;
            work->lvl_state = 1;
            strncpy( work->open_el, name, 1024 );
            strncpy( work->open_text, "\0\0", 3 );
            strncpy( work->sibling_el, "\0\0", 3 );
            strncpy( work->sibling_text, "\0\0", 3 );
        }
        else if ( 0 == strlen( work->open_el ) ) {
            work->lvl_state = 1;
            strncpy( work->open_el, name, 1024 );
            strncpy( work->open_text, "\0\0", 3 );
            strncpy( work->sibling_el, "\0\0", 3 );
            strncpy( work->sibling_text, "\0\0", 3 );
        }
        else {
            work->lvl_state = 2;
            strncpy( work->sibling_el, name, 1024 );
            strncpy( work->sibling_text, "\0\0", 3 );
        }
    }
    else if ( 15 == ntype ) {   // Close Element
        if ( 1 == work->lvl_state ) { // open_el, open_text
            work->lvl_state = 0;
            if ( 0 == str_diffn("Playlists", work->open_text, 1024) ) {
                work->in_tracks = 0;
                work->in_playlists = 1;
                extradebug("sn:Playlist Start\n");
            }
            if ( 0 == str_diffn("Tracks", work->open_text, 1024) ) {
                work->in_tracks = 1;
                work->in_playlists = 0;
            }
        }
        else if ( 2 == work->lvl_state ) { // sibling_el, sibling_text
            work->lvl_state = 0;
            if (   ( 1 == work->in_tracks )
                    && ( 1 != work->is_trid )
                    && ( 0 == str_diffn( "Track ID", work->open_text, 9 ) )
                    && ( _check_for_id( work->sibling_text ) )
                    ) {
                // extradebug("sn:Track ID check, %s against ", work->sibling_text);
                strncpy(trid, work->sibling_text, 1024);
                work->is_trid = -1;
                if ( prevwork ) {
                    // extradebug("... Parent ID %s: ", prevwork->open_text);
                    if (
                        str_diffn(
                              prevwork->open_text
                            , work->sibling_text, 1024
                            )
                        ) {
                        mydebug(
                            "TRID in track %s does not match TRID on envelope %i\n"
                            , work->sibling_text, prevwork->open_text );
                        exit(3);
                    }
                    else {
                        work->id = atoi(work->sibling_text);
                        prevwork->is_trid = 1;
                        prevwork->id = work->id;
                    }
                }
                else {
                    mydebug("Found \"Track ID\" entry without envelope\n");
                    exit(3);
                }
            }
            else if ( ( 1 == work->in_tracks )
                    && ( 1 != work->is_trid )
                    ) {
                _set_track(work->id, work->open_text, work->sibling_text);
            }
            else if ( ( 1 == work->in_playlists )
                    && ( 0 == str_diffn( "Playlist ID", work->open_text, 9 ) )
                    && ( _check_for_id( work->sibling_text ) )
                    ) {
                Stats.playlists++;
                work->is_plid = 1;
                work->id = atoi( work->sibling_text );
            }
            else if ( ( 1 == work->in_playlists )
                    && ( 1 == work->is_plid )
                    && ( 0 == work->skip )
                    ) {
                if (set_list(work->id, work->open_text, work->sibling_text)) {
                    prevwork->skip = 1;
                }
            }
        }
        else {
            mydebug("sn:Close element, yet element not left open\n");
            mydebug("sn:Close : lvl_state is %d\n", work->lvl_state );
            mydebug("sn:Close : is_key is %d\n", work->is_key );
        }
    }
    else if ( 3 == ntype ) {    // TEXT (between elements)
        if ( 1 == work->lvl_state ) {
            strncpy( work->open_text, value, 1024 );
        }
        else if ( 2 == work->lvl_state ) {
            strncpy( work->sibling_text, value, 1024 );
        }
        else {
            mydebug("sn:#text [%s], but nowhere to put it.\n", value);
            mydebug("sn:#text : lvl_state is %d\n", work->lvl_state );
            mydebug("sn:#text : is_key is %d\n", work->is_key );
        }
    }

    // extradebug("Found end of set_node()\n");
}


int
trid_compare(char *strid, int itrid)
{
    char retrid[1024];
    sprintf(retrid, "%i", itrid);
    return( str_diffn( strid, retrid, 1024 ) );
}


int
_check_for_id(char* value)
{
    regex_t digits;
    int     rxret;
    char    rxerrbuf[1024];

    /* Compile regular expression */
    rxret = regcomp(&digits, "^[[:digit:]]+$", REG_EXTENDED|REG_NOSUB);
    if (rxret) {
        fprintf(stderr, "Could not compile regex\n");
        exit(2);
    }

    /* Execute regular expression */
    rxret = regexec(&digits, value, 0, NULL, 0);
    if (!rxret) {
        // extradebug("RegEx digits matched, %s\n", value);
        return(1);
    }
    else if (REG_NOMATCH == rxret) {
        // extradebug("RegEx for digits DID NOT match, [%s]\n", value);
        return(0);
    }
    else {
        regerror(rxret, &digits, rxerrbuf, sizeof(rxerrbuf));
        fprintf(stderr, "Regex match failed: %s\n", rxerrbuf);
        exit(2);
    }

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&digits);
    return(0);
}


void
storageInfo()
{
    if ( Stats.tracks ) {
        mydebug("sI: %s Totals\n", Opts.itunes_xml_file);
        mydebug("sI:    Tracks: %i\n", Stats.tracks);
        mydebug("sI: Playlists: %i\n", Stats.playlists);
    }

    trackInfo();
    listInfo();
}


void
storageFree()
{
    struct level *curlvl, *ltmp;
    listFree();
    trackFree();
    HASH_ITER(hh, node_tree, curlvl, ltmp) {
        HASH_DEL(node_tree, curlvl);
        free(curlvl);
    }
}

/**
 * vim: sw=4 ts=4 expandtab
 * EOF storage.c
 */

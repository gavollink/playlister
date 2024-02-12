/****************************************************************************
 * storage.h
 *
 * storage.c, track_storage.c, list_storage.c
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#ifndef STORAGE_H
#define STORAGE_H 1
#include "utils.h"
#include "uthash.h"
#include "utarray.h"

void storageInit();
void storageInfo();
void storageFree();
void set_node(int depth,   int ntype,  char* name, 
              int emptyel, int hasval, char* value);
int  _check_for_id(char* value);
int  trid_compare(char *strid, int itrid);
/* track_storage.c */
void _set_track(int trid, char* name, char* value);
void trackInfo();
void trackFree();
/* list_storage.c */
int set_list(int plid, char* name, char* value);
void listInfo();
void listFree();

struct statistics {
    int     tracks;
    int     playlists;
};

#ifndef STORAGE_C
extern struct statistics Stats;
#endif /* STORAGE_C */


struct trackmap {
    int   id;           // Track ID
    int   time;         // Total TIme
    char  file[1024];   // Location
    char  name[1024];   // Track Name
    char  album[1024];  // Album Name
    char  artist[1024]; // Album Artist or Artist
    UT_hash_handle hh;
};

#ifndef TRACK_STORAGE_C
extern struct trackmap *track;
#endif /* TRACK_STORAGE_C */

struct list {
    int   id;
    char  name[1024];
    int   wanted;
    UT_array * trid;
    UT_hash_handle hh;
};

#ifndef LIST_STORAGE_C
extern struct list *playlist;
#endif /* LIST_STORAGE_C */

#endif /* STORAGE_H */
/**
 * vim: sw=4 ts=4 expandtab
 * EOF storage.h
 */

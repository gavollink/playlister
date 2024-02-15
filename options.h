/****************************************************************************
 * File: options.h
 *
 * Command line option structure and handling.
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#ifndef OPTIONS_H
#define OPTIONS_H 1
#include "utils.h"

void  parseOpts  (int argc, char **argv);
void  OptsFree   (void);

struct options {
    int        config_requested;
    int        verbose;
    int        randomize;
    int        verify;
    int        m3uextended;
    char       self[1025]; // argv[0]
    char       config[1025]; // -c --con... config()
    char       itunes_xml_file[1025]; // -x --xml itunesxml()
    char       itune_path[1025]; // -r --rempath origpath()
    char       replace_path[1025]; // -n --newpath newpath()
    char       verify_path[1025]; // -o --output output()
    char       output_path[1025]; // -o --output output()
    char       extension[65]; // -X --extension extension()
    char       dist_version[65]; // Software version string.
    int        wantHelp;
    int        needHelp;
    UT_array * playlist;
};

#ifndef OPTIONS_C
// Loaded for everything EXCEPT options.c
extern struct options Opts;
#else
// Only LOADED For options.c
#define itunesxml(a) ( (0==str_diffn("--xml", (a), 6) ) \
        || (0==str_diffn("-x", (a), 3)) )
#define randomize(a)   (0==str_diffn("--ran", (a), 5) )
#define argformat(a) ( (0==str_diffn("--for", (a), 5) ) \
        || (0==str_diffn("-f", (a), 3)) )
#define origpath(a)  ( (0==str_diffn("--rem", (a), 5) ) \
        || (0==str_diffn("-r", (a), 3)) )
#define newpath(a)   ( (0==str_diffn("--new", (a), 5) ) \
        || (0==str_diffn("-n", a, 3)) )
#define output(a)    ( (0==str_diffn("--out", (a), 5) ) \
        || (0==str_diffn("-o", (a), 3)) )
#define extension(a) ( (0==str_diffn("--ext", (a), 5) ) \
        || (0==str_diffn("-X", (a), 3)) )
#define config(a)    ( (0==str_diffn("--con", (a), 5) ) \
        || (0==str_diffn("-c", (a), 3)) )
#define verbose(a)   ( (0==str_diffn("--verb", (a), 6) ) \
        || (0==str_diffn("-v", (a), 3)) )
#define quiet(a)     ( (0==str_diffn("--qui", (a), 5) ) \
        || (0==str_diffn("-q", (a), 3)) )
#define argabout(a)    (0==str_diffn("--abo", (a), 5) )
#define arglist(a)   ( (0==str_diffn("--lis", (a), 5) ) \
        || (0==str_diffn("-l", (a), 3)) )
#define arghelp(a)   ( (0==str_diffn("--hel", (a), 5) ) \
        || (0==str_diffn("-?", (a), 3)) \
        || (0==str_diffn("-h", (a), 3)) )
#define arghelpconf(a) (0==str_diffn("--help_c", (a), 8) )
#define argverify(a)   (0==str_diffn("--veri", (a), 6) )
#define argverifypath(a)  ( (0==str_diffn("--verify_p", (a), 10) ) \
        || (0==str_diffn("--verify_d", (a), 10) ) \
        || (0==str_diffn("--verify-d", (a), 10) ) \
        || (0==str_diffn("--verify-p", (a), 10) ) )
#endif /* OPTIONS_C */

#endif /* OPTIONS_H */
/**
 * vim: sw=4 ts=4 expandtab
 * EOF: options.h
 */

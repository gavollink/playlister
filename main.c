/****************************************************************************
 * main.c
 *
 * main() mydebug() extradebug()
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define MAIN_C 1
#include "utils.h"
#include "reader1.h"
#include "options.h"
#include "storage.h"
#include "listm3u.h"


int
main(int argc, char **argv)
{
    initUtils();

    parseOpts(argc, argv);

    storageInit();

    streamFile(Opts.itunes_xml_file);

    // The whole point of this program!
    createLists();

    // storage.c test output...
    if (3 <= Opts.verbose) {
        storageInfo();
    }

    storageFree();

    OptsFree();
    mydebug("Normal exit\n");
    return(0);
}


/**
 * vim: sw=4 ts=4 expandtab
 * EOF: main.c
 */

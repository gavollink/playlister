/****************************************************************************
 * options.c
 *
 * Command Line Parsing
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define OPTIONS_C 1
#include <stdio.h>     // printf(), fopen()
#include <sys/stat.h>  // stat()
#include <sys/errno.h> // int errno
#include <string.h>    // strerror(errno)
#include <regex.h>     // POSIX Regular Expressions
#include "utils.h"
#include "storage.h"
#include "options.h"

struct options Opts;
int            OptsInit = 0;

void
dohelp()
{
    if ( str_diffn("playlister", Opts.self, 11) ) {
        printf("Playlister - running as %s\n",
                Opts.self);
    }
    else {
        printf("Playlister\n");
    }
    printf("Version %s\n", Opts.dist_version );
    printf("Extract playlists from iTunes Music Library.xml to m3u\n");
    printf("\n");
    printf("License: see --about\n");
    printf("\n");
    printf("\n");
    printf("-c --config\n");
    printf("\tConfiguration File.\n");
    if ( strlen(Opts.config) ) {
        printf("\t\tValue: %s\n", Opts.config);
    }
    printf("\n");
    printf("--format (m3u|extm3u)\n");
    printf("\tm3u format, whether or not to include extended format\n");
    printf("\t(not compatible with all players).\n");
    printf("\t\tValue: %s\n", (Opts.m3uextended?"m3u Extended":"m3u standard"));
    printf("\n");
    printf("--verify_path <path>\n");
    printf("\tPath to check for file existance, implies --verify.\n");
    printf("\t  If not set --newpath will be used.\n");
    if ( strlen(Opts.verify_path) ) {
        printf("\t\tValue: %s\n", Opts.verify_path);
    }
    else if ( strlen(Opts.replace_path) ) {
        printf("\t\tValue: %s (from newpath)\n", Opts.replace_path);
    }
    printf("\n");
    printf("--verify\n");
    printf("\tVerify existance of output (warn/omit if not found).\n");
    printf("\t  --verify_path is applied and checked for each entry.\n");
    printf("\t\tValue: %s\n", (Opts.verify?"Yes":"No"));
    printf("\n");
    printf("--randomize\n");
    printf("\tRandomize m3u ouput.\n");
    printf("\t\tValue: %s\n", (Opts.randomize?"Yes":"No"));
    printf("\n");
    printf("-x --xml <file>\n");
    printf("\tiTunes XML file.\n");
    if ( strlen(Opts.itunes_xml_file) ) {
        printf("\t\tValue: %s\n", Opts.itunes_xml_file);
    }
    printf("\n");
    printf("-r --rempath <path>\n");
    printf("\tOriginal Path in iTunes XML (to remove from output filenames).\n");
    if ( strlen(Opts.itune_path) ) {
        printf("\t\tValue: %s\n", Opts.itune_path);
    }
    printf("\n");
    printf("-n --newpath <path>\n");
    printf("\tNew Path to prefix on output filenames.\n");
    if ( strlen(Opts.replace_path) ) {
        printf("\t\tValue: %s\n", Opts.replace_path);
    }
    printf("\n");
    printf("-o --output <path>\n");
    printf("\tPath to place generated playlists.\n");
    if ( strlen(Opts.output_path) ) {
        printf("\t\tValue: %s\n", Opts.output_path);
    }
    printf("\n");
    printf("-X --extension <ext>\n");
    printf("\tPlaylist extension (defaults to m3u).\n");
    if ( strlen(Opts.extension) ) {
        printf("\t\tValue: %s\n", Opts.extension);
    }
    printf("\n");
    printf("--nolist\n");
    printf("\tRemove all lists (like from a config).\n");
    printf("\n");
    printf("-l --list\n");
    printf("\tPlaylist to include (can use multiple times).\n");
    if ( utarray_len(Opts.playlist) ) {
        int once = 1;
        for ( char ** list = (char **) utarray_front(Opts.playlist)
            ; list != NULL
            ; list = (char **) utarray_next(Opts.playlist, list) ) {
            if ( once ) {
                printf("\t\tValue(s):\n");
                once = 0;
            }
            printf("\t\t    %s\n", *list);
        }
    }
    printf("\n");
    printf("-v --verbose\n");
    printf("\tMore verbose.\n");
    printf("\n");
    printf("-q --quiet\n");
    printf("\tLess verbose.\n");
    printf("\n");
    printf("--help_config\n");
    printf("\tSample configuration file\n");
    printf("\n");
    printf("-h -? --help\n");
    printf("\tThis help.\n");
    printf("\n");
    printf("--about\n");
    printf("\tCopyright and License information\n");
}

void
dohelpconf()
{
    if ( str_diffn("playlister", Opts.self, 11) ) {
        printf("Playlister - running as %s\n",
                Opts.self);
    }
    else {
        printf("Playlister\n");
    }
    printf("CONFIGURATION FILE HELP, Version %s\n", Opts.dist_version );
    printf("\n");
    printf(" * Any line that exceeds %d charaters will be truncated.\n",
            BUFSIZ-1);
    printf(" * Any path that exceeds 1024 characters will be truncated.\n");
    printf(" * random and verify can accept y, Y, or 1 to mean yes.\n");
    printf(" * format is either m3u or extm3u.\n");
    printf(" * extension does not need a prefixed period.\n");
    printf(" * location_replace can \"= .\", if"
           " the target player accepts it.\n");
    printf(" * if verify_dir isn't specified, location_replace is used.\n");
    printf("   * verify_dir implies verify=Y\n");
    printf("\n");
    printf("\n");
    printf("CONFIGURATION FILE SAMPLE\n");
    printf("\n");
    printf("itunesxml = /path/to/xml\n");
    printf("output_dir = /path/to/put/playlists\n");
    printf("verify_dir = /path/on/destination\n");
    printf("format = extm3u\n");
    printf("extension = m3u8\n");
    printf("random = Y\n");
    printf("verify = Y\n");
    printf("location_remove = C:\\path\\to\\iTunes\\iTunes Media\\\n");
    printf("location_replace = /path/on/destination\n");
    printf("\n");
    printf("[LISTS]\n");
    printf("Favorite Playlist\n");
    printf("Smart One\n");
    printf("\n");
}


void
doabout()
{
    if ( str_diffn("playlister", Opts.self, 11) ) {
        printf("Playlister - running as %s\n",
                Opts.self);
    }
    else {
        printf("Playlister\n");
    }
    printf("Version %s\n", Opts.dist_version );
    printf("Copyright (c) 2019-2024, Gary Allen Vollink.  %s\n"
            , "http://voll.ink/playlister");
    printf("All rights reserved.\n");
    printf("\n");
    printf("This program and it's source code are available for use by\n");
    printf("agreeing to the terms of the\n");
    printf("Common Development and Distribution License (CDDL) Version 1.0.\n");
    printf("This is included in the source distribution in the file:\n");
    printf("  CDDLICENSE.txt\n");
    printf("An online copy of which can be found here:\n");
    printf("  https://opensource.org/licenses/cddl1.txt\n");
    printf("\n");
    exit(0);
}


void
initOpts(char *me)
{
    char *idx = NULL;
    char newme[BUFSIZ] = "\0\0\0\0";

    if ( OptsInit ) {
        utarray_free(Opts.playlist);
    }
    memset(&Opts, 0, sizeof(struct options));
    Opts.verbose = 3;       // INFO
    if (   ( strlen(me)   )
        && ( idx = rindex(me, '/') )
        && ( 1 < strlen(idx) )
        ) {
        strncpy(newme, idx+1, BUFSIZ);
        strncpy(Opts.self, newme, 1024);
    }
    else {
        strncpy(Opts.self, me, 1024);
    }
    strncpy(Opts.config, Opts.self, 1018);
    strncpy(Opts.config + strlen(Opts.config), ".conf", 6);
    if ( 63 > ( strlen(PLAYLISTER_VERSION) ) ) {
        sprintf(Opts.dist_version, "%s", PLAYLISTER_VERSION);
    }
    else {
        strncpy(Opts.dist_version, "Unknown", 64);
    }
    strncpy(Opts.extension, "m3u", 4);
    utarray_new(Opts.playlist, &ut_str_icd);
    OptsInit = 1;
}


char *
cleanLine(char * line)
{
    char buffer[BUFSIZ];
    char buffer2[BUFSIZ];
    int          cx = 0;
    char * indexret = NULL;

    indexret = strstr(line, " #");
    if ( indexret ) {
        indexret[0] = '\0';
        indexret = NULL;
    }
    else if ( '#' == line[0] ) {
        line[0] = '\0';
        return( line );
    }

    if ( ( indexret = index(line, '=') ) ) {
        if (   ( BUFSIZ > ( indexret - line + 1 ) )
            && ( strlen(line) > ( indexret - line + 1 ) )
            ) {
            indexret[0] = '\0';
            strncpy(buffer2, &indexret[1], BUFSIZ);
            cleanLine(buffer2);
            cleanLine(line);
            strncpy(line + strlen(line), "=", BUFSIZ - strlen(line) );
            strncpy(line + strlen(line), buffer2, BUFSIZ - strlen(line) );
        }
    }

    for ( cx = strlen(line); 0 <= cx; cx-- ) {
        if ( 0x20 >= line[cx] ) {
            // All Ctrl chars up to and including 0x20 (space)
            line[cx] = '\0';
        }
        else if ( 0x7F == line[cx] ) {
            // ascii DEL
            line[cx] = '\0';
        }
        else {
            break;
        }
    }
    while ( strlen(line) ) {
        if ( 0x20 >= line[0] ) {
            strncpy(buffer, &line[1], BUFSIZ);
            strncpy(line, buffer, BUFSIZ);
        }
        else if ( 0x7F == line[0] ) {
            strncpy(buffer, &line[1], BUFSIZ);
            strncpy(line, buffer, BUFSIZ);
        }
        else {
            break;
        }
    }
    return (line);
}


int
parseConfigOption(char *line)
{
    char buffer1[BUFSIZ] = "\0";
    char buffer2[BUFSIZ] = "\0";
    char *      indexret = NULL;
    int            rxret = 0;
    regex_t        lists;

    strncpy(buffer1, line, BUFSIZ-1);

    /* Compile regular expression */
    rxret = regcomp(&lists, "^\\[lists\\]$", REG_ICASE|REG_EXTENDED|REG_NOSUB);
    if (rxret) {
        fprintf(stderr, "Could not compile regex\n");
        exit(5);
    }
    /* Execute regular expression */
    rxret = regexec(&lists, buffer1, 0, NULL, 0);
    if (!rxret) {
        // extradebug("RegEx lists matched, %s\n", buffer1);
        regfree(&lists);
        return(2);
    }
    else if (REG_NOMATCH == rxret) {
        // printf("RegEx for lists DID NOT match, [%s]\n", buffer1);
    }
    else {
        regerror(rxret, &lists, buffer2, sizeof(buffer2));
        fprintf(stderr, "Regex match failed: %s\n", buffer2);
        exit(5);
    }
    regfree(&lists);


    if ( ( indexret = index(buffer1, '=') ) ) {
        if (   ( BUFSIZ > ( indexret - buffer1 + 1 ) )
            && ( strlen(buffer1) > ( indexret - buffer1 + 1 ) )
            ) {
            indexret[0] = '\0';
            strncpy(buffer2, &indexret[1], BUFSIZ-1);
        }
    }

    if ( 0 == str_diffn("itunesxml", buffer1, 5) ) {
        if ( strlen(buffer2) ) {
            strncpy(Opts.itunes_xml_file, buffer2, 1024);
        }
        else {
            myfatal("itunesxml config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("output_path", buffer1, 6) ) {
        if ( strlen(buffer2) ) {
            strncpy(Opts.output_path, buffer2, 1024);
        }
        else {
            myfatal("output_path config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("verify_dir", buffer1, 8) ) {
        if ( strlen(buffer2) ) {
            strncpy(Opts.verify_path, buffer2, 1024);
            Opts.verify = 1;
        }
        else {
            myfatal("verify_dir config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("verify", buffer1, 7) ) {
        if ( strlen(buffer2) ) {
            if (   ( 'y' == buffer2[0] )
                || ( 'Y' == buffer2[0] )
                || ( '1' == buffer2[0] )
                ) {
                Opts.verify = 1;
            }
            else {
                Opts.verify = 0;
            }
        }
        else {
            myfatal("verify config option with no value.");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("format", buffer1, 4) ) {
        if ( strlen(buffer2) ) {
            if ( 0 == strncasecmp(buffer2, "extm3u", 5) ) {
                Opts.m3uextended = 1;
            }
            else if ( 0 == strncasecmp(buffer2, "m3uext", 5) ) {
                Opts.m3uextended = 1;
            }
            else if ( 0 == strncasecmp(buffer2, "m3u", 4) ) {
                Opts.m3uextended = 0;
            }
            else {
                myfatal("Unknown format request: %s\n"
                        , buffer2);
                exit(1);
            }
        }
        else {
            myfatal("format config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("extension", buffer1, 3) ) {
        if ( strlen(buffer2) ) {
            strncpy(Opts.extension, buffer2, 64);
        }
        else {
            myfatal("extension config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("location_remove", buffer1, 15) ) {
        if ( strlen(buffer2) ) {
            replCharString(buffer2, '\\', '/', 1024);
            strncpy(Opts.itune_path, buffer2, 1024);
        }
        else {
            myfatal("location_remove config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("location_replace", buffer1, 16) ) {
        if ( strlen(buffer2) ) {
            strncpy(Opts.replace_path, buffer2, 1024);
        }
        else {
            myfatal("location_replace config option with no value.\n");
            exit(1);
        }
    }
    else if ( 0 == str_diffn("random", buffer1, 6) ) {
        if ( strlen(buffer2) ) {
            if (   ( 'y' == buffer2[0] )
                || ( 'Y' == buffer2[0] )
                || ( '1' == buffer2[0] )
                ) {
                Opts.randomize = 1;
            }
            else {
                Opts.randomize = 0;
            }
        }
        else {
            myfatal("random config option with no value.\n");
            exit(1);
        }
    } else {
        myfatal("Unrecognized option line: %s = %s\n",
                buffer1, buffer2 );
        exit(1);
    }
    return 0;
}


void
readConfigFile()
{
    struct stat statbuf;
    char filename[BUFSIZ] = "\0\0\0\0";
    size_t linebufsiz = BUFSIZ;
    char *linebuffer;
    FILE *      fh = NULL;
    int    statret = 0;
    int         cx = 0;

    extradebug("readConfigFile(): start\n");

    if ( NULL == ( linebuffer = malloc(linebufsiz) ) ) {
        myerror("Configuration file reader: %s\n"
                , strerror(errno) );
        exit(5);
    }
    else {
        memset(linebuffer, 0, linebufsiz);
    }

    strncpy(filename, getenv("HOME"), BUFSIZ);
    strncpy(filename + strlen(filename)
            , "/.", BUFSIZ - strlen(filename));
    strncpy(filename + strlen(filename)
            , Opts.config, BUFSIZ - strlen(filename));

    extradebug("Try Config Filename: %s\n", filename);

    while ( ( statret = stat( filename, &statbuf ) ) ) {
#ifndef CONFIGFILE_CUSTOM_PATH
        cx++;
#endif /* ! CONFIGFILE_CUSTOM_PATH */
        switch (cx) {
#ifdef CONFIGFILE_CUSTOM_PATH
            case 0:
                strncpy(filename, CONFIGFILE_CUSTOM_PATH, BUFSIZ);
                if( '/' != filename[strlen(filename)-1] ) {
                    strncpy(filename + strlen(filename)
                            , "/", BUFSIZ - strlen(filename));
                }
                strncpy(filename + strlen(filename)
                        , Opts.config, BUFSIZ - strlen(filename));
                break;
#endif /* CONFIGFILE_CUSTOM_PATH */
            case 1:
                strncpy(filename, getenv("HOME"), BUFSIZ);
                if( '/' != filename[strlen(filename)-1] ) {
                    strncpy(filename + strlen(filename)
                            , "/", BUFSIZ - strlen(filename));
                }
                strncpy(filename + strlen(filename)
                        , Opts.config, BUFSIZ - strlen(filename));
                break;
            case 2:
                strncpy(filename, "/usr/local/etc/", BUFSIZ);
                strncpy(filename + strlen(filename)
                        , Opts.config, BUFSIZ - strlen(filename));
                break;
            case 3:
                strncpy(filename, "/etc/", BUFSIZ);
                strncpy(filename + strlen(filename)
                        , Opts.config, BUFSIZ - strlen(filename));
                break;
            case 4:
                strncpy(filename, Opts.config, BUFSIZ);
                break;
        }
        if ( 4 < cx ) {
            break;
        }
#ifdef CONFIGFILE_CUSTOM_PATH
        cx++;
#endif /* ! CONFIGFILE_CUSTOM_PATH */
        extradebug("Try Config Filename: %s\n", filename);
    }
    cx = 0;

    if ( statret ) {
        if ( Opts.config_requested ) {
            dohelp();
            myfatal( "Configuration file %s: %s\n"
                    , filename, strerror(errno) );
            exit(5);
        }
        strncpy(Opts.config, "\0\0\0\0\0\0\0\0", 9);
        return;
    }
    strncpy(Opts.config, filename, 1024);


    fh = fopen(filename, "r");
    if ( 0 >= fh ) {
        myfatal( "Configuration file %s: %s\n"
                , filename, strerror(errno) );
        exit(5);
    }

    int lists = 0;
    while ( 0 < getline(&linebuffer, &linebufsiz, fh) ) {
        cx++;
        linebuffer = cleanLine(linebuffer);
        if (strlen(linebuffer) ) {
            if ( 0 == lists ) {
                if ( 2 == parseConfigOption(linebuffer) ) {
                    lists = 1;
                    memset(linebuffer, 0, BUFSIZ);
                }
            }
            else if ( 1 == lists ) {
                // printf("Playlist: %s\n", linebuffer);
                utarray_push_back(Opts.playlist, &linebuffer);
            }
        }
    }

    extradebug("CONFIG:\nFile: %s\nSize: %lli\n", filename, statbuf.st_size);

    return;
}


int
_helpBeat(int useError)
{
    if ( (useError) && ( -1 == Opts.needHelp ) ) {
        Opts.needHelp = 1;
    }
    else if ( (useError) && ( 0 <= Opts.needHelp ) ) {
        Opts.needHelp++;
    }
    else if ( 0 == Opts.needHelp ) {
        Opts.needHelp = -1;
    }

    return Opts.needHelp;
}


void
parseOpts(int argc, char **argv)
{
    initOpts(argv[0]);

    if (   ( NULL != argv[1] )
        && ( argv[1][0] != '-' )
        && ( 2 == argc )
        ) {
        strncpy(Opts.itunes_xml_file, argv[1], 1024);
        return;
    }

    /* Pass 1, Configuration file/verbosity only. */
    for ( int cx = 1; cx < argc; cx++ ) {
        if ( config(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.config, argv[cx], 1024);
                Opts.config_requested = 1;
            }
        }
        else if ( verbose(argv[cx]) ) {
            Opts.verbose++;
        }
        else if ( quiet(argv[cx]) ) {
            Opts.verbose--;
        }
        else if ( argabout(argv[cx]) ) {
            doabout();
            // Always exits
        }
    }

    /* VERBOSITY IS SET, so we can use debug statements from here. */

    readConfigFile();

    /* Pass 2, CMDLINE options override Configuration File options */
    for ( int cx = 1; cx < argc; cx++ ) {
        if ( config(argv[cx]) ) {
            // Pulled above
            cx++;
        }
        else if ( verbose(argv[cx]) ) {
            // Pulled above
            ;
        }
        else if ( quiet(argv[cx]) ) {
            // Pulled above
            ;
        }
        else if ( itunesxml(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.itunes_xml_file, argv[cx], 1024);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( origpath(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.itune_path, argv[cx], 1024);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( newpath(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.replace_path, argv[cx], 1024);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( argformat(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                if ( 0 == strncasecmp(argv[cx], "extm3u", 5) ) {
                    Opts.m3uextended = 1;
                }
                else if ( 0 == strncasecmp(argv[cx], "m3uext", 5) ) {
                    Opts.m3uextended = 1;
                }
                else if ( 0 == strncasecmp(argv[cx], "m3u", 4) ) {
                    Opts.m3uextended = 0;
                }
                else {
                    myerror("Unknown format request: %s\n"
                            , argv[cx]);
                    _helpBeat(1);
                }
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( output(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.output_path, argv[cx], 1024);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( argverify(argv[cx]) ) {
            if ( argverifypath(argv[cx]) ) {
                if ( ( cx+1 ) < argc ) {
                    cx++;
                    strncpy(Opts.verify_path, argv[cx], 1024);
                    Opts.verify = 1;
                } else {
                    myerror("%s passed with no data.\n", argv[cx]);
                    _helpBeat(1);
                }
            } else {
                Opts.verify = 1;
            }
        }
        else if ( randomize(argv[cx]) ) {
            Opts.randomize = 1;
        }
        else if ( extension(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                strncpy(Opts.extension, argv[cx], 64);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( arglist(argv[cx]) ) {
            if ( ( cx+1 ) < argc ) {
                cx++;
                /* TODO : Handle comma separated listnames */
                utarray_push_back(Opts.playlist, &argv[cx]);
            }
            else {
                myerror("%s passed with no data.\n", argv[cx]);
                _helpBeat(1);
            }
        }
        else if ( 0 == str_diffn(argv[cx], "--nolist", 9) ) {
            utarray_free(Opts.playlist);
            utarray_new(Opts.playlist, &ut_str_icd);
        }
        else if ( arghelp(argv[cx]) ) {
            if ( arghelpconf(argv[cx]) ) {
                Opts.wantHelp = 2;
            } else {
                Opts.wantHelp = 1;
                _helpBeat(0);
            }
        }
        else {
            myerror("Unrecognized option: %s\n", argv[cx]);
            _helpBeat(1);
        }
    }

    mydebug("Options           Program Name = %s\n", Opts.self);
    mydebug("Options     configuration file = %s\n"
        , (strlen(Opts.config)?Opts.config:"NONE"));
    mydebug("Options                Verbose = %i (%s)\n",
            Opts.verbose, LogString[Opts.verbose]);
    mydebug("Options              Randomize = %i\n", Opts.randomize);
    mydebug("Options    Verify output files = %i\n", Opts.verify);
    mydebug("Options        iTunes XML file = %s\n", Opts.itunes_xml_file);
    mydebug("Options  Remove path component = %s\n", Opts.itune_path);
    mydebug("Options Prepend path component = %s\n", Opts.replace_path);
    mydebug("Options   Playlist output path = %s\n", Opts.output_path);
    mydebug("Options Playlist output format = %s\n"
            , (Opts.m3uextended?"M3U Extended":"M3U standard"));
    mydebug("Options     Playlist extension = %s\n", Opts.extension);

    if ( Opts.wantHelp ) {
        if ( 2 == Opts.wantHelp ) {
            dohelpconf();
        } else {
            dohelp();
        }
        exit(0);
    }

    if ( 0 == strlen(Opts.itunes_xml_file) ) {
        _helpBeat(1);
        myerror("Can't run without an itunes_xml_file.\n");
    }

    if ( Opts.needHelp ) {
        dohelp();
        if ( 1 < Opts.needHelp ) {
            mywarning("NOTE %i ERRORS ABOVE\n", Opts.needHelp);
            exit(5);
        }
        else if ( 1 == Opts.needHelp ) {
            mywarning("NOTE ERROR ABOVE\n");
            exit(5);
        }
        else {
            exit(0);
        }
    }
}


void
OptsFree()
{
    if ( Opts.playlist ) {
        utarray_free(Opts.playlist);
    }
}

/**
 * vim: sw=4 ts=4 expandtab
 * EOF: config.c
 */

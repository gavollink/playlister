/****************************************************************************
 * File: utils.c
 *
 * String manipulation, etc.
 *
 * Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see LICENSE file in this distribution.
 */
#define UTILS_C 1
#include <stdio.h>
#include <sys/errno.h>   // errno
#include <string.h>      // strerror
#include <sys/types.h>   // stat
#include <sys/stat.h>    // stat
#include <stdarg.h>      // va_args (wrapping fprintf)
#include <regex.h>       // POSIX Regular Expressions
#include "utils.h"
#include "options.h"

const char * const LogString[] = {
    [FATAL]   = "FATAL",
    [ERROR]   = "ERROR",
    [WARN]    = "WARN",
    [INFO]    = "INFO",
    [VERBOSE] = "VERBOSE",
    [DEBUG]   = "DEBUG",
    [DUMP]    = "DUMP"
};

void
initUtils()
{
    /* Setting locale. */
    setlocale(LC_ALL, "");

    /* This is a define macro, see configure.h */
    configrandseed();

    return;
}


char *
replaceString(char *str, const char *search, const char *replace, size_t max)
{
    char     *fname = "replaceString";
    char   *findptr = NULL;
    char    *buffer = NULL;
    size_t  lenorig = str_len(str);
    size_t  lensrch = str_len(search);
    size_t  lenrplc = str_len(replace);
    size_t  lentail = 0;
    size_t  lengrow = 0;

    superdebug("%s: Start\n[%s]\nsearch=[%s]\nreplace=[%s]\nmax=[%li]\n"
                , fname, str, search, replace, max);

    if ( max < ( lenorig - lensrch + lenrplc ) ) {
        mywarning("%s: suggested replacement would break buffer size, %lli\n"
                ,fname, max);
        return NULL;
    }

    if ( NULL == ( findptr = str_strn(str, search, lenorig) ) ) {
        superdebug("%s: str_strn reported not found.\n", fname);
        return str;
    }

    if ( NULL == ( buffer = malloc(max+1) ) ) {
        mywarning("%s: Unable to allocate buffer size, %lli: %s\n"
                , fname, max, strerror(errno));
        return NULL;
    }

    lengrow = (findptr - str);
    lentail = (lengrow + lensrch);
    strncpy( buffer, str, lengrow );
    strncpy( buffer+lengrow, replace, lenrplc );
    lengrow = (lengrow + lenrplc);
    strncpy( buffer+lengrow, str+lentail, (max - lengrow) );
    memset(str, 0, max);
    strncpy( str, buffer, max );
    free(buffer);
    return findptr;
}


int
URIunescape(char *str)
{
    char  buffer[BUFSIZ] = "\0\0\0\0\0\0\0\0";
    char buffer2[BUFSIZ] = "\0\0\0\0\0\0\0\0";
    char        *findptr = NULL;
    char       *startptr = str;
    int            repcx = 0;
    int          charhex = 0;
    int            rxret = 0;
    regex_t      percnum;

    /* Compile regular expression */
    rxret = regcomp(&percnum, "%[a-f0-9][a-f0-9]"
                    , REG_ICASE|REG_EXTENDED|REG_NOSUB);
    if (rxret) {
        fprintf(stderr, "Could not compile regex\n");
        exit(5);
    }

    while ( ( findptr = strchr(startptr, '%') ) ) {
        /* Execute regular expression */
        rxret = regexec(&percnum, startptr, 0, NULL, 0);
        if (!rxret) {
            superdebug("RegEx percnum matched, %s\n", findptr);
            findptr = index(startptr, '%');
            if ( ! findptr ) {
                fprintf(stderr, "RegEx found percent, index() could not.");
                exit(5);
            }
            strncpy( buffer, findptr, 3 );
            buffer[3] = '\0';
            charhex = (int) strtol( buffer + 1, NULL, 16 );
            buffer2[0] = (char) charhex;
            buffer2[1] = '\0';
            replaceString(startptr, buffer, buffer2, str_len(startptr) );
            startptr = findptr + 1;
            repcx++;
        }
        else if (REG_NOMATCH == rxret) {
            // extradebug("RegEx for percnum DID NOT match, [%s]\n", value);
            for ( int cx = 0; cx < (findptr - str); cx++ ) {
                buffer2[cx] = ' ';
            }
            buffer2[findptr - str] = '\0';
            strcpy(buffer2 + str_len(buffer2), "^ found here");
            mywarning("%s: %% without hex found in string:\n[%s]\n:%s\n"
                    , "URIunescape", str, buffer2);
            startptr = findptr + 1;
        }
        else {
            regerror(rxret, &percnum, buffer2, sizeof(buffer2));
            fprintf(stderr, "Regex match failed: %s\n", buffer2);
            exit(5);
        }
    }
    regfree(&percnum);
    return(repcx);
}


char *
prependString(char *str, const char *pre, size_t max)
{
    char     *fname = "prependString";
    char   *buffer = NULL;
    size_t    bsiz = 0;

    superdebug("%s: Start\n[%s]\nsearch=[%s]\nmax=[%li]\n"
                , fname, str, pre, max);

    // Adding "/" and room for a terminating null character.
    bsiz = ( 2 + str_len(str) + str_len(pre) );

    if ( bsiz > max ) {
        mywarning("_prepend_string: Unable to insert path replacement: %s\n"
                , "buffer not big enough" );
        exit(6);
    }

    if ( NULL == ( buffer = malloc(bsiz) ) ) {
        mywarning("_prepend_string: Memory: %s\n"
                , strerror(errno) );
        exit(6);
    }

    strncpy(buffer, pre, bsiz);
    strncpy(buffer + str_len(buffer), str, bsiz - str_len(buffer));
    strncpy(str, buffer, max);
    free(buffer);
    return str;
}


int
removeStringIdx(char *str, size_t index, size_t ssz)
{
    // char       *fname = "removeStringIdx";
    int         advance = 0;
    int         dozero = 0;
    size_t      cx = 0;

    for ( cx = 0; cx < ssz; cx++ ) {
        if ( dozero ) {
            str[cx] = (char)0;
        }
        else {
            if ( (char)0 == str[cx+advance] ) {
                dozero = 1;
            }
            else {
                if ( cx == index ) {
                    advance++;
                }
                if ( advance ) {
                    str[cx] = str[cx+advance];
                    str[cx+1] = (char)0;
                    if ( 0 == str[cx] ) {
                        dozero = 1;
                    }
                }
            }
        }
    }
    return( advance );
}

int
removeString(char *str, const char *needle, size_t ssz)
{
    char       *fname = "removeString";
    char       *found = NULL;
    int            cx = 0;
    int            dx = 0;
    int       removed = 0;
    int       fulllen = str_len(str);
    int        outlen = str_len(needle);

    superdebug("%s: Start\n[%s]\nsearch=[%s]\nmax=[%li]\n"
                , fname, str, needle, ssz);

    if ( NULL != ( found = str_strn(str, needle, ssz) ) ) {
        superdebug("%s: found = [%x]\n"
                , fname, found);
        for ( cx = (found - str), dx = (cx + outlen)
                ; dx <= fulllen
                ; cx++, dx++
            ) {
            str[cx] = str[dx];
            removed++;
        }
        for ( ; cx < dx; cx++ ) {
            str[cx] = '\0';
        }
    }
    return removed;
}


char *
replCharString(char *str, const char out, const char in, size_t ssz)
{
    int         cx = 0;
    int term_found = 0;

    for ( cx = 0
            ; cx <= ssz
            ; cx++
        ) {
        if ( 1 == term_found ) {
            str[cx] = '\0';
        }
        else if ( '\0' == str[cx] ) {
            term_found = 1;
        }
        else if ( out == str[cx] ) {
            str[cx] = in;
            if ( '\0' == in ) {
                term_found = 1;
            }
        }
    }

    return str;
}


void
superdebug(const char* text, ...)
{
    va_list args;
    if (DUMP <= Opts.verbose) {
        fprintf(stderr, "%-7s: ", LogString[DUMP]);
        va_start(args, text);
        vfprintf(stderr, text, args);
        va_end(args);
    }
}

void
extradebug(const char* text, ...)
{
    va_list args;
    if (DEBUG <= Opts.verbose) {
        fprintf(stderr, "%-7s: ", LogString[DEBUG]);
        va_start(args, text);
        vfprintf(stderr, text, args);
        va_end(args);
    }
}


void
mydebug(const char* text, ...)
{
    va_list args;
    if (VERBOSE <= Opts.verbose) {
        fprintf(stderr, "%-7s: ", LogString[VERBOSE]);
        va_start(args, text);
        vfprintf(stderr, text, args);
        va_end(args);
    }
}

void
myprint(const char* text, ...)
{
    va_list args;
    if (INFO <= Opts.verbose) {
        fprintf(stderr, "%-7s: ", LogString[INFO]);
        va_start(args, text);
        vprintf(text, args);
        va_end(args);
    }
}

void
mywarning(const char* text, ...)
{
    va_list args;
    if (WARN <= Opts.verbose) {
        fprintf(stderr, "%-7s: ", LogString[WARN]);
        va_start(args, text);
        vfprintf(stderr, text, args);
        va_end(args);
    }
}

void
myerror(const char* text, ...)
{
    va_list args;
    fprintf(stderr, "%-7s: ", LogString[ERROR]);
    va_start(args, text);
    vfprintf(stderr, text, args);
    va_end(args);
}

void
myfatal(const char* text, ...)
{
    va_list args;
    fprintf(stderr, "%-7s: ", LogString[FATAL]);
    va_start(args, text);
    vfprintf(stderr, text, args);
    va_end(args);
}

char *
spaces(char *buff, size_t bufsiz, int count)
{
    if ( count + 1 > bufsiz ) {
        myfatal(
            "spaces() asked for count %d on a buffer of %ld.\n",
            count,
            bufsiz
        );
        exit(15);
    }
    char *p = buff;
    int cx = count;
    while ( cx-- > 0 ) {
        *p++ = ' ';
        *p   = 0;
    }
    return buff;
}

char *
carrot(char *buff, size_t bufsiz, int count)
{
    if ( count + 2 > bufsiz ) {
        myfatal(
            "carrot() asked for count %d on a buffer of %ld.\n",
            count,
            bufsiz
        );
        exit(15);
    }
    char *p = buff;
    int cx = count;
    while ( cx-- > 0 ) {
        *p++ = ' ';
    }
    *p++ = '^';
    *p = 0;
    return buff;
}

char *
checkFileExists(char *filename, size_t fnamesize)
{
    struct stat       statbuf;
    int               statret = 0;
    char  lcopy[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  lwork[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  lpart[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  erbuf[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char              *findex = NULL;
    int                  loop = 0;
    size_t         nxslashidx = 0;
    size_t           slashidx = 0;
    size_t             curlen = 0;

    slashidx = strlen(filename);
    if ( 0 == slashidx ) {
        myerror(
            "checkFileExists("", %l): filename is empty (zero bytes).",
            fnamesize
        );
        return NULL;
    }
    if ( FILENAME_MAX <= slashidx ) {
        myerror(
            "checkFileExists(\"...\", %l): filename larger than FILENAME_MAX (%l).",
            fnamesize,
            FILENAME_MAX
        );
        myerror("\t[%s]", filename);
        return NULL;
    }

    // If the filename exists, just return the filename.
    if ( 0 == ( statret = stat( filename, &statbuf ) ) ) {
        return filename;
    }

    mywarning(
        "checkFileExists(\"%s\"): does not exist.\n",
        filename
    );

    // If not, we're going to grow lpart in steps, and look for
    // ways to fix the whole path...

    // First local caopy filename, so we aren't messing with it here.
    strncpy(lcopy, filename, FILENAME_MAX-1);
    findex = lcopy;

    slashidx = 0;
    while ( findex ) {
        loop++;
        if ( ( 1 == loop ) && ( '/' == lcopy[0] ) ) {
            slashidx = 0;
            strncpy(lwork, "/", 2);
            if ( 0 != ( statret = stat( lwork, &statbuf ) ) ) {
                myerror(
                    "checkFileExists: Unable to stat '%s': unrecoverable",
                    lpart
                );
                exit(16);
            }
        }
        if (( findex = strchr(lcopy + slashidx + 1, '/') )) {
            memset(lwork, 0, FILENAME_MAX);
            memset(lpart, 0, FILENAME_MAX);

            nxslashidx = findex - lcopy;
            curlen = (( nxslashidx ) - (slashidx + 1));

            //myerror("checkFileExists Debug Break: [%d]\n", loop);
            //myerror("lcopy: [%s]\n", lcopy);
            //myerror("\tlast slash at [%ld]\n", slashidx);
            //myerror("\tnext slash at [%ld]\n", nxslashidx);
            //myerror("\tcur length is [%ld]\n", curlen);

            if ( FILENAME_MAX-1 <= slashidx ) {
                myerror(
                    "checkFileExists, segment filesize beyond FILENAME_MAX\n"
                );
                exit(6);
            }

            strncpy(lwork, lcopy, FILENAME_MAX - 1);
            lwork[slashidx] = 0;

            mydebug("lwork: [%s]\n", lwork);

            strncpy(lpart, lcopy + slashidx + 1, curlen);
            mydebug(
                "lpart: %s[%s]\n",
                spaces(erbuf, FILENAME_MAX - 1, strlen(lwork) + 1),
                lpart
            );
            mydebug(
                "index: %s[%s]\n",
                spaces(erbuf, FILENAME_MAX - 1, strlen(lwork) + strlen(lpart) + 1),
                findex
            );

            strncat(
                lwork,
                "/",
                ((FILENAME_MAX - 2) - strlen(lwork))
            );
            strncat(
                lwork,
                lpart,
                ((FILENAME_MAX - 1) - (strlen(lpart) + strlen(lwork)))
            );

            mydebug("L[%d]: [%s]\n", loop, lwork);
            if ( 0 != ( statret = stat( lwork, &statbuf ) ) ) {
                if ( NULL == tryFindMatch(lcopy, lpart) ) {
                    findex = NULL;
                    myerror("Unable to find file, path broken at carrot (^):\n");
                    myerror("[%s]\n", filename);
                    myerror(
                        " %s\n",
                        carrot(erbuf, FILENAME_MAX - 1, slashidx + 1),
                        lwork
                    );
                    return NULL;
                }
            }
        }
        slashidx = nxslashidx;
    } // END of :: while ( 0 == done )

    if ( strlen(lwork) < strlen(lcopy) ) {
        memset(lpart, 0, FILENAME_MAX);
        strncpy(
            lpart,
            lcopy + nxslashidx + 1,
            ((FILENAME_MAX - 1) + (strlen(lwork) - nxslashidx))
        );
    }
        mydebug("lwork: [%s]\n", lwork);
        mydebug(
            "lpart: %s[%s]\n",
            spaces(erbuf, FILENAME_MAX - 1, strlen(lwork) + 1),
            lpart
        );
    if ( 0 == ( statret = stat( lcopy, &statbuf ) ) ) {
        strncpy(filename, lcopy, fnamesize);
        return filename;
    }
    else if ( NULL != tryFindMatch(lcopy, lpart) ) {
        strncpy(filename, lcopy, fnamesize);
        return filename;
    }
    return NULL;
}

char *
checkFileExists_old(char *filename, size_t fnamesize)
{
    struct stat       statbuf;
    int               statret = 0;
    char  lcopy[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  lpart[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char              *findex = NULL;
    int                  done = 0;

    if ( 0 == ( statret = stat( filename, &statbuf ) ) ) {
        return filename;
    }
    mywarning( "WARN: Playlist file not found: %s\n", filename );

    strncpy(lcopy, filename, FILENAME_MAX-1);
    strncpy(lpart, filename, FILENAME_MAX-1);

    while ( 0 == done ) {
        if ( NULL == ( findex = rindex(lcopy, '/') ) ) {
            extradebug( "WARN: Couldn't find slash in %s\n", lcopy );
            done = 1;
        }
        else if ( ( '/' == lcopy[0] )
            && ( findex == lcopy ) ) {
            extradebug( "WARN: At first slash in %s\n", lcopy );
            strncpy(lpart, findex+1, FILENAME_MAX-1);
            done = 1;
        }
        else {
            strncpy(lpart, findex+1, FILENAME_MAX-1);
            findex[0] = '\0';
        }
        if ( 0 == ( statret = stat( lcopy, &statbuf ) ) ) {
            mywarning( "WARN: Portion found: %s\n", lcopy );
            done = 1;
        }
        else {
            mywarning( "WARN: Portion not found: %s\n", lcopy );
        }
    }

    if ( NULL != tryFindMatch(lcopy, lpart) ) {
        return checkFileExists(lcopy, fnamesize);
    }
    
    mywarning( "Segment [%s]\n", lpart );

    return NULL;
}


char *
tryFindMatch(char *filename, char *segment)
{
    DIR                     *dh = NULL;
    char                 *fport = NULL;
    struct dirent        *entry = NULL;
    char workfile[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";

    strncpy(workfile, filename, FILENAME_MAX-1);

    if ( NULL == (fport = strstr(workfile, segment)) ) {
        mywarning("WARNING: tryFindMatch [%s] non matching part [%s].\n"
                    , workfile, segment);
        return NULL;
    }
    *fport = '\0';

    if ( NULL == ( dh = opendir(workfile) ) ) {
        myerror("Unable to opendir %s: %s\n"
                    , workfile, strerror(errno) );
        return NULL;
    }

    while(NULL != ( entry = readdir(dh) )) {
        if ( 0 == strcmp( entry->d_name, segment ) ) {
            mywarning( "tryFindMatch, segment found without modify.\n" );
            mywarning( "tryFindMatch, segment  [%s].\n"
                        , segment );
            mywarning( "tryFindMatch, filesystem %s [%s].\n"
                        , ((DT_DIR&entry->d_type)?"dir":"file")
                        , entry->d_name );
            closedir(dh);
            return(filename);
        }
        else if ( 0 == strcasecmp( entry->d_name, segment ) ) {
            *fport = segment[0];
            replaceString(
                workfile,
                segment,
                entry->d_name,
                FILENAME_MAX
            );
            myprint("tryFindMatch [%s]\n", filename);
            strcpy(filename, workfile);
            myprint(
                "tryFindMatch replacing [%s] with [%s]\n[%s]\n",
                segment,
                entry->d_name,
                filename
            );
            return(filename);
        }
    }

    closedir(dh);

    return NULL;
}


void
randomUTarray(UT_array *orig)
{
    int            cx = 0;
    int        random = 0;
    int         total = 0;
    int        *xport = NULL;
    int        *ordel = NULL;
    int        *rigel = NULL;
    UT_array *ordered = NULL;
    UT_array    *copy = NULL;

    total = utarray_len(orig);

    utarray_new(ordered, &ut_int_icd);
    utarray_new(copy, &ut_int_icd);

    extradebug("randomize: Copy the array\n");
    for ( cx = 0
            ; cx < total
            ; cx++
        ) {
        /* An array that just counts from 0 to number of elements in orig */
        utarray_push_back(ordered, &cx);
        /* An array that is copy of orig */
        xport = (int *)utarray_eltptr(orig, cx);
        utarray_push_back(copy, xport);
    }

    extradebug("randomize: randomly copy it back");
    /* Walk each element */
    for ( cx = 0
            ; cx < total
            ; cx++
        ) {
        /* Find a random within ordered */
        random = (configrand() % total);
        // There shouldn't be a way to not get a hit.
        ordel = (int *)utarray_eltptr(ordered, random);
        if (NULL == ordel) {
            mywarning("\n\neltptr returned NULL (%i)!\n", random);
        }

        while ( -1 == *ordel ) {
            extradebug("-");
            /*******************************************
             * Tried the idea of just walking up to the
             * next unused entry, but the vast majority
             * of the time, this takes longer than just 
             * randomly poking at the stack until we
             * reach an unused value.
          **********************************************
          *  if ( total <= ++random ) {
          *      random = 0;
          *      extradebug("/");
          *  }
          **********************************************
             * So... the faster version remains. */
            random = (configrand() % total);

            ordel = (int *)utarray_eltptr(ordered, random);
            if (NULL == ordel) {
                mywarning("\n\neltptr returned NULL (%i)!\n", random);
            }
        }

        // Get the ordel element from copy
        xport = (int *)utarray_eltptr(copy, *ordel);
        // Get the cx element from orig
        rigel = (int *)utarray_eltptr(orig, cx);
        // Overwrite orig[cx] with copy[rand]
        *rigel = *xport;
        // Make sure I can't use the same ordered element again.
        *ordel = -1;
        extradebug(".");
    }
    extradebug(" Done\n");

    utarray_free(ordered);
    utarray_free(copy);
}

char *
str_strn(const char *haystack, const char *needle, size_t len)
{
    char *fname = "str_strn";
    char   *hay = (char *)haystack;
    char   *nee = (char *)needle;
    char  start = needle[0];
    int   index = 0;
    int   match = 0;
    /* Needle must start by this point in haystack */
    int within = str_len(hay) - str_len(nee);

    superdebug("%s: Start\nhaystack=[%s]\nneedle=[%s]\nmax=[%li]\n"
                , fname, hay, nee, len);

    if ( 0 > within ) {
        /* Needle is longer than haystack */
        superdebug("%s: Needle is longer than haystack\n", fname);
        return NULL;
    }

    if ( len < within ) {
        /* Use len ONLY if it is shorter than within */
        superdebug("%s: Restricting search to supplied max.\n", fname);
        within = len;
    }

    if ( len < str_len(nee) ) {
        superdebug("%s: Needle (%li) is longer than max length (%li)\n"
                    , fname, str_len(nee), len);
        return NULL;
    }

    while ( 0 == match ) {
        index = str_chr(hay, start);
        if ( within < index ) {
            superdebug("%s: First char (%c) was not found in (%x) [%s] by pos [%li]\n"
                    , fname, start, hay, hay, within);
            return NULL;
        }
        superdebug("%s: First char (%c) was found in (%x) [%s] at pos %li\n"
               , fname, start, hay, hay, index);
        hay = hay + index;
        within = within - index;
        if ( 0 == ( match = str_start(hay, nee) ) ) {
            hay++;
        }
    }
    return hay;
}

/**
 * vim: sw=4 ts=4 expandtab
 * EOF: utils.c
 */

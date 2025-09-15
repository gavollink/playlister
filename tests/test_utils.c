#include <stdio.h>     // printf(), fopen()
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>   // stat
#include <sys/stat.h>    // stat
#include "../options.h"
#include "../utils.h"

struct options Opts;

void
usage(char *self)
{
    printf("%s\n", "This is for testing internal string utils.");
    printf("\n");
    printf("%s %s\n", self, "0|1|2");
    printf("\n");
    printf("%s\n", "   0  Pre-Tests String (not a test)");
    printf("%s\n", "   1  Tests URIunescape");
    printf("%s\n", "   2  Tests removeString");
    printf("%s\n", "   existing_dir  Tests checkFileExists()");
    return;
}

int
file_exists(char *filename)
{
    struct stat       statbuf;
    int               statret = 0;
    char  lcopy[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  lfake[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char  lpone[FILENAME_MAX] = "\0\0\0\0\0\0\0\0";
    char              fake[5] = "````";
    char              real[5] = "````";
    FILE *fh;
    strncpy(lcopy, filename, FILENAME_MAX-1);
    strncpy(lfake, filename, FILENAME_MAX-1);

    if ( 0 != ( statret = stat( lfake, &statbuf ) ) ) {
        myerror("file_exists: this test expects a real directory.\n");
        exit(2);
    }

    while(0 == ( statret = stat( lfake, &statbuf ) ) ) { 
        strncpy(lfake, filename, FILENAME_MAX-1);
        strncat(
            lfake,
            "/",
            ( ( FILENAME_MAX - 1 ) - strlen(lfake) )
        );
        if ( 'Z' == fake[0] ) {
            myerror("file_exists: exhausted alphabet looking for non-exists");
            exit(6);
        }
        if ( 'z' == fake[0] ) {
            fake[0] = 'A';
        }
        for(int cx=0; cx<4; cx++) {
            fake[cx] = fake[cx]+1;
        }
        strncat(
            lfake,
            fake,
            ( ( FILENAME_MAX - 1 ) - strlen(lfake) )
        );
    } // END of :: while(stat)
    strncat(
        lfake,
        "/lkjh9876lkjh/file.mp3",
        ( ( FILENAME_MAX - 1 ) - strlen(lfake) )
    );

    if ( NULL == checkFileExists(lfake, FILENAME_MAX - 1) ) {
        fprintf(stderr, "Path test failed well\n");
    } else {
        exit(12);
    }

    strncpy(real, fake, 4);
    for (int cx=0; cx<4; cx++ ) {
        if ('a' <= real[cx] && 'z' >= real[cx] ) {
            real[cx] = real[cx] - 32;
        } else {
            real[cx] = real[cx] + 32;
        }
    }

    strncpy(lpone, real, FILENAME_MAX-1);
    if ( -1 == mkdir(lpone, 0777) ) {
        myerror("Unable to create testing dir '%s': %s\n", real, strerror(errno));
        return -1;
    }
    strncat(
        lpone,
        "/lkjh9876lkjh",
        ( ( FILENAME_MAX - 1 ) - (strlen(lfake) + 13 ))
    );
    if ( -1 == mkdir(lpone, 0777) ) {
        myerror("Unable to create testing dir \"%s\"\n", lpone);
        return -1;
    }
    strncat(
        lpone,
        "/file.MP3",
        (( FILENAME_MAX - 1 ) - (strlen(lfake) + 9))
    );
    if (NULL == (fh = fopen(lpone, "w"))) {
        myerror("Unable to touch testing file '%s'\n", lpone);
        return -1;
    } else {
        fclose(fh);
    }

    fprintf(stderr, "Path original text  [%s].\n", lfake);
    if ( NULL != checkFileExists(lfake, FILENAME_MAX - 1) ) {
        fprintf(stderr, "Path case corrected [%s].\n", lfake);
    }

    if ( 0 == unlink(lpone) ) {
        char *fp = rindex(lpone, '/');
        *fp = 0;
        if ( 0 == rmdir(lpone) ) {
            fp = rindex(lpone, '/');
            *fp = 0;
            if ( -1 == rmdir(lpone) ) {
                myerror("Unable to rmdir '%s': %s\n",
                        lpone, strerror(errno));
            }
        } else {
            myerror("Unable to rmdir '%s': %s\n", lpone, strerror(errno));
        }
    } else {
        myerror("Unable to unlink '%s': %s\n", lpone, strerror(errno));
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    Opts.verbose = 0;

    char string[47] = "This is a test%3b%20a string with%20escapes%2E\n";

    if ( 2 != argc ) {
        usage(argv[0]);
        exit(1);
    }

    if ( '0' == *argv[1] ) {
        printf("%s", string);
        exit(0);
    }

    URIunescape(string);
    if ( '1' == *argv[1] ) {
        printf("%s", string);
    }
    else if ( '2' == *argv[1] ) {
        removeString(string, "This is a test", 40);
        printf("%s", string);
    }
    else if ( *argv[1] ) {
        Opts.verbose = 3;
        if (0 != file_exists(argv[1])) {
            exit(1);
        }
        exit(0);
    }
}

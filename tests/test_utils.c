#include <stdio.h>     // printf(), fopen()
#include "../options.h"
#include "../utils.h"

struct options Opts;

int
main(int argc, char *argv[])
{
    Opts.verbose = 0;

    char string[47] = "This is a test%3b%20a string with%20escapes%2E";

    if ( 1 == argc ) {
        printf("%s", string);
    }

    URIunescape(string);
    if ( '1' == *argv[1] ) {
        printf("%s", string);
    }

    removeString(string, "This is a test", 40);

    if ( '2' == *argv[1] ) {
        printf("%s", string);
    }
}

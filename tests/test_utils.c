#include <stdio.h>     // printf(), fopen()
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
    return;
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

    removeString(string, "This is a test", 40);

    if ( '2' == *argv[1] ) {
        printf("%s", string);
    }
}

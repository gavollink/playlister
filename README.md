# Playlister

Extracts playlists from iTunes Music Library.xml file based on command line or configuration file.

More information at the project's [homepage](https://www.home.vollink.com/gary/playlister)

Copyright (c) 2019-2024, Gary Allen Vollink.  http://voll.ink/playlister
All rights reserved.

# macOS / Apple OS X

The Makefile will now build a universal ( `x86_64` & `arm64` combined ) binary
by default if being build on OS X 11 or higher
(that has the first version of XCode to support arm64 at all).

# Notes

## Things to check

Look at the DEPENDS file for run-time and compile dependencies.

Look at the README (not the same as this) for changes between release tags.

## Makefile Inefficiency

The way the Makefile is built to be able to support univeral binaries on 
macOS (M + x86), is not done in a great way and it forces the object files
to be deleted during the build process (so it can rebuild for the second
target architecture).  That means, running `make` twice in a row will
actually build the whole project again.
( And doing a `make INSTALLDIR=<dir> install` will ALSO build the whole
project again. )

It's messy, but it works.  Blame late night hacking.


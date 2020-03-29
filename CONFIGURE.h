/****************************************************************************
 * CONFIGURE.h
 * $Rev: 47 $
 *
 * SYSTEM CONFIGURATION OPTIONS (where OTHER projects use autoconf).
 *
 * Copyright (c) 2019, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#ifndef CCOONNFFIIGGUURREE_H
#define CCOONNFFIIGGUURREE_H 1

/***************************************
 * INSTRUCTIONS
 * All of the configuration options here are
 * commented out, and none need o be defined for
 * this program to compile and work.
 **
 * Further information about each is described in a comment
 * block (like this one) below.
 */

/***************************************
 * TARGET
 * Is defined in Makefile and is the name playlister will compile to.
 * Note that this isn't actually used IN the program, as Playlister
 * will always use argv[0] to figure out its own name.
 * Thus, where TARGET is mentioned below as a documented point,
 * it is only to illustrate the base-name of the running program.
 */

/***************************************
 * CONFIGFILE_CUSTOM_PATH -- If set, Playlister will look for
 * a configuration file there second.
 * See next comment group.
 */

// #define CONFIGFILE_CUSTOM_PATH "/etc/playlister/"

/***************************************
 * Config file search order:
 **
 * 1- $(HOME)/.$(TARGET).conf
 * 2 SKIPPED if not defined:
 *  - $(CONFIGFILE_CUSTOM_PATH)/$(TARGET).conf
 * 3- $(HOME)/$(TARGET).conf
 * 4- /usr/local/etc/$(TARGET).conf
 * 5- /etc/$(TARGET).conf
 * 6- $(TARGET).conf
 */

/***************************************
 * The VERY few things below this line is the stuff that
 * autoconf would usually figure out for us in a more substantial
 * package.  I can't be bothered for this few choices, all of which
 * will work with default fallbacks anyway.
 */

/***************************************
 * HAS_ARC4RANDOM
 * man arc4random -- should display page if the system has it.
 */

// #define HAS_ARC4RANDOM 1

/***************************************
 * HAS_SRANDDEV
 * man srandev -- should display page if the system has it.
 */

// #define HAS_SRANDDEV 1

#endif /* CCOONNFFIIGGUURREE_H */
/**
 * vim: sw=4 ts=4 expandtab
 * EOF: CONFIGURE.h
 */

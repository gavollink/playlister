# If the first ones are set command-line,
# `:=` won't override the values.
CC:=gcc
# NOTE: -std=gnu99 will be added to CCFLAGS by mk.skel, don't add -std here
CCFLAGS:=-Wall -O2
# DO NOT SET DIRECTLY, BUT THIS IS THE `install' PATTERN...
## INSTALLDIR=$(DESTDIR)$(exec_prefix)/$(bindir)/
# Normally, DESTDIR is reserved for chroot style overrides,
# like for a binary distribution packaging step.
prefix=
exec_prefix=$(prefix)
bindir=bin
# The include path may need to be modified
CCFLAGS+=-I/usr/include/libxml2
LDFLAGS=-lxml2

# IF UTHASHDIR IS SOMEWHERE ELSE, NOTE IT HERE
UTHASHDIR?=uthash

########################################
# FEATURES
########################################

# HAS_ARC4RANDOM
# man arc4random -- should display page if the system has it.
#
# NOTE: On LINUX, this is preferred over the default, and setting this
# may need tweaking the LDFLAGS below to include whatever
# library stores this on YOUR system (on Deb-like, this is -lbsd).
#
# HAS_ARC4RANDOM=1
#
# These are set IF the above is uncommented
ifdef HAS_ARC4RANDOM
CCFLAGS+=-DHAS_ARC4RANDOM=1
LDFLAGS+=-lbsd
endif

# MacOS - Set a code signing signature name here.
# SIGNID=CompanyName Releases
SIGNID:=
# MacOS/Sign - If signature is in a non-standard keychain:
# CHAIN=Offline.keychain-db
CHAIN:=

# vim: ft=make
# EOF configure.dist.mk / configure.mk

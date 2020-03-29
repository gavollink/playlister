# Makefile for playlister
# $Rev: 51 $
TARGET=playlister
BUILDDIR=$(shell pwd)
SRCS=utils.c storage.c options.c reader1.c track_storage.c \
        list_storage.c main.c listm3u.c
DJBSRCS=djb/str_len.c djb/str_diffn.c djb/str_chr.c djb/str_start.c
OBJS=utils.o storage.o options.o reader1.o track_storage.o \
        list_storage.o main.o listm3u.o str_len.o \
        str_diffn.o str_chr.o str_start.o
CFLAGS=-g -I/usr/include/libxml2 \
 -I/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include/libxml2
LFLAGS=-lxml2
DEPENDS=CONFIGURE.h utils.h reader1.h storage.h options.h listm3u.h Makefile
UTHASH=uthash.h utarray.h
SUPPORT=DEPENDS README

default: $(TARGET)

%.o: %.c $(DEPENDS) $(UTHASH)
	gcc -c $< -o $@ $(CFLAGS)

$(TARGET): $(OBJS)
	gcc $(OBJS) $(LFLAGS) -o $@

test: $(TARGET)
	cd tests && $(MAKE) BUILDDIR=$(BUILDDIR) TARGET=$(TARGET)

uthash.h: uthash
	ln -fs uthash/src/uthash.h

utarray.h: uthash
	ln -fs uthash/src/utarray.h

uthash:
	@echo "git clone https://github.com/troydhanson/uthash.git"
	false   # force a fail here.

distclean: clean
	-rm -f $(TARGET) $(UTHASH)
	cd tests && $(MAKE) BUILDDIR=$(BUILDDIR) TARGET=$(TARGET) clean

clean:
	-rm -f $(OBJS)
	cd tests && $(MAKE) BUILDDIR=$(BUILDDIR) TARGET=$(TARGET) clean

#############
# Dan J. Bernstein code (Public Domain) -- included in this package.
# Definitions included in utils.h
str_len.o: djb/str_len.c
	gcc -c $< -o $@ $(CFLAGS)

str_diffn.o: djb/str_diffn.c
	gcc -c $< -o $@ $(CFLAGS)

str_chr.o: djb/str_chr.c
	gcc -c $< -o $@ $(CFLAGS)

str_start.o: djb/str_start.c
	gcc -c $< -o $@ $(CFLAGS)

# vim: sw=4 ts=4 expandtab

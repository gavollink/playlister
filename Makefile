CC=gcc
CCFLAGS=-Wall -O2
CCFLAGS+=-std=gnu99
ifdef DEBUG
	# Maintainer stuff only, you don't want to see this.
CCFLAGS+=-ggdb -DDEBUG
endif
CFLAGS+=-I/usr/include/libxml2
#CCFLAGS+=-ggdb
LFLAGS=-lxml2
BUILDDIR=$(shell pwd)
INSTALLDIR=

FINAL=playlister
SOURCE=utils.c storage.c options.c reader1.c track_storage.c
SOURCE+=list_storage.c main.c listm3u.c
OBJS=utils.o storage.o options.o reader1.o track_storage.o
OBJS+=list_storage.o main.o listm3u.o str_len.o
OBJS+=str_diffn.o str_chr.o str_start.o
UTHASH=uthash.h utarray.h
X_DEPS=Makefile
X_DEPS+=CONFIGURE.h utils.h reader1.h storage.h options.h listm3u.h

SYS=$(shell uname -s)
ARCH=$(shell uname -m)
MAJ_VER=$(shell uname -r | awk -F. '{print $$1}')
# OS X 11, Big Sur, is the earliest macOS that supported arm64
TARGET_ARM64=-target arm64-apple-macos11
# OS X 10.10 is Yosemite, released 2014, which supports
# all hardware from 2010 or more recent.
TARGET_X86_64=-target x86_64-apple-macos10.10

ifeq ($(SYS), Darwin)
CFLAGS+=-I/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include/libxml2
endif

ifeq ($(shell test "Darwin" = $(SYS) -a $(MAJ_VER) -ge 20; echo $$?), 0)
# This is a mac running Big Sur (11, D20) or NEWER:
# Darwin 20 is Big Sur, Big Sur is the earliest macOS that supported arm64.
# Any build platform here (x86_64 or arm64) will be able to target
# a universal binary with arm64-Big Sur and x86_64-Yosemite.
INTERIM=universal.$(FINAL)
NEEDTARGET=1
else ifeq ($(shell test "Darwin" = $(SYS) -a $(MAJ_VER) -lt 14; echo $$?), 0)
# This is a mac running Mavericks (10.9, D13) or OLDER.
# Treating this the same as Linux, where we have no -target specified
# and just expect the compiler will do the right thing.
INTERIM=default.$(FINAL)
else ifeq ($(shell test "Darwin" = $(SYS); echo $$?), 0)
# This is a macOS between Yosemite (10.10, D14) and Catalina (10.15, D19)
# Yosemite is the earliest target macOS I own (or have access to),
INTERIM=$(ARCH).$(FINAL)
NEEDTARGET=1
else
INTERIM=default.$(FINAL)
endif

$(FINAL): $(INTERIM)
	cp $(INTERIM) $(FINAL)

test: $(FINAL)
	cd tests && $(MAKE) BUILDDIR=$(BUILDDIR) TARGET=$(FINAL)

%.o: %.c $(X_DEPS) $(UTHASH)
	@if [ "1" = "$(NEEDTARGET)" ]; then \
		if [ -z "$(TARGET)" ]; then \
			echo "Something went wrong, need to specify TARGET= for build."; \
			false; \
		else true; \
		fi ; \
	fi
	gcc -c $< -o $@ $(CFLAGS) $(TARGET)

universal.$(FINAL): x86_64.$(FINAL) arm64.$(FINAL)
	lipo -create -output universal.$(FINAL) x86_64.$(FINAL) arm64.$(FINAL)

x86_64_run:
	$(MAKE) TARGET="$(TARGET_X86_64)" $(OBJS)

arm64_run:
	$(MAKE) TARGET="$(TARGET_ARM64)" $(OBJS)

# x86_64 target is Yosemite (10.10, D14)
# Yosemite is the earliest target macOS I own (or have access to),
# You might have luck targeting something that's older, but I
# won't release what I cannot test.
# So that's where I'm pegging x86_64 specific targets.
# There's a comment at the end of the file for further info...
x86_64.$(FINAL): x86_64_run
	$(CC) $(OBJS) $(LFLAGS) -o $@ $(TARGET_X86_64)
	@rm -f $(OBJS)

# OS X 11, Big Sur is the first arm64 capable version, so it is the lowest
# target possible for arm64.
arm64.$(FINAL): arm64_run
	$(CC) $(OBJS) $(LFLAGS) -o $@ $(TARGET_ARM64)
	@rm -f $(OBJS)

default.$(FINAL): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $@ $(TARGET)
	@rm -f $(OBJS)

install: $(FINAL)
	@if [ -d "$(INSTALLDIR)" ]; then \
		install -m 775 -C $(FINAL) $(HOME)/sbin; \
	elif [ -z "$(INSTALLDIR)" ]; then \
		echo "Cannot install, 'INSTALLDIR=', is not set." ; \
		echo "TRY: $$ make INSTALLDIR=<path> install" ; \
		false ;\
	else \
		echo "Cannot install, 'INSTALLDIR=$(INSTALLDIR)', does not exist." ; \
		false ;\
	fi

clean:
	-rm -f *.o *.obj *.$(FINAL)

dist-clean: clean
	-rm -f $(FINAL)

uthash.h: uthash
	ln -fs uthash/src/uthash.h

utarray.h: uthash
	ln -fs uthash/src/utarray.h

uthash:
	@echo "    "
	@echo "    HEY!  You need a copy of UTHASH locally.  Try this:"
	@echo "    "
	@echo "git clone https://gitlab.home.vollink.com/not-mine/troydhanson/uthash.git"
	@echo "    my (tested) copy"
	@echo "    ---- OR ----"
	@echo "git clone https://github.com/troydhanson/uthash.git"
	@echo "    original (latest)"
	@echo "    "
	@echo "    If you have this checked out somewhere else,"
	@echo "    a softlink to it will work too..."
	@echo "ln -s ../uthash ."
	@echo "    "
	@echo "    Come back when I can find uthash!"
	@echo "    "
	@false   # force a fail here.

#############
# Dan J. Bernstein code (Public Domain) -- included in this package.
# Definitions included in utils.h
str_len.o: djb/str_len.c
	gcc -c $< -o $@ $(CFLAGS) $(TARGET)

str_diffn.o: djb/str_diffn.c
	gcc -c $< -o $@ $(CFLAGS) $(TARGET)

str_chr.o: djb/str_chr.c
	gcc -c $< -o $@ $(CFLAGS) $(TARGET)

str_start.o: djb/str_start.c
	gcc -c $< -o $@ $(CFLAGS) $(TARGET)

# FROM WIKIPEDIA...
#
#   NAME       (Moc OS X Version , Darwin Kernel Version)
# Cheetah 		( 10,    1.3.1    )  POWERPC ONLY (32 bit only)
# Puma 			( 10.1,  1.4.1 or 5 )
# Jaguar 	 	( 10.2,  6        )  First to support 64 bit POWERPC)
# Panther 		( 10.3,  7        )
# Tiger 		( 10.4,  8        )  First to support INTEL (32bit Darwin)
# Leopard 		( 10.5,  9        )
# Snow Leopard 	( 10.6,  10       )  Last support POWERPC, first 64bit Darwin
# Lion 			( 10.7,  11       )
# Mountain Lion	( 10.8,  12       )  64bit INTEL Darwin only
# Mavericks 	( 10.9,  13       )
# Yosemite 		( 10.10, 14       )  My oldest MBAir is here
# El Capitian 	( 10.11, 15       )
# Sierra 		( 10.12, 16       )
# High Sierra 	( 10.13, 17       )
# Mojave 		( 10.14, 18       )
# Catalina 		( 10.15, 19       )  Drops ALL 32bit app support
# Big Sur 		( 11, 	 20       )  Introduction of arm64
# Monterey		( 12, 	 21       )
# Ventura 		( 13, 	 22       )
# Sonoma  		( 14, 	 23       )  I don't have access to this yet
#
# So Tiger (10.4, D8) on 64bit would be the oldest theoretical x86_64 target
# but Mountain Lion (10.8, D12) is more realistic because it's the first
# place we're guaranteed to see a 64bit Darwin.
# I don't even know if it CAN be targeted anymore, or if it would work.
# So, since I can test Yosemite and know it works, that is what this
# Makefile does, the info here is for those who have older stuff and want to
# fiddle around with trying to support it with a UNIVERSAL binary.
#
# Real talk... All Apple hardware sold from 2010 on supports Yosemite
# or newer.  That's 13 years ago, which is forever for Apple hardware.
#
# EOF Makefile

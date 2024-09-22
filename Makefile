# Makefile for playlister
include configure.mk
CTARGETS=playlister
ITARGETS=$(CTARGETS)
MDEP=configure.mk mk.skel Makefile

DJBSRC=djb/str_diffn.c djb/str_chr.c djb/str_start.c djb/str_len.c
SOURCE=utils.c storage.c options.c reader1.c track_storage.c
SOURCE+=list_storage.c main.c listm3u.c
SOURCE+=$(DJBSRC)
UTHASH=uthash.h utarray.h
X_DEPS=configure.h utils.h reader1.h storage.h options.h listm3u.h djb/str.h

all: playlister

playlister: $(MDEP) $(SOURCE) $(X_DEPS) $(UTHASH)
	$(MAKE) -f mk.skel SOURCE="$(SOURCE)" XDEP="$(X_DEPS)" FINAL=$@ $@

configure.mk: configure.dist
	@if [ -e "$@" ]; then \
		NEWCONF=`cat $@ | grep -v '^#' | awk -F'[?:+=]' '$$1 != "" {print $$1}' | sort`; \
		OLDCONF=`cat $< | grep -v '^#' | awk -F'[?:+=]' '$$1 != "" {print $$1}' | sort`; \
		if [ "$${NEWCONF}" != "$${OLDCONF}" ]; then \
			echo "##########################################################"; \
			echo "####  Distribution's configure.dist is newer than $@"; \
			echo "####   please compare and update $@ manually."; \
			echo "##########################################################"; \
			false; \
		else true; \
		fi; \
	else \
		cp $< $@; \
	fi

$(UTHASH): $(UTHASHDIR)/src/uthash.h
	@ln -s "$(UTHASHDIR)/src/$@"

$(UTHASHDIR)/src/uthash.h:
	@echo "    "; \
		echo "    HEY!  You need a copy of UTHASH locally.  Try this:"; \
		echo "    "; \
		echo "git clone https://github.com/troydhanson/uthash.git"; \
		echo "    original (latest)"; \
		echo "    ---- OR ----"; \
		echo "git clone https://gitlab.home.vollink.com/not-mine/troydhanson/uthash.git"; \
		echo "    author's (tested) copy"; \
		echo "    "; \
		echo "    If you have this checked out somewhere else, set UTHASHDIR"; \
		echo "    in configure.mk"; \
		echo "    "; \
		echo "    NOTE: This Makefile looks for $$UTHASHDIR/src/uthash.h"; \
		echo "    "; \
		echo "    Come back when I can find uthash!"; \
		echo "    "; \
		false

clean:
	$(MAKE) -f mk.skel ITARGETS="$(ITARGETS)" clean
	@cd tests && $(MAKE) BUILDDIR=.. clean

distclean dist-clean:
	rm -f $(UTHASH)
	$(MAKE) -f mk.skel ITARGETS="$(ITARGETS)" distclean
	@if [ -e "configure.mk" ]; then \
		diff "configure.mk" "configure.dist" 2>&1 >/dev/null; \
		if [ "0" != "$$?" ]; then \
			echo "##########################################################"; \
			echo "#### Modified configure.mk exists, remove manually..."; \
			echo "#### rm configure.mk"; \
			echo "##########################################################"; \
			false; \
		else \
			rm "configure.mk"; \
		fi \
	fi
	@cd tests && $(MAKE) BUILDDIR=.. distclean

test: playlister
	@cd tests && $(MAKE) BUILDDIR=.. TARGET=playlister

install: $(ITARGETS)
	$(MAKE) -f mk.skel ITARGETS="$(ITARGETS)" install

.PHONY: all clean dist-clean distclean

# vim: ft=make syntax=make
# EOF Makefile

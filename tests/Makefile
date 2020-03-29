TESTS=clean output nooutput config1 extended utils
UTILDEPS=$(BUILDDIR)/utils.o $(BUILDDIR)/str_len.o $(BUILDDIR)/str_start.o \
	$(BUILDDIR)/str_chr.o $(BUILDDIR)/str_diffn.o
CFLAGS+= -I$(BUILDDIR)
all: $(TESTS)

clean:
	-rm -f Test_List.m3u Test_List.test

test_utils: test_utils.c $(UTILDEPS)
	$(CC) -o test_utils test_utils.c $(UTILDEPS) $(CFLAGS)

utils: test_utils
	O1=`./test_utils 1`; \
	if [ "$${O1}" != "This is a test; a string with escapes." ]; then \
		echo "This is a test; a string with escapes."; \
		echo $${O1}; \
		echo Fail; \
		false; \
	fi
	O2=`./test_utils 2`; \
	if [ "$${O2}" != "; a string with escapes." ]; then \
		echo "; a string with escapes."; \
		echo $${O2}; \
		echo Fail; \
		false; \
	fi

config1:
	$(BUILDDIR)/$(TARGET) -v -v --conf test1.conf
	cat Test_List.test
	rm Test_List.test

output:
	$(BUILDDIR)/$(TARGET) -v -v --xml './iTunes Music Library.xml' \
		--out . --nolist --list 'Test List'
	cat Test_List.m3u
	rm Test_List.m3u

nooutput:
	$(BUILDDIR)/$(TARGET) -v -v --xml './iTunes Music Library.xml' \
		--out . --nolist --list 'Test List' --verify
	@echo "cat Test_List.m3u # Should be empty:"
	@cat Test_List.m3u
	@echo ":Should have been empty"
	rm Test_List.m3u

extended:
	$(BUILDDIR)/$(TARGET) -v -v --xml './iTunes Music Library.xml' \
		--out . --format extm3u --nolist --list 'Test List'
	grep '#' Test_List.m3u
	rm Test_List.m3u

# vim: sw=4 ts=4 noexpandtab

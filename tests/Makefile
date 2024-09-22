TESTS=clean output nooutput config1 extended utils
UTILDEPS=utils.o str_len.o str_start.o str_chr.o str_diffn.o
CFLAGS+= -I$(BUILDDIR)

ifndef BUILDDIR
$(error Don't use this directly try "cd .. && make test")
endif

all: $(TESTS)

clean:
	-rm -f Test_List.m3u Test_List.test

test_utils: test_utils.c $(UTILDEPS)
	$(CC) -o test_utils test_utils.c $(UTILDEPS) $(CFLAGS)

utils: test_utils
	@ O1=`./test_utils 1`; \
	if [ "$${O1}" != "This is a test; a string with escapes." ]; then \
		echo "This is a test; a string with escapes."; \
		echo $${O1}; \
		echo Fail; \
		false; \
	else \
		echo "test_utils 1 : passed"; \
	fi
	@ O2=`./test_utils 2`; \
	if [ "$${O2}" != "; a string with escapes." ]; then \
		echo "; a string with escapes."; \
		echo $${O2}; \
		echo Fail; \
		false; \
	else \
		echo "test_utils 2 : passed"; \
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

#############
# Dan J. Bernstein code (Public Domain) -- included in this package.
# Definitions included in utils.h
str_len.o: ../djb/str_len.c
	gcc -c $< -o $@ $(CFLAGS)

str_diffn.o: ../djb/str_diffn.c
	gcc -c $< -o $@ $(CFLAGS)

str_chr.o: ../djb/str_chr.c
	gcc -c $< -o $@ $(CFLAGS)

str_start.o: ../djb/str_start.c
	gcc -c $< -o $@ $(CFLAGS)

utils.o: ../utils.c
	gcc -c $< -o $@ $(CFLAGS)

clean:
	- rm -f *.o

dist-clean: clean
	- rm -f test_utils

# vim: sw=4 ts=4 noexpandtab

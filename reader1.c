/****************************************************************************
 * reader1.c
 * $Rev: 33 $
 *
 * Parse an iTunes XML file with xmlReader library
 *
 * Copyright (c) 2019, Gary Allen Vollink.  http://voll.ink/playlister
 * All rights reserved.
 *
 * Licence to use, see CDDLICENSE.txt file in this distribution.
 */
#define READER1_C 1
#include <stdio.h>
#include <libxml/xmlreader.h>
#include "utils.h"
#include "storage.h"
#include "reader1.h"

#ifndef LIBXML_READER_ENABLED

void
streamFile(const char *filename) {
    fprintf(stderr, "XInclude support not compiled in\n");
    exit(1);
}

#else

/**
 * _processNode:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
static void
_processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL) {
	    name = BAD_CAST "--";
    }

    value = xmlTextReaderConstValue(reader);

    // storage.c addition (to if zero this block)
    if (1) {
        superdebug("%d %d %s %d %d", 
            xmlTextReaderDepth(reader),
            xmlTextReaderNodeType(reader),
            name,
            xmlTextReaderIsEmptyElement(reader),
            xmlTextReaderHasValue(reader));
        if ( ( value == NULL ) || ( 15 == xmlTextReaderNodeType(reader) ) ) {
            superdebug("\n");
        }
        else {
            if (xmlStrlen(value) > 80)
                superdebug(" %.80s...\n", value);
            else
            superdebug(" %s\n", value);
        }
    }

    // storage.c addition...
    set_node( xmlTextReaderDepth(reader),
	    xmlTextReaderNodeType(reader),
        (char*)name,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader),
        (char*)value );
}

/**
 * function: streamFile
 * filename: the file name to parse
 *
 * Parse and print information about an XML file.
 */
void
streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            _processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            mywarning("%s : failed to parse\n", filename);
        }
    } else {
        mywarning("Unable to open file [%s].\n", filename);
    }

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
}

#endif
/**
vim: sw=4 ts=4 expandtab
 * EOF: reader1.c
 */

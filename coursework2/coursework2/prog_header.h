//
//  prog_header.h
//  coursework2
//
//  Created by Olly Seber on 23/11/2017.
//  Copyright © 2017 Olly Seber. All rights reserved.
//

#ifndef prog_header_h
#define prog_header_h

#include <stdio.h>

#include "uifunctions.h"
#include "firfilter.h"
#include "filehandler.h"


/* DATA TYPES */

enum ERR {
    NO_ERR,
    BAD_MEMORY,
    BAD_COMMAND_LINE,
    BAD_RUNTIME_ARG,
    OUT_OF_BOUNDS_VALUE
};


/* FUNCTION PROTOTYPES */

/*      cleanupMemory()
 * Helper function for freeing dynamicly allocated memory at end of program */
void cleanupMemory( audioFile *inputFile, audioFile *outputFile, firFilter *filter );

#endif // prog_header_h
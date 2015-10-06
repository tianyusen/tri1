#include "command.h"
#include "command-internals.h"
//#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
static function definitions, etc.  */
//Implementation

#include "alloc.h"    
// safely allocate memory (provided by skeleton)

#include <ctype.h>    
// define isalnum(): returns value different from zero (i.e., true) 
// if indeed c is either a digit or a letter. Zero (i.e., false) otherwise.

#include <limits.h>   // INT_MAX
//  #include <stdbool.h>  // required for boolean functions (ref)

#include <stdio.h>    // define EOF
#include <stdlib.h>   // to free memory

#include <string.h>  
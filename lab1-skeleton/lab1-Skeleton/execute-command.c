// UCLA CS 111 Lab 1 command execution

#include "command.h"
//#include "command-internals.h"

<<<<<<< HEAD
//#include <error.h>
=======
//+#include <error.h>
>>>>>>> 74ff8ba805069680fc2d2035d804143bba7ca320

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
<<<<<<< HEAD
  //error (1, 0, "command execution not yet implemented");
=======
  //+error (1, 0, "command execution not yet implemented");
>>>>>>> 74ff8ba805069680fc2d2035d804143bba7ca320

  //Implementation
  //Get rid of error dummy code
  	c->status;
  	time_travel;
  //End-Implementation
}

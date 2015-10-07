// UCLA CS 111 Lab 1 storage allocation

#include "alloc.h"

<<<<<<< HEAD
//#include <error.h>
=======
//+#include <error.h>
>>>>>>> 74ff8ba805069680fc2d2035d804143bba7ca320
#include <errno.h>
#include <stdlib.h>

static void
memory_exhausted (int errnum)
{
<<<<<<< HEAD
  //error (1, errnum, "memory exhausted");
=======
  //+error (1, errnum, "memory exhausted");
>>>>>>> 74ff8ba805069680fc2d2035d804143bba7ca320
}

static void *
check_nonnull (void *p)
{
  if (! p)
    memory_exhausted (errno);
  return p;
}

void *
checked_malloc (size_t size)
{
  return check_nonnull (malloc (size ? size : 1));
}

void *
checked_realloc (void *ptr, size_t size)
{
  return check_nonnull (realloc (ptr, size ? size : 1));
}

void *
checked_grow_alloc (void *ptr, size_t *size)
{
  size_t max = -1;
  if (*size == max)
    memory_exhausted (0);
  *size = *size < max / 2 ? 2 * *size : max;
  return checked_realloc (ptr, *size);
}

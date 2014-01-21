#include "stack.h"

#include <stdlib.h>
#include <string.h>
#include <err.h>


stackHead*            stackCreate(void)
{
  stackHead*          result;

  if (!(result = malloc(sizeof(stackHead))))
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory for stack head");

  memset(result, 0, sizeof(stackHead));

  return result;
}

int                   stackAdd(stackHead*                    stack,
                               void*                         elt)
{
  stackElt*           newElt;

  if (!stack)
    errx(EXIT_FAILURE, "ERROR: NULL stack");

  if (!(newElt = malloc(sizeof(stackElt))))
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory for stack element");

  newElt->data  = elt;
  newElt->next  = stack->first;
  stack->first  = newElt;
  stack->nbElt += 1;

  return 1;
}

void*                 stackRemove(stackHead*                 stack)
{
  void*               result;
  stackElt*           tmp;

  if (!stack)
    errx(EXIT_FAILURE, "ERROR: NULL stack");

  if (!stack->first) /* No element */
    return NULL;

  tmp               = stack->first;
  result            = tmp->data;
  stack->first      = tmp->next;
  stack->nbElt     -= 1;
  free(tmp);

  return result;
}

void                  stackFree(stackHead*                   stack)
{
  stackElt*           tmp;

  if (!stack)
    errx(EXIT_FAILURE, "ERROR: NULL stack");

  while (!!stack->first)
  {
    tmp          = stack->first;
    stack->first = tmp->next;

    free(tmp);
  }

  memset(stack, 0, sizeof(stackHead));
  free(stack);
}

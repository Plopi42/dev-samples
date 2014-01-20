#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include <err.h>


queue*                queueCreate(void)
{
  queue*              result;

  if (!(result = malloc(sizeof(queue))))
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory for queue");

  memset(result, 0, sizeof(queue));

  return result;
}

int                   queueAdd(queue*                        queueHead,
                               void*                         elt)
{
  queue*              newElt;

  if (!queueHead)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  newElt       = queueCreate();
  newElt->data = elt;

  if (!queueHead->next) /* No element */
  {
    if (!!queueHead->prev)
    {
      warnx("ERROR: Invalid state for queue");
      return 0;
    }

    queueHead->prev  = newElt;
    queueHead->next  = newElt;

    return 1;
  }

  newElt->next       = queueHead->next;
  newElt->next->prev = newElt;
  queueHead->next    = newElt;

  return 1;
}

void*                 queueRemove(queue*                     queueHead)
{
  void*               result;
  queue*              tmp;

  if (!queueHead)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  if (!queueHead->prev) /* No element */
    return NULL;

  tmp             = queueHead->prev;
  result          = tmp->data;
  queueHead->prev = tmp->prev;

  if (!tmp->prev)
    queueHead->next = NULL;
  else
    tmp->prev->next = NULL;


  free(tmp);

  return result;
}

void                  queueFree(queue*                       queueHead)
{
  if (!queueHead)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  while (!!queueHead->next)
  {
    queueHead->prev = queueHead->next;
    queueHead->next = queueHead->next->next;

    free(queueHead->prev);
  }

  memset(queueHead, 0, sizeof(queue));

  free(queueHead);
}

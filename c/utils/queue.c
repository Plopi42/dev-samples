#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include <err.h>


queueHead*            queueCreate(void)
{
  queueHead*          result;

  if (!(result = malloc(sizeof(queueHead))))
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory for queue head");

  memset(result, 0, sizeof(queueHead));

  return result;
}

int                   queueAdd(queueHead*                    queue,
                               void*                         elt)
{
  queueElt*           newElt;

  if (!queue)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  if (!(newElt = malloc(sizeof(queueElt))))
    errx(EXIT_FAILURE, "ERROR: Cannot allocate memory for queue element");

  newElt->data = elt;
  newElt->next = NULL;

  if (!queue->first || !queue->last) /* No element */
  {
    if (queue->last != queue->first)
    {
      warnx("ERROR: Invalid state for queue");
      return 0;
    }

    queue->first = newElt;
    queue->last  = newElt;
    queue->nbElt = 1;

    return 1;
  }

  queue->last->next = newElt;
  queue->last       = newElt;
  queue->nbElt     += 1;

  return 1;
}

void*                 queueRemove(queueHead*                 queue)
{
  void*               result;
  queueElt*           tmp;

  if (!queue)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  if (!queue->first) /* No element */
    return NULL;

  tmp               = queue->first;
  result            = tmp->data;
  queue->first      = tmp->next;
  queue->nbElt     -= 1;

  if (!queue->first)
  {
    queue->last     = NULL;
    queue->nbElt    = 0;
  }

  free(tmp);

  return result;
}

void*                 queueFirst(queueHead*                  queue)
{
  if (!queue)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  if (!queue->first) /* No element */
    return NULL;

  return queue->first->data;
}

void                  queueFree(queueHead*                   queue)
{
  if (!queue)
    errx(EXIT_FAILURE, "ERROR: NULL queue");

  while (!!queue->first)
  {
    queue->last  = queue->first;
    queue->first = queue->last->next;

    free(queue->last);
  }

  memset(queue, 0, sizeof(queueHead));
  free(queue);
}

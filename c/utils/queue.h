#ifndef UTILS__QUEUE_H_
# define UTILS__QUEUE_H_

/**
 ** Structure
 */
typedef struct                   queueElt
{
  void*                          data;
  struct queueElt*               next;
}                                queueElt;

typedef struct                   queueHead
{
  struct queueElt*               first;
  struct queueElt*               last;
  unsigned int                   nbElt;
}                                queueHead;


/**
 ** Methods
 */

/**
 ** Create a new queue object.
 **
 ** \return An initialized queue structure or NULL.
 */
queueHead*            queueCreate(void);

/**
 ** Add a new element to an existing queue.
 **
 ** \param  queue       The queue object.
 ** \param  elt         The element to insert.
 **
 ** \return 1 if ok, else 0.
 */
int                   queueAdd(queueHead*                    queue,
                               void*                         elt);

/**
 ** Remove and return the older element in the queue.
 **
 ** \param  queue       The queue object.
 **
 ** \return The older object in the queue.
 */
void*                 queueRemove(queueHead*                 queue);

/**
 ** Free a queue object properly
 **
 ** \param  queue       The queue object.
 */
void                  queueFree(queueHead*                   queue);


#endif /* UTILS__QUEUE_H_ */

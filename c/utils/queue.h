#ifndef UTILS__QUEUE_H_
# define UTILS__QUEUE_H_

/**
 ** Structure
 */
typedef struct                   queue
{
  void*                          data;
  struct queue*                  prev;
  struct queue*                  next;
}                                queue;


/**
 ** Methods
 */

/**
 ** Create a new queue object.
 **
 ** \return An initialized queue structure or NULL.
 */
queue*                queueCreate(void);

/**
 ** Add a new element to an existing queue.
 **
 ** \param  queue       The queue object.
 ** \param  elt         The element to insert.
 **
 ** \return 1 if ok, else 0.
 */
int                   queueAdd(queue*                        queueHead,
                               void*                         elt);

/**
 ** Remove and return the older element in the queue.
 **
 ** \param  queue       The queue object.
 **
 ** \return The older object in the queue.
 */
void*                 queueRemove(queue*                     queueHead);

/**
 ** Free a queue object properly
 **
 ** \param  queue       The queue object.
 */
void                  queueFree(queue*                       queueHead);

#endif /* UTILS__QUEUE_H_ */

#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int                   main(void)
{
  queueHead*          queue;
  int                 i1 = 1;
  int                 i2 = 2;
  int                 i3 = 3;
  int                 i4 = 4;

  /*
   * Test 1
   */

  /* Create queue */
  assert(NULL != (queue = queueCreate()));

  /* Fill the queue */
  assert(1    == (queueAdd(queue, &i1)));      /* Insert 1 */
  assert(1    == (queueAdd(queue, &i2)));      /* Insert 2 */
  assert(1    == (queueAdd(queue, &i3)));      /* Insert 3 */
  assert(1    == (queueAdd(queue, &i4)));      /* Insert 4 */

  /* Check the queue */
  assert(1    == (*(int*)queueRemove(queue))); /* Remove 1 */
  assert(2    == (*(int*)queueRemove(queue))); /* Remove 2 */
  assert(3    == (*(int*)queueRemove(queue))); /* Remove 3 */
  assert(4    == (*(int*)queueRemove(queue))); /* Remove 4 */

  assert(NULL    == (queueRemove(queue)));     /* Queue is empty */

  /* Clear the queue */
  queueFree(queue);

  printf("Queue: Test1 success!\n");

  /*
   * Test 2
   */

  /* Create queue */
  assert(NULL != (queue = queueCreate()));

  /* Fill the queue */
  assert(1    == (queueAdd(queue, &i1)));      /* Insert 1 */
  assert(1    == (*(int*)queueRemove(queue))); /* Remove 1 */

  assert(1    == (queueAdd(queue, &i1)));      /* Insert 1 */
  assert(1    == (queueAdd(queue, &i2)));      /* Insert 2 */
  assert(1    == (*(int*)queueRemove(queue))); /* Remove 1 */
  assert(2    == (*(int*)queueRemove(queue))); /* Remove 2 */

  assert(1    == (queueAdd(queue, &i1)));      /* Insert 1 */
  assert(1    == (queueAdd(queue, &i2)));      /* Insert 2 */
  assert(1    == (*(int*)queueRemove(queue))); /* Remove 1 */
  assert(1    == (queueAdd(queue, &i3)));      /* Insert 3 */
  assert(2    == (*(int*)queueRemove(queue))); /* Remove 2 */
  assert(1    == (queueAdd(queue, &i4)));      /* Insert 4 */

  assert(3    == (*(int*)queueRemove(queue))); /* Remove 3 */
  assert(4    == (*(int*)queueRemove(queue))); /* Remove 4 */

  /* Fill the queue */
  assert(1    == (queueAdd(queue, &i1)));      /* Insert 1 */
  assert(1    == (queueAdd(queue, &i2)));      /* Insert 2 */
  assert(1    == (queueAdd(queue, &i3)));      /* Insert 3 */
  assert(1    == (queueAdd(queue, &i4)));      /* Insert 4 */

  /* Check the queue */
  assert(1    == (*(int*)queueRemove(queue))); /* Remove 1 */
  assert(2    == (*(int*)queueRemove(queue))); /* Remove 2 */
  assert(3    == (*(int*)queueRemove(queue))); /* Remove 3 */
  assert(4    == (*(int*)queueRemove(queue))); /* Remove 4 */

  assert(NULL    == (queueRemove(queue)));     /* Queue is empty */

  /* Clear the queue */
  queueFree(queue);

  printf("Queue: Test2 success!\n");

  return 0;
}

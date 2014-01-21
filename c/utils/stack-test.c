#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int                   main(void)
{
  stackHead*          stack;
  int                 i1 = 1;
  int                 i2 = 2;
  int                 i3 = 3;
  int                 i4 = 4;

  /*
   * Test 1
   */

  /* Create stack */
  assert(NULL != (stack = stackCreate()));

  /* Fill the stack */
  assert(1    == (stackAdd(stack, &i1)));      /* Insert 1 */
  assert(1    == (stackAdd(stack, &i2)));      /* Insert 2 */
  assert(1    == (stackAdd(stack, &i3)));      /* Insert 3 */
  assert(1    == (stackAdd(stack, &i4)));      /* Insert 4 */

  /* Check the stack */
  assert(4    == (*(int*)stackRemove(stack))); /* Remove 4 */
  assert(3    == (*(int*)stackRemove(stack))); /* Remove 3 */
  assert(2    == (*(int*)stackRemove(stack))); /* Remove 2 */
  assert(1    == (*(int*)stackRemove(stack))); /* Remove 1 */

  assert(NULL    == (stackRemove(stack)));     /* Stack is empty */

  /* Clear the stack */
  stackFree(stack);

  printf("Stack: Test1 success!\n");

  /*
   * Test 2
   */

  /* Create stack */
  assert(NULL != (stack = stackCreate()));

  /* Fill the stack */
  assert(1    == (stackAdd(stack, &i1)));      /* Insert 1 */
  assert(1    == (*(int*)stackRemove(stack))); /* Remove 1 */

  assert(1    == (stackAdd(stack, &i1)));      /* Insert 1 */
  assert(1    == (stackAdd(stack, &i2)));      /* Insert 2 */
  assert(2    == (*(int*)stackRemove(stack))); /* Remove 2 */
  assert(1    == (*(int*)stackRemove(stack))); /* Remove 1 */

  assert(1    == (stackAdd(stack, &i1)));      /* Insert 1 */
  assert(1    == (stackAdd(stack, &i2)));      /* Insert 2 */
  assert(2    == (*(int*)stackRemove(stack))); /* Remove 2 */
  assert(1    == (stackAdd(stack, &i3)));      /* Insert 3 */
  assert(3    == (*(int*)stackRemove(stack))); /* Remove 3 */
  assert(1    == (stackAdd(stack, &i4)));      /* Insert 4 */

  assert(4    == (*(int*)stackRemove(stack))); /* Remove 4 */
  assert(1    == (*(int*)stackRemove(stack))); /* Remove 1 */

  /* Fill the stack */
  assert(1    == (stackAdd(stack, &i1)));      /* Insert 1 */
  assert(1    == (stackAdd(stack, &i2)));      /* Insert 2 */
  assert(1    == (stackAdd(stack, &i3)));      /* Insert 3 */
  assert(1    == (stackAdd(stack, &i4)));      /* Insert 4 */

  /* Check the stack */
  assert(4    == (*(int*)stackRemove(stack))); /* Remove 4 */
  assert(3    == (*(int*)stackRemove(stack))); /* Remove 3 */
  assert(2    == (*(int*)stackRemove(stack))); /* Remove 2 */
  assert(1    == (*(int*)stackRemove(stack))); /* Remove 1 */

  assert(NULL    == (stackRemove(stack)));     /* Stack is empty */

  /* Clear the stack */
  stackFree(stack);

  printf("Stack: Test2 success!\n");

  return 0;
}

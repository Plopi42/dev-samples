#ifndef UTILS__STACK_H_
# define UTILS__STACK_H_

/**
 ** Structure
 */
typedef struct                   stackElt
{
  void*                          data;
  struct stackElt*               next;
}                                stackElt;

typedef struct                   stackHead
{
  struct stackElt*               first;
  unsigned int                   nbElt;
}                                stackHead;


/**
 ** Methods
 */

/**
 ** Create a new stack object.
 **
 ** \return An initialized stack structure or NULL.
 */
stackHead*            stackCreate(void);

/**
 ** Add a new element to an existing stack.
 **
 ** \param  stack       The stack object.
 ** \param  elt         The element to insert.
 **
 ** \return 1 if ok, else 0.
 */
int                   stackAdd(stackHead*                    stack,
                               void*                         elt);

/**
 ** Remove and return the older element in the stack.
 **
 ** \param  stack       The stack object.
 **
 ** \return The older object in the stack.
 */
void*                 stackRemove(stackHead*                 stack);

/**
 ** Free a stack object properly
 **
 ** \param  stack       The stack object.
 */
void                  stackFree(stackHead*                   stack);


#endif /* UTILS__STACK_H_ */

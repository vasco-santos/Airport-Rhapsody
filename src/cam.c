/**
 *  \file cam.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  CAM management.
 *
 *  The following operations are defined:
 *     \li initialization
 *     \li search for a value
 *     \li insertion of a value
 *     \li retrieval of a value
 *     \li test for CAM full
 *     \li test for CAM empty.
 *
 *  \author António Rui Borges - November 2013
 */

#ifndef CAM_H_
#define CAM_H_

#include <stdio.h>
#include <stdbool.h>

#include "probDataStruct.h"

/**
 *  \brief CAM initialization.
 *
 *         CAM will be empty after it.
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 */

void camInit (CAM *p_c)
{
  if (p_c == NULL) return;
  p_c->n = 0;
}

/**
 *  \brief Search for a value in the CAM.
 *
 *         The key is in this case the value itself.
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be searched
 *
 *  \return \c true, if the value is found
 *  \return \c false, otherwise
 */

bool camSearch (CAM *p_c, unsigned int val)
{
  unsigned int i;                      /* counting variable */

  if (p_c == NULL) return false;
  for (i = 0; i < p_c->n; i++)
    if (p_c->mem[i].id == val)
       return true;
       else if (p_c->mem[i].id > val)
               return false;
  return false;
}

/**
 *  \brief Insertion of a value into the CAM.
 *
 *         The key is in this case the value itself to be stored.
 *         The function fails if a null pointer is passed as a parameter or the CAM is full.
 *         Nothing is stored if the function fails.
 *         If an equal value is already stored in the CAM, the new value is stored in FIFO order.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be stored
 */

void camIn (CAM *p_c, unsigned int val)
{
  unsigned int i, j;                   /* counting variables */

  if ((p_c == NULL) || (p_c->n == M*N)) return;
  if (p_c->n == 0)
     /* CAM is empty */
     p_c->mem[0].id = val;
     else if (p_c->mem[0].id > val)
             { /* insertion at the beginning of the array */
               for (i = p_c->n; i > 0; i--)
                 p_c->mem[i] = p_c->mem[i-1];
               p_c->mem[0].id = val;
             }
             else { /* insertion somewhere else */
                    for (i = 1; i < p_c->n; i++)
                      if (p_c->mem[i].id > val) break;
                    for (j = p_c->n; j > i; j--)
                      p_c->mem[j] = p_c->mem[j-1];
                    p_c->mem[i].id = val;
                  }
  p_c->n += 1;
}

/**
 *  \brief Retrieval of a value from the CAM.
 *
 *         The key is in this case the value itself to be retrieved.
 *         The function fails if a null pointer is passed as a parameter or the CAM is empty or no value is presently
 *         stored there.
 *         Nothing is retrieved if the function fails.
 *         If more than one value is stored in the CAM, the extracted value is retrieved in FIFO order.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be retrieved
 */

void camOut (CAM *p_c, unsigned int val)
{
  unsigned int i, j;                   /* counting variables */

  if ((p_c == NULL) || (p_c->n == 0)) return;
  for (i = 0; i < p_c->n; i++)
    if (p_c->mem[i].id == val)
       { for (j = i + 1; j < p_c->n; j++)
           p_c->mem[j-1] = p_c->mem[j];
         p_c->n -= 1;
         break;
       }
}

/**
 *  \brief Test for CAM full.
 *
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *
 *  \return \c true, if the CAM is full
 *  \return \c false, otherwise
 */

bool camFull (CAM *p_c)
{
  if (p_c == NULL) return false;
  return p_c->n == M*N;
}

/**
 *  \brief Test for CAM empty.
 *
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *
 *  \return \c true, if the CAM is empty
 *  \return \c false, otherwise
 */

bool camEmpty (CAM *p_c)
{
  if (p_c == NULL) return false;
  return p_c->n == 0;
}

#endif /* CAM_H_ */
